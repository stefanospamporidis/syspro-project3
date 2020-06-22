#ifndef whoClient_hpp
#define whoClient_hpp

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iterator>
#include <algorithm> 
#include <unistd.h>
#include <pthread.h>
#include <fstream>
#include <sys/socket.h>	     
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	/* gethostbyaddr */

using namespace  std;

#define BUFFERSIZE 200

typedef struct Parameters
{
	char queryFile[1024];
	int numThreads;
	int servPort;
	char servIP[20]; //ftiaxnoume mia parapanw 8esi gia ti periptwsi opou oi xwres px einai 11 kai oi workers 3

}Parameters;

int isNumber(char* number);
inline bool file_exists (string& name);
int number_of_lines_in_file();
void getArgs(int argc,char **argv);
void printArgs(void);
void readQueries(char *queryFile);
void* send_receive_Query(void* args); //stelnei to query sto server mesw tou thread kai dexetai ta apotelesmata
void create_whoServer_connection(struct sockaddr **serverptr,struct hostent **rem,char* sname,struct sockaddr_in *server, int *sock,int port);
void Send_Message(int fd,char *message);
char* Receive_Message(int fd);
void initialize_All();
void perror_exit(char *message);

#endif