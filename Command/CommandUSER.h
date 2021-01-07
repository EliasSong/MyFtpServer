#include "../MyFtpServerTask.h"
#include <string.h>
#include <iostream>
using namespace std;
class CommandUSER : public MyFtpServerTask{
public:
    virtual void parser(string type, string msg);
};

void CommandUSER::parser(string type, string msg){
    if(type =="USER"){
        cout<<"Parser "<<type<<" "<<msg<<endl;
        response("230 Login successful.\r\n");
    }
}
