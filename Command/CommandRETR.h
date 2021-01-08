#include "../MyFtpServerTask.h"
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
using namespace std;

class CommandRETR : public MyFtpServerTask{
public:
    virtual void parser(string type, string msg);
    virtual void write(struct bufferevent *bev);
    virtual void event(struct bufferevent *bev, short what);
private:
    
    char buffer[1024] = {0};
};

void CommandRETR::parser(string type, string msg) {
    int blankPos = msg.rfind(" ") + 1;
    string filename = msg.substr(blankPos, msg.size() - blankPos - 2);
    string path = cmdTask -> rootDir + cmdTask -> currentDir + "/" + filename;
    fp = fopen(path.c_str(),"rb");
    cout<<"fd: "<<fp<<endl;
    if (fp) {
        connect();
        response("150 File confirmed!\r\n");
        //触发写入事件
        bufferevent_trigger(bev,EV_WRITE,0);
    }
    else {
        response("450 File open failed!\r\n");
    }
}

void CommandRETR::write(struct bufferevent *bev){
    if(!fp) {
        cout<<"none"<<endl;
    }
    int len = fread(buffer, 1, sizeof(buffer), fp);
    if(len <= 0) {
        response("226 file transfer complete.\r\n");
        close();
        return ;
    }
    cout<<"["<<len<<"]"<<flush;
    send(buffer, len);
}

void CommandRETR::event(struct bufferevent *bev, short what){
    if (what & (BEV_EVENT_EOF|BEV_EVENT_ERROR|BEV_EVENT_TIMEOUT)) {
        cout<<"connect time out"<<endl;
        close();
    }
    else if(what & BEV_EVENT_CONNECTED) {
        cout<<"file connect successful"<<endl;
    }
}