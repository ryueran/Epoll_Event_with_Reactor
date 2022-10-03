#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_

#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <functional>
#include "Reactor.h"
#include "Event_Handler.h"

class Acceptor
{
public:
    Acceptor(Reactor* reactor, int fd);
    ~Acceptor();
    void Accept(int listen_fd);
    void listen_func(int fd, int listen_enq);
    void setnewConnectionCallback(const std::function<void(int)>& conn_cb);
private:
    int fd_;
    Reactor* reactor_;

    socklen_t client_;
    struct sockaddr_in client_addr_;
    std::function<void(int)> newConnectionCallback_; // read(int fd)
    Event_Handler *acceptor_Handler_;
};

Acceptor::Acceptor(Reactor* reactor, int fd)
{
    std::cout << "Acceptor constrcuted!" << std::endl;
    reactor_ = reactor;
    fd_ = fd;

}

Acceptor::~Acceptor()
{
    std::cout << "Acceptor destructed!" << std::endl;
    acceptor_Handler_->remove();
}

void Acceptor::setnewConnectionCallback(const std::function<void(int)>& conn_cb)
{
    newConnectionCallback_ = conn_cb;
}

void Acceptor::Accept(int listen_fd)
{
    std::cout << "Acceptor::Accept" << std::endl;
    socklen_t client_addr_len = sizeof(client_addr_);
    int connfd_ = accept(listen_fd, (sockaddr*)&client_addr_, &client_addr_len);
    if(connfd_ > 0)
    {
        newConnectionCallback_(connfd_);
    } else {
        std::cout << "Accept failed!" << std::endl;
    }
}

void Acceptor::listen_func(int fd, int listen_enq)
{
    std::cout << "Acceptor::listen_func" << std::endl;
    int listen_v = listen(fd, listen_enq);

    acceptor_Handler_ = new Event_Handler(fd, reactor_);
    acceptor_Handler_->set_read_actor(std::bind(&Acceptor::Accept, this, std::placeholders::_1));
    acceptor_Handler_->enableReading();
}

#endif