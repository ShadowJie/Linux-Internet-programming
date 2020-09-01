#include "net.h"

int main(void)
{
	fd_set rset;
	int maxfd = -1;

	struct timeval tout;

	fd = socket(...);
	bind(fd, ...);
	listen(fd, ...);

	while(1)
	{
		maxfd = fd;
		FD_ZERO(&rset);

		FD_SET(fd. &rset);
		//依次把已经建立好连接的fd加入到集合中，记录下来最大的文件描述符maxfd
		//...FIXME!!
#if 0
		select(maxfd+1, &rset, NULL, NULL, NULL)
#else
		tout.tv_sec = 5;
		tout.tv_usec = 0;
		select(maxfd+1, &rset, NULL, NULL, &tout);
#endif
		if(FD_ISSET(fd, &rset))
		{
			newfd = accept(fd, ...);
		}
	
		//依次判断已建立连接的客户端是否有数据
		//...FIXME!!
	
	}

	return 0;
}
