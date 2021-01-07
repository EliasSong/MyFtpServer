#include "../MyFtpServerTask.h"
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
using namespace std;

class CommandSTOR : public MyFtpServerTask {
public:
    virtual void parser(string type, string msg);
    virtual void read(struct bufferevent *bev);
    virtual void event(struct bufferevent *bev, short what);
private:
    FILE *fp = 0;
    char buffer[1024] = {0};
};

void CommandSTOR::parser(string type, string msg){
    int blankPos = msg.rfind(" ") + 1;
    string filename = msg.substr(blankPos, msg.size() - blankPos - 2);
    string path = cmdTask -> rootDir + cmdTask -> currentDir + filename;
    fp = fopen(path.c_str(),"wb");
    if (fp) {
        connect();
        response("125 Start receiving file!\r\n");
        //触发读取事件
        bufferevent_trigger(bev,EV_READ,0);
    }
    else {
        response("450 file open failed!\r\n");
    }
}

void CommandSTOR::read(struct bufferevent *bev){
    if(!fp) return;
    while(true) {
        int len = bufferevent_read(bev, buffer, sizeof(buffer));
        if (len <= 0) return;
        int size = fwrite(buffer, 1, len, fp);
        cout<<"<"<<len<<"-"<<size<<">"<<endl;
    }
}

void CommandSTOR::event(struct bufferevent *bev, short what){
    if (what & (BEV_EVENT_EOF|BEV_EVENT_ERROR|BEV_EVENT_TIMEOUT)) {
        cout<<"BEV_EVENT_EOF|BEV_EVENT_ERROR|BEV_EVENT_TIMEOUT"<<endl;
        close();
        if(fp){
            fclose(fp);
            fp = 0;
        } 
        response("226 transfer successfully\r\n");
    }
    else if(what & BEV_EVENT_CONNECTED) {
        cout<<"file connect successful"<<endl;
    }
}

