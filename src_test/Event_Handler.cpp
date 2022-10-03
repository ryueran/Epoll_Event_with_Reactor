#include <iostream>
#include "Event_Handler.h"
#include "Reactor.h"

Event_Handler::Event_Handler(int fd, Reactor *loop)
{
    std::cout << "Event_Handler constructed!" << std::endl;
    fd_ = fd;
    loop_ = loop;
}

Event_Handler::~Event_Handler()
{
    std::cout << "Event_Handler destructed!" << std::endl;
}

void Event_Handler::set_event_type(uint32_t type)
{
    r_type_ = type;
}

void Event_Handler::set_read_actor(std::function<void(int)> read_task)
{
    read_actor_ = read_task;
}

void Event_Handler::set_write_actor(std::function<void(int)> write_task)
{
    write_actor_ = write_task;
}

void Event_Handler::handle_event()
{
    if(r_type_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
    {
        read_actor_(fd_);
    }

    if(r_type_ & EPOLLOUT)
    {
        write_actor_(fd_);
    }
}

event_status Event_Handler::get_status()
{
    return e_status_;
}

int Event_Handler::Fd()
{
    return fd_;
}

void Event_Handler::set_status(event_status status)
{
    e_status_ = status;
}

uint32_t Event_Handler::event()
{
    return type_;
}

void Event_Handler::update()
{
    loop_->register_handler(this);
}

void Event_Handler::remove()
{
    loop_->remove_handler(this);
}