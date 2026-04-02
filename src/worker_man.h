
#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

#include "http_def.h"
#include <condition_variable>
#include <mutex>
#include "http_def.h"
#include "logger.h"
#include "res_man.h"
#include <atomic>
#include <queue>
#include <thread>
#include <unistd.h>
#include <vector>

struct Request_Wrapper {
    HttpRequest request;
    int connection_fd;
};

class Worker_Manager {
    private:
    std::mutex lock_q;
    std::condition_variable cv_wait_jobs;
    std::queue<Request_Wrapper> jobs;

    std::vector<std::thread> workers;

    bool verbose;
    std::atomic_bool RUNNING = true;
    Ressource_Manager& manager;

    public:
    explicit Worker_Manager(Ressource_Manager& man, bool verbose);
    ~Worker_Manager();
    void push_job(HttpRequest req, int fd) noexcept;
    void thread_job();


};


#endif
