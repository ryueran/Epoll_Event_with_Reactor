#include <iostream>
#include "Reactor.h"
#include "Event_Handler.h"
#include "Epoller.h"

Reactor::Reactor()
{
    std::cout << "Reactor constructed!" << std::endl;
    poller_ = new Epoller();
}

Reactor::~Reactor()
{
    std::cout << "Reactor destructed!" << std::endl;
}

void Reactor::register_handler(Event_Handler *h)
{
    poller_->add_Handler(h);
}

void Reactor::remove_handler(Event_Handler *h)
{
    poller_->remove_Handler(h);
}

void Reactor::handle_events()
{
    // here we have to extract the handlers that has already been assigned
    while(1)
    {
        std::vector<Event_Handler*> active_handlers;
        poller_->Epoll_Wait(&active_handlers);
    
        if(!active_handlers.empty())
        {
            for(int i = 0; i < active_handlers.size(); i++)
            {
                (active_handlers)[i]->handle_event();
            }
        }
    }
}