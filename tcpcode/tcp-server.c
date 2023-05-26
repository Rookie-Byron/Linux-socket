#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <wait.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
void sigchld(int signum)
{
	//信号处理函数，用于回收子进程
	for(;;)
	{
		pid_t pid = waitpid(-1, NULL, WNOHANG);
		if(pid == -1)
		{
			if(errno != ECHILD)
			{
				perror("waitpid");
				exit(-1);
			}
			printf("服务器：全部子进程都已退出！\n");
			break;
		}
		if(pid)
		{
			printf("服务器：发现%d子进程退出！\n", pid);
		}
		else
		{
			printf("服务器：暂时没有子进程退出\n");
			break;
		}
	}
}

int client(int connfd)
{
	//处理客户机通信
	for(;;)
	{
		printf("%d：接收请求\n", getpid());
		char buf[1024] = {0};
		ssize_t rb = recv(connfd, buf, sizeof(buf), 0);
		if(rb == -1)
		{
			perror("recv");
			return -1;
		}
		if(rb == 0)
		{
			printf("%d：客户机已关闭\n", getpid());
			break;
		}
		printf("%d：发送响应\n", getpid());
		//接收啥内容发回啥内容
		if(send(connfd, buf, rb, 0) == -1)
		{
			perror("send");
			return -1;
		}
	}
	return 0;
}

int main (int argc, char** argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "用法：%s <端口号>\n", argv[0]);
		return -1;
	}
	if(signal(SIGCHLD, sigchld) == SIG_ERR)
	{
		perror("signal");
		return -1;
	}
	printf("服务器：创建网络套接字\n");
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("socket");
		return -1;
	}
	printf("服务器：准备地址并绑定\n");
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[1]));
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
		return -1;
	}
	printf("服务器：侦听套接字\n");
	if(listen(sockfd, 1024) == -1)
	{
		perror("listen");
		return -1;
	}
	for(;;)
	{
		struct sockaddr_in addrcli = {};
		socklen_t addrlen = sizeof(addrcli);
		int connfd = accept(sockfd, (struct sockaddr*)&addrcli, &addrlen);
		if(connfd == -1)
		{
			perror("accept");
			return -1;
		}
		printf("服务器：客户机%s:%u\n", inet_ntoa(addrcli.sin_addr), ntohs(addrcli.sin_port));
		//创建子进程
		pid_t pid = fork();
		if(pid == -1)
		{
			perror("fork");
			return -1;
		}
		if(pid == 0)
		{
			//子进程
			//关闭侦听套接字
			if(close(sockfd) == -1)
			{
				perror("close");
				return -1;
			}
			//处理客户机请求
			if(client(connfd) == -1)
			{
				return -1;
			}
			if(close(connfd) == -1)
			{
				perror("close");
				return -1;
			}
			printf("完成\n");
			return 0;
		}
		//父进程
		if(close(connfd) == -1)
		{
			perror("close");
			return -1;
		}
		return 0;
	}
	return 0;
}
