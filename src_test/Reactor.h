#ifndef REACTOR_H_
#define REACTOR_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <memory>

class Event_Handler;
class Epoller;

class Reactor {
public:
    Reactor();
    ~Reactor();
    void handle_events();
    void register_handler(Event_Handler *h);
    void remove_handler(Event_Handler *h);

private:
    Epoller* poller_;
};

#endif