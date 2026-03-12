
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>



struct Task
{
    char* request_dummy;
    int fd; // filedesc um client zu antworten
};



#include <queue>
class Worker_Manager
{

    std::queue<Task> job_queue;
    std::mutex lock_queue;
    std::condition_variable new_job;

    bool terminate = false;

    std::vector<std::thread> workers;

    void worker_fn()
    {
        while(true) {
            Task task;
            {
                // doppelscope für lock
                std::unique_lock<std::mutex> lock(this->lock_queue);
                new_job.wait(lock, [this] {return !job_queue.empty() || terminate; });


                if(terminate && job_queue.empty())
                {
                    return;
                }
                task = job_queue.front();
                job_queue.pop();
            }
            // finish job.
        }
    }


    public:

    void queue_job(Task t)
    {
        // immer locken
        {
            std::unique_lock<std::mutex> lock(this->lock_queue);
            job_queue.push(t);
        }
        new_job.notify_one();
    }

    Worker_Manager()
    {
        // def 5
        for(int i = 0; i < 4; i++)
        {
            workers.emplace_back(&Worker_Manager::worker_fn, this);
        }

    };
    ~Worker_Manager()
    {
        // doppelscope for lock
        {
            std::unique_lock<std::mutex> lock(this->lock_queue);
            terminate = true;
        }
        new_job.notify_all();
        for(auto &thr: workers)
        {
            if(thr.joinable()) thr.join();
        }
    };


};
