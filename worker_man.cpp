#include <queue>
#include <thread>

#include "fileman.h"
#include "worker_fn.h"

struct HttpResponseDummy;

class WorkerManager
{
    public:
        WorkerManager(FileManager file_manager): file_manager(file_manager)
        {

        }

    private:

        FileManager file_manager;

        std::queue<HttpRequest> request_queue;

        void observe_respond()
        {
            while (true) {
                if(request_queue.empty()) {
                    std::this_thread::yield();
                } else {
                    HttpRequest& hr = this->request_queue.back();
                    int size;
                    auto resource = file_manager.get_ressource(hr.requested_resource, size);
                    this->request_queue.pop();
                    char* response;
                    int response_size;
                    if(resource == nullptr)
                    {
                        response = http_generic_error(size);
                    }
                    else
                    {
                        response = http_ok_response(resource, size, response_size);
                    }
                }
            }
        }

};
