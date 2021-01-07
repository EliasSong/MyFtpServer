#include <event2/event.h>
#include <event2/listener.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h> 
#include <thread>
#include <memory.h>
#include <errno.h>
#include <event2/bufferevent.h>
#include "MyThreadPool.h"
#include "MyFtpServerFactory.h"
using namespace std;

void listen_cb(struct evconnlistener *e, evutil_socket_t s, struct sockaddr *a, int socklen, void *arg){
	MyTask *t = MyFtpServerFactory::getInstance() -> createTask();
	t -> sock = s;
	MyThreadPool::getInstance() -> dispath(t);
}

int main() {
	//初始化线程池
	auto p = MyThreadPool::getInstance();
	p -> getInstance() -> init(8);
	//创建base上下文
	event_base *base = event_base_new();
	//创建网络服务器
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_port = htons(21);
	sin.sin_family = AF_INET;
	evconnlistener *ev = evconnlistener_new_bind(base,
						listen_cb,
						base,
						LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,
						10,
						(sockaddr*)&sin,
						sizeof(sin)
						);			
	event_base_dispatch(base);
	evconnlistener_free(ev);
	event_base_free(base);
	return 0;
}
 
 












