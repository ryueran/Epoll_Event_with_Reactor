#include <iostream>
#include <errno.h>
#include "Epoller.h"
#include "Event_Handler.h"
#include "Reactor.h"

Epoller::Epoller()
: event_list_(16)
{
    std::cout << "Epoller constructed" << std::endl;
    epoll_fd_ = epoll_create(EPOLL_CLOEXEC);
}

Epoller::~Epoller()
{
    close(epoll_fd_);
}

void Epoller::add_Handler(Event_Handler *eh)
{
    int status = eh->get_status();
    if(status == knew || status == kdelete)
    {
        int fd = eh->Fd();
        if(status == knew)
        {
            assert(handlerMap_.find(fd) == handlerMap_.end());
            handlerMap_[fd] = eh;
        } else {
            assert(handlerMap_.find(fd) != handlerMap_.end());
            assert(handlerMap_[fd] == eh);
        }

        eh->set_status(kadd);
        std::cout << "Add epoll event" << std::endl;
        update_(EPOLL_CTL_ADD, eh);
    } else {
        int fd = eh->Fd();
        assert(handlerMap_.find(fd) != handlerMap_.end());
        std::cout << "Mod epoll event" << std::endl;
        update_(EPOLL_CTL_MOD, eh);
    }
}

void Epoller::remove_Handler(Event_Handler* eh)
{
    int fd = eh->Fd();
    //assert(handlerMap_.find(fd) != handlerMap_.end());
    eh->set_status(kdelete);
    handlerMap_.erase(fd);
    update_(EPOLL_CTL_DEL, eh);
}

void Epoller::Epoll_Wait(std::vector<Event_Handler*>* active_handlers)
{
    int event_num = epoll_wait(epoll_fd_, event_list_.data(), event_list_.size(), 500);

    if(event_num == -1)
    {
        if(errno == EINTR)
        {
            std::cout << "process error" << std::endl;
        } else {
            std::cout << "other errors" << std::endl;
        }
    }

    if(event_num > 0)
    {
        fill_Handlers_(active_handlers, event_num);

        if(event_num == static_cast<int>(event_list_.size()))
        {
            event_list_.resize(event_list_.size() * 2);
        }
    } else {
        
    }
}

void Epoller::update_(int operation, Event_Handler *eh)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = eh->event(); // technical debt
    event.data.ptr = eh;
    int fd = eh->Fd(); // technical debt
    if(epoll_ctl(epoll_fd_, operation, fd, &event))
    {
        std::cout << "epoll_ctl fails" << std::endl;
    } else {
        std::cout << "epoll_ctl success" << std::endl;
    }
}

void Epoller::fill_Handlers_(std::vector<Event_Handler*>* activeHandlers, int nums)
{
   for(int i = 0; i < nums; i++)
   {
       Event_Handler* handler = static_cast<Event_Handler*>(event_list_[i].data.ptr);
       int fd = handler->Fd(); // technical debt
       auto it = handlerMap_.find(fd);

       if(it == handlerMap_.end())
       {
           std::cout << "Event lost" << std::endl;
       }
   
       handler->set_event_type(event_list_[i].events); // change type. technical debt
       activeHandlers->push_back(handler);
   }
}