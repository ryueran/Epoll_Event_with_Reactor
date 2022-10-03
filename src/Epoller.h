#ifndef EPOLLER_H_
#define EPOLLRT_H_

#include <iostream>
#include <vector>
#include <string.h>
#include <sys/epoll.h>
#include "Handler.h"

class Handler;

class Epoller {
public:
    Epoller();
    ~Epoller();
    void update(Handler *handler);
    std::vector<Handler*> fillActiveChannels(int activeNums);
    std::vector<Handler*> poll();

    
private:
    static const int number_active_events = 16;
    std::vector<epoll_event> active_events_;
    std::vector<Handler*> handler_list_;
    int epoll_fd_;
};

Epoller::Epoller() : active_events_(number_active_events)
{
    epoll_fd_ = epoll_create(EPOLL_CLOEXEC);
}

Epoller::~Epoller()
{

}

void Epoller::update(Handler *handler)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = handler->events(); // Tech debt
    event.data.ptr = handler;
    int fd = handler->fd(); // Tech debt

    int ret;
    if(handler->operation() == EPOLL_CTL_ADD)
    {
        ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
    } else if (handler->operation() == EPOLL_CTL_DEL) {
        ret = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &event);
    } else {
        ret = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event);
    }

    if(ret < 0)
    {
        std::cout << "epoll_ctl error!" << std::endl;
    }
}

std::vector<Handler*> Epoller::fillActiveChannels(int activeNums)
{
    std::vector<Handler*> activeEventList;

    for(int i = 0; i < activeNums; i++)
    {
        Handler* handlerPtr = static_cast<Handler*>(active_events_[i].data.ptr);
        if(active_events_[i].events & EPOLLIN)
        {
            handlerPtr->set_event_types(EPOLLIN);
        }

        if(active_events_[i].events & EPOLLOUT)
        {
            handlerPtr->set_event_types(EPOLLOUT);
        }

        activeEventList.push_back(handlerPtr);
    }

    return activeEventList;
}

std::vector<Handler*> Epoller::poll()
{
    std::vector<Handler*> activeEventPtrList;

    int nready = epoll_wait(epoll_fd_, active_events_.data(), active_events_.size(), -1);
    if(nready > 0)
    {
        activeEventPtrList = fillActiveChannels(nready);
        if(nready == active_events_.size())
        {
            active_events_.resize(active_events_.size() * 2);
        }
    }
    return activeEventPtrList;
}

#endif