#ifndef MainUtils_hpp
#define MainUtils_hpp

#define INFTY std::numeric_limits<int>::max()

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <poll.h>
#include "Database.hpp"
#include "Structs.hpp"
#include <fstream>
#include <sstream>
#include <limits>
#include <utility>
#include <unistd.h>
#include <signal.h>
#include <csignal>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>      
#include <netinet/in.h>      /* internet sockets */
#include <netdb.h>  /* gethostbyaddr */

using namespace  std ;

struct Parameters
{
    string patientRecordsFile;
    int diseaseHashtableNumOfEntries;
    int countryHashtableNumOfEntries;
    int bucketSize;
    int bufferSize;
    int read_filedesc;
    int write_filedesc;
    char fifo_read[100];
    char fifo_write[100];
    char serverIP[20];
    int serverPort;
    int workerPort;

};


class MainUtils {
private:
    Parameters params ;
    CasesList *caselist;
    Hashtable *countryHashtable, *diseaseHashtable ;
public:
    MainUtils(int argc,char **argv);
    int isNumber(char* number);
    inline bool file_exists (string& name);
    void getArgs(int argc,char **argv);
    void printArgs();
    void Create_Open_fifos();
    void Send_Message(int fd,char *message); // stelnei ena minuma sto receiver
    char * Receive_Message(int fd);
    void Sent_results_from_Countries();
    void Worker_Server_communication();
    void creating_passive_endpoint(struct sockaddr**sptr,struct sockaddr**cptr,
        struct sockaddr_in *server, struct sockaddr_in *client,int *sock,int port,socklen_t *clientlen);
    void create_whoServer_connection(struct sockaddr **serverptr,struct hostent **rem,char* sname,struct sockaddr_in *server, int *sock,int port);
    void send_Statistics_and_Port_to_whoServer(int worker_port);
    void perror_exit(char *message);
    void getDirectories_of_Cases();
    string getCountry(string path);
    void add_Cases(string recordID,string entry_exit_key,string patientFirstName,string patientLastName,string country,string date,string diseaseID,string age);
    void readCommands();
    void parseCommands(int fd,char *buffer);
    ~MainUtils(){
        delete caselist;
        delete countryHashtable;
        delete diseaseHashtable;
    }
protected:

};

MainUtils* mainutils;

void Create_Open_fifos();
void exitFromSignal(int signo);

#endif