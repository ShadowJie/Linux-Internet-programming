#include "net.h"

/* ./client serv_ip serv_port */

void usage(char *s)
{

	printf("\n\t unix_domain_file\n\n");
}
int main(int argc, char **argv)
{
	int fd = -1;

	int port = -1;
	struct sockaddr_un sun;
	if(argc != 2)
	{
		usage(argv[0]);
		exit(1);
	}

	/*1.创建socket fd*/
	if((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(1);
	}
	
	/*2.连接服务器*/
	/*2.1填充struct sockaddr_un 结构体变量*/
	bzero(&sun,sizeof(sun));
	sun.sun_family = AF_LOCAL;
	/*确保UNIX_DOMAIN_FILE要先存在，不存在则退出*/
	if(access(UNIX_DOMAIN_FILE, F_OK | W_OK) < 0)
	{
		exit(1);
	}
	strncpy(sun.sun_path, UNIX_DOMAIN_FILE, strlen(UNIX_DOMAIN_FILE));

	if (connect(fd,(struct sockaddr *)&sun, sizeof(sun)) < 0)
	{
		perror("connect");
		exit(1);
	}

	printf("Unix client starting ... OK!\n");
#if 0
	/*3.读写数据*/
	char buf[BUFSIZ];
	while(1)
	{
		bzero(buf, BUFSIZ);
		if (fgets(buf, BUFSIZ-1, stdin) == NULL)
		{
			continue;
		}
		write(fd, buf, strlen(buf));
		if(!strncasecmp(buf, QUIT_STR, strlen(QUIT_STR))) //用户输入了quit字符
		 {
	 		printf("Client is exiting!\n");
			break;
		 }
	
	}
#else
	int ret = -1;
	fd_set rset;
	int maxfd = -1;
	struct timeval tout;
	char buf[BUFSIZ];
	while(1)
	{
		FD_ZERO(&rset);
		FD_SET(0, &rset);
		FD_SET(fd, &rset);
		maxfd = fd;
		
		tout.tv_sec = 5;
		tout.tv_usec = 0;
		select(maxfd+1, &rset, NULL, NULL, &tout);
		if(FD_ISSET(0, &rset))
		{	
			//标准键盘上有输入
			//读键盘输入，发送到网络套接字fd
			bzero(buf, BUFSIZ);
			do
			{
				ret = read(0 ,buf, BUFSIZ-1);
			} while(ret < 0 && EINTR == errno);
			if(ret < 0)
			{
				perror("read");
				continue;
			}
			if(!ret) break; 
			if(write(fd, buf, strlen(buf)) < 0)
			{
				perror("write() to socket");
				continue;
			}
			if(!strncasecmp(buf, QUIT_STR, strlen(QUIT_STR))) //用户输入了quit字符
		    {
	 			printf("Client is exiting!\n");
				break;
		 	}
		}
		if(FD_ISSET(fd, &rset))
		{
			//服务器给发送过来了数据
			//读取套接字数据，处理
			bzero(buf, BUFSIZ);
			do
			{
				ret = read(fd ,buf, BUFSIZ-1);
			} while(ret < 0 && EINTR == errno);
			if(ret < 0)
			{
				perror("read from socket");
				continue;
			}
			if(!ret) break; /*服务器关闭*/
			
			//There is a BUG, FIXME!
			printf("server said: %s\n", buf);
			if((strlen(buf) > strlen(SERV_RESP_STR)) && !strncasecmp(buf+strlen(SERV_RESP_STR), QUIT_STR, strlen(QUIT_STR))) //用户输入了quit字符
		    {
	 			printf("Client is exiting!\n");
				break;
		 	}

		}
	}
#endif
	/*4.关闭套接字*/
	close(fd);

	return 0;
}

