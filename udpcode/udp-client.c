#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		fprintf(stderr, "用法：%s <IP> <端口号>\n", argv[0]);
		return -1;
	}
	printf("客户端：创建网络套接字\n");
	int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		perror("sockfd");
		return -1;
	}
	printf("客户端：准备地址\n");
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[2]));
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	char buf[1024] = {0};
	for(;;)
	{
		printf(">");
		fgets(buf, sizeof(buf) / sizeof(buf[0]), stdin);
		if(!strcmp(buf, "!\n"))
		{
			break;
		}
		printf("客户端：向%s:%u发送请求\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
		if(sendto(sockfd, buf, strlen(buf) * sizeof(buf[0]), 0, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		{
			perror("sendto");
			return -1;
		}
		printf("客户端：接收响应\n");
		ssize_t rb = recv(sockfd, buf, sizeof(buf) - sizeof(buf[0]), 0);
		if(rb < 0)
		{
			perror("recv");
			return -1;
		}
		buf[rb / sizeof(buf[0])] = '\n';
		printf("<%s", buf);
	}
	printf("客户端：关闭套接字\n");
	if(close(sockfd) == -1)
	{
		perror("close");
		return -1;
	}
	printf("客户端：完成\n");
	return 0;
}
