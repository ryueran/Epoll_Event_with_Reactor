#ifndef HANDLER_H_
#define HANDLER_H_

#include <iostream>
#include <functional>
#include <sys/epoll.h>

typedef std::function<void(int)> EventCallback;

class Handler {
public:
    Handler(int fd);
    ~Handler();

    void setCLoseFLag(bool i_close);
    void handleEvent();
    void setReadCallback(EventCallback);
    void setWriteCallback(EventCallback);
    void setCloseCallback(EventCallback);
    void setOperation(int operation);

    void set_event_types(int events);
    int events();
    int fd();
    int operation();

private:
    int fd_;
    int events_;
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    bool if_close_;
    int operator_;

};

Handler::Handler(int fd)
{
    fd_ = fd;
}

Handler::~Handler()
{

}

void Handler::setOperation(int operation)
{
    operator_ = operation;
}

int Handler::operation()
{
    return operator_;
}

int Handler::fd()
{
    return fd_;
}

void Handler::set_event_types(int events)
{
    events_ = events;
}

int Handler::events()
{
    return events_;
}

void Handler::setCLoseFLag(bool i_close)
{
    if_close_ = i_close;
}

void Handler::setReadCallback(EventCallback readCallback)
{
    readCallback_ = readCallback;
}

void Handler::setWriteCallback(EventCallback writeCallback)
{
    writeCallback_ = writeCallback;
}

void Handler::setCloseCallback(EventCallback closeCallback)
{
    closeCallback_ = closeCallback;
}

void Handler::handleEvent()
{
    if(!if_close_)
    {
        if(events_ & EPOLLIN)
        {
            readCallback_(fd_);
        }

        if(events_ & EPOLLOUT)
        {
            writeCallback_(fd_);
        }
    } else {
        closeCallback_(fd_);
    }
}

#endif