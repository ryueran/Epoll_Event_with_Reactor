#ifndef SELECT_H_
#define SELECT_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include "Event_Handler.h"

class Select{
public:
    Select();
    ~Select();
    std::vector<Event_Handler*> select_wait(const std::vector<Event_Handler*>& handlers, int begin, int end);
private:
    std::vector<Event_Handler*> act_handlers = {};
    bool wait_;
};

Select::Select()
{
    wait_ = false;
}

Select::~Select()
{

}

std::vector<Event_Handler*> Select::select_wait(const std::vector<Event_Handler*>& handlers, int begin, int end)
{
    while(!wait_)
    {
        if(handlers.size() < end)
        {

        } else {
            std::vector<bool> ready_for_process = {};

            if(ready_for_process.size() == handlers.size())
            {
                ready_for_process.clear();
            } else {
                for(int i = 0; i < handlers.size(); i++)
                {     
                    if( i >= begin && i < end)
                    {
                        // std::cout << "Add event: " << i << std::endl;
                        //act_handlers.push_back(handlers[i]);
                        ready_for_process.push_back(handlers[i]->if_handler_available());
                    }
                }
            }

            bool mark_ready_for_process = false;
            auto it = std::find(ready_for_process.begin(), ready_for_process.end(), false);

            if(it == ready_for_process.end())
            {
                wait_ = true;
            }
        }
    }

    std::vector<Event_Handler*> act_handlers = {};
    for(int i = 0; i < handlers.size(); i++)
    {
        if( i >= begin && i < end)
        {
            act_handlers.push_back(handlers[i]);
        }
    }

    return act_handlers;
}

// This function/model handles the task collection work, should be able to block the process/thread
std::vector<Event_Handler*> select(const std::vector<Event_Handler*>& handlers, int begin, int end)
{
   std::vector<Event_Handler*> act_handlers = {};
   for(int i = 0; i < handlers.size(); i++)
   {
       if( i >= begin && i < end)
       {
           std::cout << "Add event: " << i << std::endl;
           act_handlers.push_back(handlers[i]);
       }
   }

   return act_handlers;
}
#endif
