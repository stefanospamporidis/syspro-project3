#include "Client.h"


using namespace std;

pthread_mutex_t mtxClient, mtxBuffer,mtxLast; /* mutex */
pthread_cond_t cv_prod, cv_consume; /* the condition variable */
pthread_t * threads;
CycleBuffer * cBuffer;
UserArguments userArguments;

ClientList clients;
InputList inputList;


int main(int argc,char* argv[]){
	/*prwta kanoyme elenxo ton argv, 
	meta dimiourgoume tin lista me ta pathnames apo to client dir
	meta kanoume ton kikliko buffer 
	meta kanoume ta threads 
	fitaxnoume to passive endpoint tou client
	meta kanoyme logon ston server 
	meta pernoume tin list tws client apo ton server kai tin bazoyme stin client list
	episis bazoume olous toys clients stoon kikliko buffer xwris alles plirofories
	klinoume to socket tou logon 
	katopin arxizoume na kanoume accept apo allous client h ton server mesa se ena atermon while
	ilopoioume to ctrl + c gia logoff
	*/
	/*char buf[100],b[10];
	convert_to_binary(portNum,b);
	sprintf(buf,"LOG_ON %s %s ","192.10.78.15",b);
	



	



	write(sock,buf,100);
	sleep(20);
	*/

	if(!argvCheck(argc,argv)){
		cout<<"Wrong Arguments"<<endl;
		return 1;
	}
	//cout<<userArguments.bufferSize<<" "<<userArguments.serverPort<<" "<<userArguments.dirName<<" "<<userArguments.serverIP<<endl;
	signal(SIGINT, termination);

	int sock;
	unsigned short serving_port=9050,net_port;
	unsigned short portNum=9000; // apo to argv to pernoume.
	char* hostname="osboxes";
	struct sockaddr_in server;
	//socklen_t clientlen;
	struct sockaddr *serverptr = (struct sockaddr*)&server;
   	struct hostent *rem;

   	recDirContent("input",&inputList);//diavazei to dirName
   	//inputList.Print(inputList.head);

   	initializer();

	makeconnection_serving(&serverptr,&rem,hostname,&server,&sock,serving_port); printf("ip: %s\n",rem->h_name);
	char buf[1000],b[10];
	convert_to_binary(portNum,b);

	sprintf(buf,"LOG_ON %s %s ","osboxes",b);
	
	write(sock,buf,1000);

	write(sock,"GET_CLIENTS ",1000);


	read(sock,buf,1000);

	close(sock);

	

	//na mpoun stin lista oloi oi clients
	int newsock;
	struct sockaddr_in clientServer, otherClient;
	socklen_t otherClientlen;
	struct sockaddr *clientServerptr;
	struct sockaddr *otherClientptr;

	d_connection(&clientServerptr,&otherClientptr,&clientServer,& otherClient,&sock,portNum/*server_port*/,&otherClientlen);

	while (1) {
	        /* accept connection */
	    	if ((newsock = accept(sock, otherClientptr, &otherClientlen)) < 0) perror_exit((char*)"accept");
	    	
	    	printf(" CLient Accepted connection\n");

	    	read(newsock,buf,1000);

	    	char * token=strtok(buf," ");
	    	if(!strcmp(token,"GET_FILE_LIST")){
	    		//
	    	}
	    	else if(!strcmp(token,"GET_FILE")){
	    		//
	    	}
	    	else if(!strcmp(token,"USER_ON")){
	    		userOn(token);
	    	}
	    	else if(!strcmp(token,"USER_OFF")){

	    	}

	    	close(newsock);

   	 }



	

	
	return 0;

}


void termination (int sig) {
	int err,i;

	int sock;
	unsigned short serving_port=9050,net_port;
	unsigned short portNum=9000; // apo to argv to pernoume.
	char* hostname="osboxes";
	struct sockaddr_in server;
	//socklen_t clientlen;
	struct sockaddr *serverptr = (struct sockaddr*)&server;
   	struct hostent *rem;


	makeconnection_serving(&serverptr,&rem,hostname,&server,&sock,serving_port);

	char buf[1000],b[10];
	convert_to_binary(portNum,b);
	sprintf(buf,"USER_OFF %s %s ",rem->h_name,b);

	write(sock,buf,1000);

	close(sock);



	pthread_mutex_lock(&mtxLast);
	cBuffer->buffer[userArguments.bufferSize].version[0]=1;
	pthread_mutex_unlock(&mtxLast);

	for(i=0;i<userArguments.workerThreads;i++){
		if ( err = pthread_join ( threads[i], NULL )) {
			perror_exit ((char*)" pthread_join " ); 
		}

	}
	if ( err = pthread_mutex_destroy (& mtxBuffer )) {
		perror_exit ((char*)" pthread_mutex_destroy "); 
	}
	if ( err = pthread_mutex_destroy (& mtxClient )) {
		perror_exit ((char*)" pthread_mutex_destroy "); 
	}
	if ( err = pthread_mutex_destroy (& mtxLast )) {
		perror_exit ((char*)" pthread_mutex_destroy "); 
	}
	if ( err = pthread_cond_destroy (& cv_prod )) {
		perror_exit ((char*)" pthread_cond_destroy "); 
	}
	if ( err = pthread_cond_destroy (& cv_consume )) {
		perror_exit ((char*)" pthread_cond_destroy "); 
	}
	cout<<"Client terminated"<<endl;
	pthread_exit(NULL);

	
}



void userOn(char * buf){
	char * token=strtok(NULL," ");
	char bufIp[100];
	strcpy(bufIp,token);
	token=strtok(NULL," ");
	unsigned short * portPtr=((unsigned short *)token);
	unsigned short port=*portPtr;
	port=ntohs(port);

	pthread_mutex_lock(&mtxClient);
	clients.insertNode(bufIp,port);
	pthread_mutex_unlock(&mtxClient);

	BufferElement element;
	element.setBufferElement(bufIp,port);

	insertElementToBuffer(&element);

}

void d_connection(struct sockaddr**sptr,struct sockaddr**cptr, struct sockaddr_in *server, struct sockaddr_in *client,int *sock,int port,socklen_t *clientlen){

	*sptr = (struct sockaddr *)server;
	*cptr = (struct sockaddr *)client;
	if ((*sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        	perror_exit((char *)"socket");
    	server->sin_family = AF_INET;       /* Internet domain */
    	server->sin_addr.s_addr = htonl(INADDR_ANY);
    	server->sin_port = htons(port);    //printf("eimai edw bind\n");  /* The given port */
    	if (bind(*sock, *sptr, sizeof(*server)) < 0)
        	perror_exit((char*)"bind");
    	/* Listen for connections */
    	if (listen(*sock, 5) < 0) perror_exit((char*)"listen");
    	//printf("Listening for connections to port %d\n", port);
    	//if ((*sock = accept(*sock, *cptr, clientlen)) < 0) perror_exit("accept");
    	//printf("Create passive connection\n");
}

void insertElementToBuffer(BufferElement * element){
	
	while(cBuffer->isFull()){		//edw h oura einai gemath kai peftei gia "upno"
		pthread_cond_wait(&cv_prod, &mtxBuffer);	
	}
	cBuffer->insertElement(element->ip_address,element->port,element->pathName,element->version);

	pthread_mutex_unlock(&mtxBuffer);	//printf("SERVER THREAD enqueue---------------------------\n");
	pthread_cond_signal(&cv_consume);
}

void initializer(){
	pthread_mutex_init (& mtxClient , NULL ); 
	pthread_mutex_init (& mtxBuffer , NULL ); 
	pthread_mutex_init (& mtxLast , NULL ); 
	pthread_cond_init (&cv_prod , NULL ); 
	pthread_cond_init (&cv_consume , NULL );
	cBuffer=new CycleBuffer(userArguments.bufferSize);


	int i, err;
	threads = (pthread_t*)malloc( sizeof(pthread_t) * (userArguments.workerThreads));
	for (i=0 ; i <userArguments.workerThreads ; i++)
		if ( err = pthread_create (& threads [i], NULL , consumerThread ,( void *) &i)) { 
			perror_exit ((char*)" pthread_create " ); 
		}
}

void *consumerThread(void * ptr){
	BufferElement temp;
	while(1){	

		pthread_mutex_lock(&mtxBuffer);

		while(cBuffer->isEmpty()){	//an einai adeia h lista
			pthread_cond_wait(&cv_consume, &mtxBuffer);
		}
		pthread_mutex_lock(&mtxLast);
		if(cBuffer->buffer[userArguments.bufferSize].version[0]){
			pthread_mutex_unlock(&mtxLast);
			break;
		} //idopoiei to main thread gia termatismo
			
		pthread_mutex_unlock(&mtxLast);
		cBuffer->removeElement(&temp);

		pthread_mutex_unlock(&mtxBuffer);
		pthread_cond_signal(&cv_prod);
		
		//kaleite h sinatrisi gia na eksipiretithei h aitisi


		
	}
	pthread_exit(NULL);

}

//connection me ton server
void makeconnection_serving(struct sockaddr **serverptr,struct hostent **rem,char* sname,struct sockaddr_in *server, int *sock,short port){
	*serverptr = (struct sockaddr*)server;
    	if ((*sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    		perror_exit((char*)"socket");	
    	if ((*rem = gethostbyname(sname)) == NULL) {	
	   		herror((char*)"gethostbyname"); exit(1);
    	}
        server->sin_family = AF_INET;       /* Internet domain */
    	memcpy(&server->sin_addr, (*rem)->h_addr, (*rem)->h_length);
    	server->sin_port = htons(port);         /* Server port */
    	/* Initiate connection */
    	if (connect(*sock, *serverptr, sizeof(*server)) < 0)
	   	perror_exit((char*)"connect");
    	printf("Connecting to %s port %d\n", sname, port);
}


void convert_to_binary(unsigned short port, char * b){
	unsigned short net_port;
	net_port=htons(port);
	char * ptr=(char*)&net_port;
	b[0]=*ptr;
	b[1]=*(ptr+1);
	b[2]='\0';


}

void perror_exit(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}

bool argvCheck(int argc, char ** argv){ // check to see if arguments are correct..

	int i;
	int counter=0;
	if(argc!=13)
		return false;
	
	for(i=1;i<argc;i+=2){
		if(!strcmp(argv[i],"-d")){
			strcpy(userArguments.dirName,argv[i+1]);
			counter++;
			
		}
		else if(!strcmp(argv[i],"-p")){
			if(!(isNumber(argv[i+1])))
				return false;
			userArguments.clientPort=atoi(argv[i+1]);
			counter++;
		}
		else if(!strcmp(argv[i],"-w")){
			if(!(isNumber(argv[i+1])))
				return false;
			userArguments.workerThreads=atoi(argv[i+1]);
			counter++;
		}
		else if(!strcmp(argv[i],"-b")){
			if(!(isNumber(argv[i+1])))
				return false;
			userArguments.bufferSize=atoi(argv[i+1]);
			counter++;
		}
		else if(!strcmp(argv[i],"-sp")){
			if(!(isNumber(argv[i+1])))
				return false;
			userArguments.serverPort=atoi(argv[i+1]);
			counter++;
		}
		else if(!strcmp(argv[i],"-sip")){
			strcpy(userArguments.serverIP,argv[i+1]);
			counter++;
		}
		

	}
	if (counter!=6)
		return false;
	return true;

}

bool isNumber(char * str){	//find if it's a num using isdigit()
	int len=strlen(str);
	int i;
	for(i=0,i<len;i++;)
		if(!isdigit(str[i]))
				return false;

	return true;

}

void recDirContent(const char *name,InputList * inputList){ //Read clients InputFile and pass it inside an input list
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;
    char path[1024];
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            if(name[strlen(name)-1]=='/'){
            	sprintf(path, "%s%s/", name, entry->d_name);
            }
            else
            	sprintf(path, "%s/%s/", name, entry->d_name);
            //printf("%s\n", path);
            inputList->InsertNode(path,(char*)"File");
            recDirContent(path,inputList);
        } else {
        	struct stat attr;
	    	sprintf(path, "%s%s", name, entry->d_name);
	    	stat(path, &attr);
	    	char version[100];
		    strcpy(version,ctime(&attr.st_mtime));
	    	inputList->InsertNode(path,version);
            //printf("%s\n", path);
        }
    }
    closedir(dir);
}


