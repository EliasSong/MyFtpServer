#include <memory>
#include <iostream>
#include "MyFtpServerCommand.h"
#include "./Command/CommandUSER.h"
#include "./Command/CommandLIST.h"
#include "./Command/CommandPORT.h"
#include "./Command/CommandRETR.h"
#include "./Command/CommandSTOR.h"
#pragma once
using namespace std;
class MyFtpServerFactory{
public:
    static shared_ptr<MyFtpServerFactory> getInstance();
    static shared_ptr<MyFtpServerFactory> instance;
    MyTask *createTask();
    ~MyFtpServerFactory();

private:
    MyFtpServerFactory(){};
    MyFtpServerFactory(const MyFtpServerFactory&){};
    MyFtpServerFactory& operator=(const MyFtpServerFactory&){};
};

MyFtpServerFactory::~MyFtpServerFactory(){
    cout<<"destory MyFtpServerFactory"<<endl;
}
shared_ptr<MyFtpServerFactory> MyFtpServerFactory::instance = nullptr;

shared_ptr<MyFtpServerFactory> MyFtpServerFactory::getInstance(){
    if(instance == nullptr) {
        instance = shared_ptr<MyFtpServerFactory>(new MyFtpServerFactory());
    }
    return instance;
}

MyTask* MyFtpServerFactory::createTask(){
    MyFtpServerCommand *cmd = new MyFtpServerCommand();
    CommandUSER *user = new CommandUSER();
    cmd -> addToCalls("USER", user);
    CommandLIST *list = new CommandLIST();
    cmd -> addToCalls("PWD", list);
    cmd -> addToCalls("LIST", list);
    cmd -> addToCalls("CWD", list);
    cmd -> addToCalls("CDUP", list);
    CommandPORT *port = new CommandPORT();
    cmd -> addToCalls("PORT", port);
    CommandRETR *retr = new CommandRETR();
    cmd -> addToCalls("RETR", retr);
    CommandSTOR *stor = new CommandSTOR();
    cmd -> addToCalls("STOR",stor);
    return cmd;
}