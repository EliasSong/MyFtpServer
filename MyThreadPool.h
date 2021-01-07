#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <thread>
#include "MyThread.h"
using namespace std;

class MyThreadPool{
public:
    static std::shared_ptr<MyThreadPool> getInstance();
    void init(int threadCount);//线程池初始化，并启动线程
    void dispath(MyTask *task);//任务挂载
    ~MyThreadPool(){ cout<<"Destory ThreadPool"<<endl; };
private:
    MyThreadPool(){ cout<<"Create ThreadPool"<<endl; };
    MyThreadPool(const MyThreadPool&){};
    MyThreadPool& operator=(const MyThreadPool&){};
    int lastThread = -1;//用于线程轮询
    int threadCount = 0;//线程数量
    std::vector<MyThread*> threads;//线程组
    static std::shared_ptr<MyThreadPool> instance;//线程池单例
};

shared_ptr<MyThreadPool> MyThreadPool::instance = nullptr;

shared_ptr<MyThreadPool> MyThreadPool::getInstance() {
    if(instance == nullptr) {
        instance = shared_ptr<MyThreadPool>(new MyThreadPool());
    }
    return instance;
}

void MyThreadPool::init(int threadCount) {
    this -> threadCount = threadCount;
    this -> lastThread = -1;
    for (int i = 0; i < threadCount; i++) {
        MyThread *temp = new MyThread();
        temp -> id = i + 1;
        threads.push_back(temp);
        cout<<"create thread:"<<i<<endl;
        temp -> startMyThread();
        this_thread::sleep_for(10ms);
    }
}

void MyThreadPool::dispath(MyTask *task) {
    if (!task) return;
    //轮询分发线程
    int tid = (lastThread + 1) % threadCount;
    lastThread = tid;
    threads[tid] -> addTask(task);
    threads[tid] -> activate();
    //激活线程
}