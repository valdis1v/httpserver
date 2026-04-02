#include "http_def.h"
#include "logger.h"
#include "res_man.h"
#include <atomic>
#include <queue>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <vector>
#include "worker_man.h"

Worker_Manager::Worker_Manager(Ressource_Manager& man, bool verbose): manager(man), verbose(verbose) {
    write_log("Starting workers...", 1);
    int x = 8;
    while(x-->0) {
        workers.emplace_back(
            &Worker_Manager::thread_job,
            this
        );
    }
}

Worker_Manager::~Worker_Manager() {
    RUNNING.store(false);
    cv_wait_jobs.notify_all();
    for(auto& worker: workers) {
        worker.join();
    }
}

void Worker_Manager::push_job(HttpRequest req, int fd) noexcept {
    {
        std::lock_guard<std::mutex> lockdown(lock_q);
        this->jobs.push({req, fd});
    }
    cv_wait_jobs.notify_one();
}

void Worker_Manager::thread_job() {
    if(verbose) {
        write_log("Started thread", 1);
    }
    while(RUNNING) {
        Request_Wrapper job;
        std::stringstream stream;
        stream << std::this_thread::get_id();
        std::string id = stream.str();
        {
            std::unique_lock<std::mutex> lock(lock_q);
            cv_wait_jobs.wait(lock, [this] {
                return !jobs.empty();
            });
            job = std::move(jobs.front());
            jobs.pop();
        }
        try  {
            write_log(std::string("Thread ") + id + "\n\t" + std::string("Sending response: ") + job.request.path, 1);
            auto res = manager.request_ressource(job.request.path);
            auto type = contenttype_from(job.request.path);
            HttpResponse response;
            if(res.empty()) {
                if(type == Html) {
                    res = manager.request_error_page("404");
                } else {
                    res = "";
                }
                response = HttpResponse::NotFound(type, std::string(res));
                write_log(response.into_writable(), 2);
            } else {
                response = HttpResponse::OK(type, std::string(res));
                write_log(response.into_writable(), 2);
            }
            auto out = response.into_writable();
            write(job.connection_fd, out.data(), out.size());
            close(job.connection_fd);
        } catch (...) {
            auto res = manager.request_error_page("500");
            HttpResponse response = HttpResponse::IntServerError(Html, std::string(res));
            write(job.connection_fd, response.into_writable().data(), response.into_writable().size());
            if(verbose) {
                write_log(
                    std::string("Failed to handle request: ")
                    + job.request.path
                    , 3
                );
            }
            if(close(job.connection_fd) == 0) {
                if(verbose)
                write_log("Failed to close connection", 3);
            }
        }
    }
}
