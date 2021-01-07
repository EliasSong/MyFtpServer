#pragma once
class MyTask{
public:
    struct event_base *base = 0;
    int sock = 0;
    int threadId = 0;
    virtual bool init() = 0 ;
};