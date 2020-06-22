#ifndef whoServer_hpp
#define whoServer_hpp

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>	     
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	/* gethostbyaddr */
#include <poll.h>
#include "Circle_Buffer.hpp"
#include "List.hpp"

using namespace  std;

typedef struct Parameters
{
	int queryPortNum;
	int statisticsPortNum;
	int numThreads;
	int bufferSize; 

}Parameters;

int isNumber(char* number);
void getArgs(int argc,char **argv);
void printArgs(void);
void initialize_All(Circle_Buffer &circle_buffer); //arxikopoiei ta mutexes , ta condition variables
void * communication_Thread(void* arguments);
void receive_info_from_Workers(int socket_fd);
char* ask_Worker(int worker_port,char *query);
void query_Manager(int socket_fd); //pairnei to query apo to client, ta stelnei stous worker, oi worker epistrefoun tin apaantisi sto server kai ekeinos sto client
void Send_Message(int fd,char *message);
char* Receive_Message(int fd);
void insert_to_Buffer(Circle_Buffer &circle_buffer,int type_of_job,int end_point_port);
void extract_from_Buffer(Circle_Buffer &circle_buffer,Buffer_node & buffer_node);
void creating_passive_endpoint(struct sockaddr**sptr,struct sockaddr**cptr,
	struct sockaddr_in *server, struct sockaddr_in *client,int *sock,int port,socklen_t *clientlen);
void create_worker_connection(struct sockaddr **serverptr,struct hostent **rem,char* sname,struct sockaddr_in *server, int *sock,int port);
void perror_exit(char *message);
void managing_communications(Circle_Buffer &circle_buffer);


#endif