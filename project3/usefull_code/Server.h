#ifndef SERVER_H
#define SERVER_H


#include <stdio.h>
#include <sys/wait.h>	     /* sockets */
#include <sys/types.h>	     
#include <sys/socket.h>	     
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	         /* gethostbyaddr */
#include <unistd.h>	         /* fork */		
#include <stdlib.h>	        
#include <ctype.h>	         
#include <signal.h>          /* signal */
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include "MyStructs.h"


void perror_exit(char *message);

bool isNumber(char * str);
int argvCheck(int argc, char ** argv);

void d_connection(struct sockaddr**sptr,struct sockaddr**cptr,
	struct sockaddr_in *server, struct sockaddr_in *client,int *sock,int port,socklen_t *clientlen);

void termination (int sig);
void * LogOn_Thread(void *ptr);

void UserOn(char * clientIP,unsigned short port);
void connect_to_client(struct sockaddr **serverptr,struct hostent **rem,char* sname,struct sockaddr_in *server, int *sock,short port);

void prepare_GetList(char * buf,char * clientIP,unsigned short port );

void UserOff(char * clientIP,unsigned short port);


void convert_to_binary(unsigned short port, char * b);




#endif