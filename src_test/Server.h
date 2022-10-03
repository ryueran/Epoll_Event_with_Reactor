#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>
#include <functional>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Reactor.h"
#include "Acceptor.h"
#include "Event_Handler.h"

#define MAX_BUFF_LEN 1024

class Server
{
public:
    Server(Reactor* reactor, int fd);
    ~Server();
    void newConnection(int fd);
    void readData(int fd);
    void writeData(int fd);
    void start();
private:
    Reactor* reactor_;
    Acceptor* acceptor_;
    Event_Handler* handler_;
    int fd_;
};

Server::Server(Reactor* reactor, int fd)
{
    reactor_ = reactor;
    fd_ = fd;
    acceptor_ = new Acceptor(reactor_, fd_);
    acceptor_->setnewConnectionCallback(std::bind(&Server::newConnection, this, std::placeholders::_1));
}

void Server::newConnection(int fd)
{
    std::cout << "New connection request comes!" << std::endl;
    handler_ = new Event_Handler(fd, reactor_);
    handler_ -> set_read_actor(std::bind(&Server::readData, this, std::placeholders::_1));
    handler_ -> enableReading();
}

void Server::readData(int fd)
{
    std::cout << "Read Phase" << std::endl;
    int nbyte = read(fd, handler_->buff(), MAX_BUFF_LEN);

    if(nbyte > 0)
    {
        std::cout << "Start reading" << std::endl;
        printf("receice: %s\n", handler_->buff());
        
        handler_->set_write_actor(std::bind(&Server::writeData, this, std::placeholders::_1));
        handler_->enableWriting();
    }
}

void Server::writeData(int fd)
{
    std::cout << "Write Phase" << std::endl;
    std::cout << "Start writing" << std::endl;
    int nbyte = write(fd, handler_->buff(), MAX_BUFF_LEN);
    if(nbyte > 0)
    {
        std::cout << "Back to read" << std::endl;
        //handler_ -> set_read_actor(std::bind(&Server::readData, this, std::placeholders::_1));
        handler_->enableReading();
    }
}

void Server::start()
{
    int reuse = 1;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    sockaddr_in addr;
    bzero(&addr, sizeof(addr));


    char* ip = "0.0.0.0";
    int port = atoi("12333");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    int b = bind(fd_, (sockaddr*)&addr, sizeof(addr));
    acceptor_->listen_func(fd_, 16);
}

#endif