#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		fprintf(stderr, "用法：%s <主机IP> <域名> [<资源路径>]\n", argv[0]);
		return -1;
	}
	const char* ip = argv[1];
	const char* domain = argv[2];
	const char* path = argc < 4 ? "/" : argv[3];
	//创建套接字
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("socket");
		return -1;
	}
	//服务器地址
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	if(!inet_aton(ip, &addr.sin_addr))
	{
		perror("inet_aton");
		return -1;
	}
	//连接服务器
	if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		perror("connect");
		return -1;
	}
	//格式化请求
	char request[1024] = {0};
	sprintf(request, "GET %s HTTP/1.0\r\n"
			 "Host:%s\r\n"
			 "Accept:text/html\r\n"
			 "Connection:Close\r\n"
			 "User-Agent:Mozilla/5.0\r\n"
			 "Referer:%s\r\n"
			 "\r\n\r\n", path, domain, domain);
	printf("%s", request);
	//发送请求
	if(send(sockfd, request, strlen(request) * sizeof(request[0]), 0) == -1)
	{
		perror("send");
		return -1;
	}
	//接收响应
	char respond[1024] = {};
	for(;;)
	{
		ssize_t rb = recv(sockfd, respond, sizeof(respond) - sizeof(respond[0]), 0);
		if(rb < 0)
		{
			perror("recv");
			return -1;
		}
		if(!rb)
		{
			break;
		}
		//printf("%s", respond);
		int fd = open("./index.html", O_RDWR);
		if(write(fd, respond, strlen(respond) * sizeof(respond)) < 0)
		{
			perror("write");
			close(fd);
			return -1;
		}
		close(fd);
	}
	printf("\n");
	close(sockfd);
	return 0;
}
