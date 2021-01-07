#include "MyFtpServerTask.h"
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <unordered_map>
#include <string.h>

#pragma once
using namespace std;
class MyFtpServerCommand: public MyFtpServerTask {
public:
    virtual bool init();//运行在新线程中
    virtual void read(struct bufferevent *bev);
    virtual void event(struct bufferevent *bev, short what);
    ~MyFtpServerCommand();
    void addToCalls(string s, MyFtpServerTask *call);//调用完成后才分发到线程 故不需要考虑线程安全
private:
    unordered_map<string,MyFtpServerTask*> calls;
};

MyFtpServerCommand::~MyFtpServerCommand(){
}

void MyFtpServerCommand::addToCalls(string s, MyFtpServerTask *call){
    if(!call){
        cout<<"MyFtpServerTask::call is null"<<endl;
        return;
    }
    if(s.empty()) {
        cout<<"CMD is null"<<endl;
        return;
    }
    if(calls.find(s) != calls.end()) {
        cout<<"CMD has existed"<<endl;
        return;
    }
    calls[s] = call;
}

void MyFtpServerCommand::read(struct bufferevent *bev){
    char data[1024] = {0};
    while(true) {
        int len = bufferevent_read(bev, data, sizeof(data) - 1);
        if (len <= 0) {
            break;
        }
        data[len] = '\0';
        cout<<"recv cmd: "<<data<<flush;
        //分发到处理对象
        //分析类型
        string type = "";
        for (int i = 0; i < len; ++i) {
            if(data[i] == '\r' || data[i] == ' ') break;
            type += data[i];
        }
        cout<<"type is ["<<type<<"]"<<endl;
        if(calls.find(type) != calls.end()) {
            MyFtpServerTask *t = calls[type];
            t -> cmdTask = this;//用来处理回复命令和目录
            t -> ip = ip;
            t -> port = port;
            t -> base = base;
            t -> parser(type,data);
            if (type == "PORT") {
                ip = t -> ip;
                port = t -> port;
            }
        }
        else{
            string msg = "200 OK\r\n";
            bufferevent_write(bev,msg.c_str(),msg.size());
        }
    }
}

void MyFtpServerCommand::event(struct bufferevent *bev, short what){
    //如果连接断开 可能收不到BEV_EVENT_EOF 所以需要超时机制
    if (what & (BEV_EVENT_EOF|BEV_EVENT_ERROR|BEV_EVENT_TIMEOUT)) {
        cout<<"异常断开或者超时"<<endl;
        bufferevent_free(bev);
        delete this;
    }
}

bool MyFtpServerCommand::init(){
    bufferevent *bev = bufferevent_socket_new(base,sock,BEV_OPT_CLOSE_ON_FREE);
    this -> bev = bev;
    this -> setCallback(bev);
    //添加超时事件
    timeval timeLimit = {100,0};
    bufferevent_set_timeouts(bev,&timeLimit,0);

    string welcomeMessage = "220 Welcome to MyFtpServer\r\n";
    bufferevent_write(bev, welcomeMessage.c_str(), welcomeMessage.size());

    return true;
}



