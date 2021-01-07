#include "../MyFtpServerTask.h"
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;

class CommandPORT: public MyFtpServerTask {
public:
    void parser(string type, string msg);
};

void CommandPORT::parser(string type, string msg) {
    //计算ip地址和端口号
    vector<string> vals;
    string temp = "";
    for (int i = 5; i < msg.size(); ++i) {
        if(msg[i] == ',' || msg[i] == '\r'){
            vals.push_back(temp);
            temp = "";
            continue;
        }
        temp += msg[i];
    }
    if(vals.size() != 6) {
        response("501 Syntax error in parameters\r\n");
    }
    ip = vals[0]+"."+vals[1]+"."+vals[2]+"."+vals[3];
    port = stoi(vals[4]) * 256 + stoi(vals[5]);
    cout<<"ip: "<<ip<<endl;
    cout<<"port: "<<port<<endl;
    response("200 PORT command success\r\n");
}