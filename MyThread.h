#pragma once
#include <thread>
#include <iostream>
#include <unistd.h>
#include <event2/event.h>
#include <list>
#include <event2/util.h>
#include <mutex>
#include "MyTask.h"
using namespace std;

class MyThread{
public:
    void startMyThread();//启动线程
    void Main();//线程入口函数
    bool setup();//安装线程，初始化event_base和管道监听事件用于激活线程
    void activate();//线程激活
    void notify(evutil_socket_t fd,short which);
    void addTask(MyTask *task);//添加处理的任务 一个线程可以同时处理多个任务 共用一个event_base
    MyThread(){};
    ~MyThread(){};
    int id;//线程编号
private:
    int notify_send_fd = 0;
    struct event_base *base = 0;//线程中event_base上下文
    list<MyTask*> myTasksList;//任务列表
    mutex myTasks_mutex;//互斥锁
};

static void notifyCallback(evutil_socket_t fd,short which, void *arg){
    MyThread *th = (MyThread*)arg;
    th -> notify(fd,which);
}

void MyThread::startMyThread(){
    setup();
    thread th(&MyThread::Main,this);
    th.detach();
}

void MyThread::Main(){
    cout<<"Thread: "<<id<<" dispath"<<endl;
    event_base_dispatch(base);
    event_base_free(base);
    cout<<"Thread: "<<id<<" dispath free"<<endl;
}

void MyThread::notify(evutil_socket_t fd,short which){
    //水平触发
    char buf[2] = {0};
    int re = read(fd,buf,1);
    if (re <= 0) return;
    //获取任务并初始化任务
    MyTask *currentTask = nullptr;
    myTasks_mutex.lock();
    if (myTasksList.empty()) {
        cout<<"List empty!!!!!"<<endl;
        myTasks_mutex.unlock();
        return;
    }
    currentTask = myTasksList.front();
    myTasksList.pop_front();//先进先出
    myTasks_mutex.unlock();
    currentTask -> init();
}

bool MyThread::setup(){
    //创建管道
    int fds[2];
    if (pipe(fds)) {
        cerr<<"Pipe failed"<<endl;
        return false;
    }
    notify_send_fd = fds[1];
    //创建libevent上下文，配置为非阻塞
    event_config *ev_conf = event_config_new();
    event_config_set_flag(ev_conf,EVENT_BASE_FLAG_NOLOCK);
    this -> base = event_base_new_with_config(ev_conf);
    event_config_free(ev_conf);
    if (!base) {
        cerr<<"Base create failed"<<endl;
        return false;
    }
    //创建回调事件
    event *ev = event_new(base,fds[0],EV_READ|EV_PERSIST,notifyCallback,this);
    event_add(ev,0);
    return true;
}

void MyThread::activate(){
    int re = write(this -> notify_send_fd, "c", 1);
    if (re <= 0) {
        cerr<<"Activate failed"<<endl;
    }
    else{
        cout<<"Thread: "<<id<<" activate successfully"<<endl;
    }
}

void MyThread::addTask(MyTask *t){
    if (!t) return;
    t -> base = this -> base;
    myTasks_mutex.lock();
    myTasksList.push_back(t);
    myTasks_mutex.unlock();
}

