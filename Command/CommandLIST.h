#include "../MyFtpServerTask.h"
#include <string.h>
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
using namespace std;
class CommandLIST : public MyFtpServerTask{
public:
    virtual void parser(string type, string msg);
    virtual void write(struct bufferevent *bev);
    virtual void event(struct bufferevent *bev, short what);
private:
    string getListData(string path);
};
void CommandLIST::write(struct bufferevent *bev){
    response("226 transfer successfully\r\n");
    close();
}
void CommandLIST::event(struct bufferevent *bev, short what){
    if (what & (BEV_EVENT_EOF|BEV_EVENT_ERROR|BEV_EVENT_TIMEOUT)) {
        cout<<"connect time out"<<endl;
        close();
    }
    else if(what & BEV_EVENT_CONNECTED) {
        cout<<"connect successful"<<endl;
    }
}
void CommandLIST::parser(string type, string msg) {
    string responseMsg = "";
    if (type == "PWD") {
        responseMsg = "257 \"";
        responseMsg += cmdTask -> currentDir;
        responseMsg += "\" is current dir\r\n";
        response(responseMsg);
    }
    else if (type == "LIST") {
        //命令通道回复消息 使用数据通道发送目录
        connect();
        response("150 Here comes the directory list.\r\n");
        string listData = getListData(cmdTask -> rootDir + cmdTask -> currentDir);
        send(listData.c_str(),listData.size());//数据通道发送
    }
    else if (type == "CWD") {
        //取命令中的路径
        int pos  = msg.rfind(" ") + 1;
        string path = msg.substr(pos, msg.size() - pos - 2);
        if(path[0] == '/'){
            cmdTask -> currentDir = path;
        }//传入绝对路径
        else{
            if(cmdTask -> currentDir.back() != '/') {
                cmdTask -> currentDir += "/";
            }
            cmdTask -> currentDir += path + "/";
        }//传入相对路径

        response("250 Directory changed successfully.\r\n");
    }
    else if (type == "CDUP") {
        string path = cmdTask -> currentDir;
        if(path.size() > 1) {
            if(path.back() == '/') {
                path.pop_back();
            }
            int pos = path.rfind("/");
            path = path.substr(0,pos);
        }
        cmdTask -> currentDir = path;
        response("250 Directory changed successfully.\r\n");
    }
}

string CommandLIST::getListData(string path) {
        string bashCmd = "ls -l ";
        string listData = "";
        bashCmd += path;
        cout<<bashCmd<<endl;
        FILE *fd = popen(bashCmd.c_str(), "r");
        if(!fd) return listData;
        char buffer[1024] = {0};
        while(true) {
            int len = fread(buffer, 1, sizeof(buffer) - 1, fd);
            if (len <= 0) break;
            buffer[len] = '\0';
            listData += buffer;
        }
        return listData;

}
