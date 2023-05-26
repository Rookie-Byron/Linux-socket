#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char**  argv)
{
	if(argc < 3)
	{
		fprintf(stderr, "用法：%s <IP> <端口号>\n", argv[0]);
		return -1;
	}
	printf("客户机：创建网络套接字\n");
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("socket");
		return -1;
	}
	printf("客户机：准备地址并连接\n");
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		perror("connect");
		return -1;
	}
	char buf[1024] = {0};
	for(;;)
	{
		printf(">");
		fgets(buf, sizeof(buf) / sizeof(buf[0]), stdin);
		if(!strcmp(buf, "!\n"))
		{
			break;
		}
		printf("客户机：发送请求\n");
		if(send(sockfd, buf, strlen(buf) * sizeof(buf[0]), 0) == -1)
		{
			perror("send");
			return -1;
		}
		printf("客户机：接收响应\n");
		ssize_t rb = recv(sockfd, buf, sizeof(buf) - sizeof(buf[0]), 0);
		if(rb == 0)
		{
			printf("客户机：服务器已关闭\n");
			break;
		}
		buf[rb / sizeof(buf[0])] = '\0';
		printf("<%s", buf);
	}
	printf("客户机：关闭套接字\n");
	if(close(sockfd) == -1)
	{
		perror("close");
		return -1;
	}
	printf("完成\n");
	return 0;
}
