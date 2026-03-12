#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

struct Task
{
    char* request_dummy;
    int fd;
};

class Worker_Manager
{
private:
    std::queue<Task> job_queue;
    std::mutex lock_queue;
    std::condition_variable new_job;
    bool terminate;
    std::vector<std::thread> workers;

    void worker_fn();

public:
    Worker_Manager();
    ~Worker_Manager();

    void queue_job(Task t);
};

#endif // WORKER_MANAGER_H
