#ifndef EPOLLER_H_
#define EPOLLER_H_

#include <vector>
#include <map>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/epoll.h>
#include "Event_Handler.h"

// need a state machine for event, technical debt
class Event_Handler;

class Epoller
{
public:
    Epoller();
    ~Epoller();

    void add_Handler(Event_Handler *eh);
    void remove_Handler(Event_Handler *eh);
    void Epoll_Wait(std::vector<Event_Handler*>* active_handlers);

private:
    std::map<int, Event_Handler*> handlerMap_;
    int epoll_fd_;
    static const int initEventSize = 32;
    std::vector<epoll_event> event_list_;

    // handles the epoll_event && epoll_ctl
    // epoll_wait is the consumer of producer epoll_ctl
    void update_(int operation, Event_Handler *eh);
    void fill_Handlers_(std::vector<Event_Handler*>* activeHandlers, int nums);
};



#endif