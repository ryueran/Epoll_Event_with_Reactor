#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <iostream>
#include <functional>
#include <thread>
#include <sys/epoll.h>

#define MAX_BUFFLEN_ 1024

enum event_status 
{
    knew = 0,
    kadd = 1,
    kdelete = 2
};

class Reactor;

class Event_Handler { // observal
public:

    Event_Handler(int fd, Reactor *loop);
    ~Event_Handler();
    
    void set_event_type(uint32_t type);
    uint32_t event();
    void handle_event();

    void set_read_actor(std::function<void(int)> read_task);
    void set_write_actor(std::function<void(int)> write_task);
    void set_status(event_status status);
    event_status get_status();
    int Fd();

    void update();
    void remove();

    void enableReading()
    {
        type_ = EPOLLIN;
        update();
    }

    void enableWriting()
    {
        type_ = EPOLLOUT;
        update();
    }

    char* buff()
    {
        return buff_;
    }

private:
    std::function<void(int)> read_actor_;
    std::function<void(int)> write_actor_;
    uint32_t type_;
    uint32_t r_type_;
    event_status e_status_;
    int fd_;
    Reactor *loop_;
    char buff_[MAX_BUFFLEN_];
};

#endif