#ifndef __NET_H__
#define __NET_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/un.h>

//#define SERV_PORT 5002
//#define SERV_IP_ADDR "192.168.163.128"
#define UNIX_DOMAIN_FILE "/tmp/my_domain_file.1"
#define BACKLOG 5

#define QUIT_STR "quit"
#define SERV_RESP_STR "Server"


#endif
