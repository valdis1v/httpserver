#ifndef WORKERMAN_H
#define WORKERMAN_H

#include "worker_fn.h"
#include <queue>
#include <thread>


// WorkerManager für das Management von Workern und Warteschlangen
class WorkerManager
{
public:
    WorkerManager();

private:
    std::queue<HttpRequest> response_queue;

    void observe_respond();
};

#endif
