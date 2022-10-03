#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include <iostream>
#include <memory>
#include "Epoller.h"

class Handler;

class Epoller;

class EventLoop {
public:
    EventLoop();
    ~EventLoop();
    void updateHandler(Handler *handler);
    void loop();

private:
    std::unique_ptr<Epoller> poller_;
};

EventLoop::EventLoop()
{
    poller_ = std::make_unique<Epoller>();
}

EventLoop::~EventLoop()
{

}

void EventLoop::updateHandler(Handler *handler)
{
    poller_->update(handler); // tech debt
}

void EventLoop::loop()
{
    while(1)
    {
        auto activeEventPtrLists = poller_->poll();
        if(!activeEventPtrLists.size())
        {
            for(int i = 1; i < activeEventPtrLists.size(); i++)
            {
                activeEventPtrLists[i]->handleEvent();
            }
        }
    }
}

#endif