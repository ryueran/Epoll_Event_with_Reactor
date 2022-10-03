#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Epoller.h"
#include "EventLoop.h"
#include "Handler.h"

class Server {
public:
    Server(int ip_addr, int port);
    ~Server();
    void accept();
    void start();

private:
    int ip_addr_;
    int port_;
    int socket_; 
    EventLoop *loop_;
};

Server::Server(int ip_addr, int port)
{
    ip_addr_ = ip_addr;
    port_ = port;
    socket_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    loop_ = new EventLoop();
}

Server::~Server()
{
    close(socket_);
}



#endif