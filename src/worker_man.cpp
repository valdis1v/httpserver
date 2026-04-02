

#include "http_def.h"
#include "logger.h"
#include "res_man.h"
#include <queue>
#include <thread>
#include <unistd.h>
struct Request_Wrapper {
    HttpRequest request;
    int connection_fd;
};

#include <condition_variable>
#include <mutex>
class Worker_Manager
{
    std::mutex lock_q;
    std::condition_variable cv_wait_jobs;
    std::queue<Request_Wrapper> jobs;

    bool verbose;
    bool RUNNING = true;
    Ressource_Manager& manager;

    public:

        Worker_Manager(Ressource_Manager& man, bool verbose): manager(man), verbose(verbose) {
            write_log("Starting workers...", 1);
        }

        void push_job(HttpRequest req, int fd) noexcept {
            {
                std::lock_guard<std::mutex> lockdown(lock_q);
                this->jobs.push({req, fd});
            }
            cv_wait_jobs.notify_one();
        }

        void thread_job() {
            // Lock
            if(verbose) {
                write_log("Started thread", 1);
            }
            while(RUNNING) {
                Request_Wrapper job;
                {
                    std::unique_lock<std::mutex> lock(lock_q);
                    cv_wait_jobs.wait(lock, [this] {
                        return !jobs.empty();
                    });
                    job = std::move(jobs.front());
                    jobs.pop();
                }
                try  {
                    auto ressource = manager.request_or_fallback(job.request.path);
                    HttpResponse response = HttpResponse::OK(Html, std::string(ressource));
                    auto out = response.into_writable();
                    write(job.connection_fd, out.data(), out.size());
                    close(job.connection_fd);
                } catch (...) {
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

};
