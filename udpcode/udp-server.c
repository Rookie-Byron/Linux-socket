#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "用法：%s <端口号>\n", argv[0]);
		return -1;
	}
	printf("服务器：创建网络套接字\n");
	int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
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
	char buf[1024] = {0};
	for(;;)
	{
		printf("服务器：接收请求\n");
		struct sockaddr_in addrcli = {};
		socklen_t addrlen = sizeof(addrcli);
		ssize_t rb = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&addrcli, &addrlen);
		if(rb == -1)
		{
			perror("recvfrom");
			return -1;
		}
		printf("服务器：向%s:%u发送响应\n", inet_ntoa(addrcli.sin_addr), ntohs(addrcli.sin_port));
		if(sendto(sockfd, buf, rb, 0, (struct sockaddr*)&addrcli, addrlen) == -1)
		{
			perror("sendto");
			return -1;
		}
	}
	return 0;
}
