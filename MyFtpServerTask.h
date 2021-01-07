#pragma once
#include "MyTask.h"
#include <iostream>
#include <string.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
using namespace std;
class MyFtpServerTask : public MyTask{
public:
    string currentDir = "/";
    string rootDir = "./";
    string ip = "";
    int port = 0;
    MyFtpServerTask *cmdTask = 0;//命令通道task
    virtual void read(struct bufferevent *bev){};
    virtual void write(struct bufferevent *bev){};
    virtual void event(struct bufferevent *bev, short what){};
    virtual void parser(string type, string msg){};
    void setCallback(struct bufferevent *bev);
    bool init(){return true;}
    void response(string msg);//回应cmd请求
    void connect();//建立数据通道
    void send(const char *data,int size);//支持二进制发送
    void close();//关闭数据通道链接
protected:
    static void ReadCallBack(struct bufferevent *bev, void *arg);
    static void WriteCallBack(struct bufferevent *bev, void *arg);
    static void EventCallBack(struct bufferevent *bev, short what, void *arg);
    struct bufferevent *bev = 0;//命令bev

};
void MyFtpServerTask::ReadCallBack(bufferevent *bev, void *arg){
    MyFtpServerTask *t = (MyFtpServerTask*)arg;
    t -> read(bev);
}

void MyFtpServerTask::WriteCallBack(bufferevent *bev, void *arg){
    MyFtpServerTask *t = (MyFtpServerTask*)arg;
    t -> write(bev);
}

void MyFtpServerTask::EventCallBack(struct bufferevent *bev, short what, void *arg){
    MyFtpServerTask *t = (MyFtpServerTask*)arg;
    t -> event(bev,what);
}
void MyFtpServerTask::setCallback(struct bufferevent *bev){
    bufferevent_setcb(bev,ReadCallBack,WriteCallBack,EventCallBack,this);
    bufferevent_enable(bev,EV_READ|EV_WRITE);
}

void MyFtpServerTask::response(string msg) {
    if(!cmdTask || !cmdTask -> bev) return;
    bufferevent_write(cmdTask -> bev, msg.c_str(), msg.size());
} 

void MyFtpServerTask::connect(){
    if(ip.empty() || port <= 0 || !base) {
        cout<< "connect failed ip or port is null"<<endl;
        return;
    }
    close();
    bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    sockaddr_in sin;
    memset(&sin,0,sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    evutil_inet_pton(AF_INET,ip.c_str(),&sin.sin_addr.s_addr);
    //设置回调 权限
    setCallback(bev);
    timeval rt = {120, 0};
    bufferevent_set_timeouts(bev, &rt, 0);
    bufferevent_socket_connect(bev, (sockaddr*)&sin,sizeof(sin));
}


void MyFtpServerTask::send(const char *data,int size) {
    if(!bev) return;
    bufferevent_write(bev,data,size);
}

void MyFtpServerTask::close(){
    if(bev){
        bufferevent_free(bev);
        bev = 0;
    }
}

