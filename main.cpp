#include <cstdio>
#include "./lock/locker.h"
#include "./threadpool/threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "./http/http_conn.h"
#include <exception>
#include <errno.h>
#include <sys/epoll.h>

#define MAX_FD 65535 //最多的文件描述符个数
#define MAX_EVENT_NUMBER 10000  //最大监听的事件的个数

//添加epoll监听事件
extern void addfd(int epollfd, int fd, bool oneshot);

//删除epoll监听事件
extern void removefd(int epollfd, int fd);

//修改文件描述符
extern void modfd(int epollfd,int fd);

int main(int argc, char* argv[]) {
	if (argc <= 1) {
		printf("请输入 %s port", basename(argv[0]));
        exit(-1);
	}
	int port = atoi(argv[1]);

	//创建线程池    任务为http_conn类,处理用户连接
	threadpool<http_conn>* pool = new threadpool<http_conn>;

	//创建一个数组用于保存用户的信息，处理业务
	http_conn* users = new http_conn[MAX_FD];

	//创建监听socket
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);

	//设置端口复用
	int reuse = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	//绑定ip地址和端口号
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;
	int ret=bind(listenfd, (struct sockaddr*)&saddr, sizeof(saddr));
	if (ret == -1) {
		std::exception();
	}

	//设置监听
	listen(listenfd, 5);

	//创建epoll对象，事件数组
	int epollfd = epoll_create(5);
	epoll_event events[MAX_EVENT_NUMBER];

	//将监听文件描述符加入到epoll对象中
	addfd(epollfd, listenfd, false);
	http_conn::m_epollfd = epollfd;

	//循环检查
	while (true) {
		int num = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
		if (num <= 0 && errno != EINTR) {//是系统中断错误
			printf("epoll_wait failed..\n");
			break;
		}
		//循环遍历就绪事件
		for (int i = 0; i < num; i++) {
			int sockfd = events[i].data.fd;
			if (sockfd == listenfd) {
				struct sockaddr_in addr;
				socklen_t addrlen = sizeof(addr);
				int connfd=accept(sockfd,(struct sockaddr *)&addr,&addrlen);
				if (http_conn::m_user_count >= MAX_FD) {
					//链接已满
					close(connfd);
					continue;
				}
				//将HTTP类中的信息更新。
				users[connfd].init(connfd,addr);
			}
			else if (events[i].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
				//对方异常或者错误断开
				users[sockfd].close_conn();
			}
			else if (events[i].events & EPOLLIN) {
				//检测到读事件就绪
				//主线程去读数据，让子线程去处理逻辑
				if(users[sockfd].read()){
                    pool->append(users+sockfd);
                }else{
                    users[sockfd].close_conn();
                }
			}
			else if (events[i].events & EPOLLOUT) {
				//写事件就绪
				if(!users[sockfd].write()){
                   users[sockfd].close_conn(); 
                }
			}
		}
	}
	close(listenfd);
	close(epollfd);
	delete[] users;
    delete pool;
	return 0;
}