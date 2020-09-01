#include "net.h"
#include <signal.h>
#include <sys/wait.h>

void cli_data_handle(void * arg);
void sig_child_handle(int signo)
{
	if(SIGCHLD == signo)
	{
		waitpid(-1, NULL, WNOHANG);
	}
}

int main(void)
{
	int fd = -1;
	//struct sockaddr_in sin;
	signal(SIGCHLD, sig_child_handle);
	/*1.创建socket fd*/
	if((fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(1);
	}

	/*允许绑定地址快速重用*/
	int b_reuse = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &b_reuse, sizeof(int));

#if 0
	struct sockaddr_in sin;
	/*2绑定*/
	/*2.1填充struct sockaddr_in 结构体变量*/
	bzero(&sin,sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SERV_PORT); //网络字节序的端口号
	
/*优化1：让服务器程序能绑定在任意的IP上*/
#if 1
//	sin.sin_addr.s_addr = inet_addr(SERV_IP_ADDR);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
#else
	if( inet_pton(AF_INET, SERV_IP_ADDR, (void *)&sin.sin_addr.s_addr) != 1)
	{
		perror("inet_pton");
		exit(1);
	}

#endif

#else
	/*2.1填充struct sockaddr_un 结构体变量*/

	struct sockaddr_un sun;
	bzero(&sun, sizeof(sun));
	sun.sun_family = AF_LOCAL;
	
	/*如果UNIX_DOMAI_FILE所指向的文件已存在，则删除*/
	if(!access(UNIX_DOMAIN_FILE, F_OK))
	{
		unlink(UNIX_DOMAIN_FILE);
	}
	strncpy(sun.sun_path, UNIX_DOMAIN_FILE, strlen(UNIX_DOMAIN_FILE));
#endif
	/*2.2 绑定*/
   if( bind(fd, (struct sockaddr *)&sun, sizeof(sun)) < 0 )
   {
	   perror("bind");
	   exit(1);
   }
   /*3.调用listen把主动套接字变成被动套接字*/
  if (listen(fd, BACKLOG) < 0)
  {
  	   perror("listen");
	   exit(1);
  }
  printf("Unix domain server starting ...OK!\n");
  int newfd = -1;
/*阻塞等待客户端连接请求*/
#if 0

#else
	struct sockaddr_in cin;
	socklen_t addrlen = sizeof(cin);
	while(1)
	{
		pid_t pid = -1;
		if((newfd =accept(fd, NULL, NULL)) < 0)//此处改为NULL
		{
			perror("accept");
			break;
		}
		/*创建一个子进程用于处理已建立连接的客户的交互数据*/
		if((pid =fork() )< 0)
		{
			perror("fork");
			break;
		}

		if(0 == pid)
		{	//子进程
			close(fd);
			
	
			printf("Client is connected!\n");
			cli_data_handle(&newfd);
			return 0;

		}
		else
		{	//p实际上此处pid > 0,父进程中
			close(newfd);
		}

	}

	
#endif
  close(fd);
  return 0;
}
/*子进程执行函数*/
void cli_data_handle(void * arg)
{
	int newfd = *(int *)arg;

	printf("Child handler process: newfd =%d\n", newfd);
  //...和newfd进行数据读写
#define SERV_RESP_STR "Server"

  int ret = -1;
  char buf[BUFSIZ];
  char resp_buf[BUFSIZ+10];
  while(1)
  {
  	bzero(buf, BUFSIZ);
  	do{
  			ret = read(newfd, buf, BUFSIZ-1); //阻塞
 	 }while(ret < 0 && EINTR == errno);
  	if(ret < 0)
 	 {
  		perror("read");
		exit(1);
 	 }
 	 if(!ret) //对方已关闭
 	 {
  		break;
 	 }
     printf("Receive data: %s\n", buf); 
	bzero(resp_buf, BUFSIZ+10);
	strncpy(resp_buf, SERV_RESP_STR, strlen(SERV_RESP_STR));
	strcat(resp_buf, buf);
	 do
	{
		ret = write(newfd, resp_buf, strlen(resp_buf));
	}while(ret < 0 && EINTR == errno);

	 if(!strncasecmp(buf, QUIT_STR, strlen(QUIT_STR))) //用户输入了quit字符
	 {
	 	printf("Client(fd = %d) is exiting!\n", newfd);
		break;
	 }
  }
  close(newfd);

}
