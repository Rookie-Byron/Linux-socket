#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "用法：%s <域名>\n", argv[0]);
		return -1;
	}
	struct hostent* host = gethostbyname(argv[1]);
	if(host == NULL)
	{
		perror("gethostbyname");
		return -1;
	}
	if(host->h_addrtype != AF_INET)
	{
		printf("非IPV4地址\n");
		return -1;
	}
	printf("主机官方名：\n");
	printf("\t%s\n", host->h_name);
	printf("主机别名表：\n");
	for(char** ppa = host->h_aliases; *ppa; ++ppa)
	{
		printf("\t%s\n", *ppa);
	}
	printf("主机地址表：\n");
	for(struct in_addr** pp = (struct in_addr**)host->h_addr_list; *pp; ++pp)
	{
		printf("\t%s\n", inet_ntoa(**pp));
	}
	return 0;
}
