#include "Server.h"


ClientList clients;
ThreadList threads;

pthread_mutex_t mtx;


int main(int argc, char *argv[]){

	int tempPort=argvCheck(argc,argv);
	if(tempPort==0){
		cout<<"Wrong Arguments"<<endl;
		return 1;
	}
	cout<<tempPort<<endl;
	int sock, newsock, i, err;
	short server_port=9050;

	struct sockaddr_in server, client;
	socklen_t clientlen;
	struct sockaddr *serverptr;
	struct sockaddr *clientptr;

	pthread_t  thread_id;
	
	pthread_mutex_init (& mtx , NULL ); 

	signal(SIGINT, termination);
	
	d_connection(&serverptr,&clientptr,&server,&client,&sock,server_port,&clientlen);
	//command port
	while (1) {
	        /* accept connection */
	    	if ((newsock = accept(sock, clientptr, &clientlen)) < 0) perror_exit((char*)"accept");

	    	if ( err = pthread_create (&thread_id, NULL , LogOn_Thread ,( void *) &newsock)) { 
				perror_exit ((char*)" pthread_create " ); 
			}
	    	
	    	printf("Accepted connection\n");
	    	threads.insertTheard(thread_id);


   	 }
    		

	pthread_exit ( NULL );
	

}

void connect_to_client(struct sockaddr **serverptr,struct hostent **rem,char* sname,struct sockaddr_in *server, int *sock,short port){
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

void perror_exit(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}

void * LogOn_Thread(void *ptr){
	char buf[1000];
	int r,socket=*((int*)ptr);

	char * token;

	r=read(socket,buf,1000);
	token=strtok(buf," ");

	if(!strcmp(token,"LOG_ON")){
		cout<<"ep"<<endl;
		token=strtok(NULL," ");
		char clientIP[20];
		strcpy(clientIP,token);
		token=strtok(NULL," ");
		unsigned short port=*((unsigned short *)token);
		port=ntohs(port);
		//cout<<clientIP<<" "<<port<<endl; 
		UserOn(clientIP,port);

		pthread_mutex_lock(&mtx);
		clients.insertNode(clientIP,port);
		pthread_mutex_unlock(&mtx);
		//cout<<clients.head->ip_address<<" "<<clients.head->port<<endl;

		r=read(socket,buf,1000);
		prepare_GetList(buf,clientIP,port);
		write(socket,buf,1000);




		close(socket);
	}
	else{
		token=strtok(NULL," ");
		char clientIP[20];
		strcpy(clientIP,token);
		token=strtok(NULL," ");
		unsigned short port=*((unsigned short *)token);
		port=ntohs(port);

		if(!clients.findNode(clientIP,port)){


			write(socket,"ERROR_IP_PORT_NOT_FOUND_IN_LIST ",100);
			close(socket);
		}
		else{
			clients.make_inactive(clientIP,port);

			
			write(socket,"OKAY ",100);
			close(socket);
			UserOff(clientIP,port);
		}
		
	}
	
	

}

void UserOff(char * clientIP,unsigned short port){
	if(clients.head==NULL){
		cout<<"List Empty"<<endl;
		return;
	}

	int sock;
	unsigned short serving_port;
	char hostname[100];
	struct sockaddr_in server;
	socklen_t clientlen;
	struct sockaddr *serverptr = (struct sockaddr*)&server;
   	struct hostent *rem;


	char buf[100],b[10];
	convert_to_binary(port,b);
	sprintf(buf,"USER_OFF %s %s ",clientIP,b);



	ClientNode * temp=clients.head;
	while(temp){
		if(temp->active && (strcmp(temp->ip_address,clientIP) && temp->port!=port ) ){

			serving_port=temp->port;
			strcpy(hostname,temp->ip_address);
			connect_to_client(&serverptr,&rem,hostname,&server,&sock,serving_port);
			write(sock,buf,100);
			close(sock);
		}
		temp=temp->next;

	}

}

void prepare_GetList(char * buf,char * clientIP,unsigned short port){
	if(clients.head->next==NULL){
		cout<<"Only one  client"<<endl;
		return;
	}
	int counter=0;
	//strcpy(buf,"CLIENT_LIST ");
	ClientNode * temp=clients.head;
	while(temp){
		if(temp->active && (strcmp(temp->ip_address,clientIP) && temp->port!=port ) ){
			counter++;
			char b[10];
			convert_to_binary(temp->port, b);
			sprintf(buf,"%s %s ",temp->ip_address,b);



		}
		temp=temp->next;
	}

	sprintf(buf,"CLIENT_LIST %d %s ",counter,buf);

}

void UserOn(char * clientIP,unsigned short port){

	if(clients.head==NULL){
		cout<<"List Empty"<<endl;
		return;
	}

	int sock;
	unsigned short serving_port;
	char hostname[100];
	struct sockaddr_in server;
	socklen_t clientlen;
	struct sockaddr *serverptr = (struct sockaddr*)&server;
   	struct hostent *rem;


	char buf[100],b[10];
	convert_to_binary(port,b);
	sprintf(buf,"USER_ON %s %s ",clientIP,b);



	ClientNode * temp=clients.head;
	while(temp){
		if(temp->active){
			serving_port=temp->port;
			strcpy(hostname,temp->ip_address);
			connect_to_client(&serverptr,&rem,hostname,&server,&sock,serving_port);
			write(sock,buf,100);
			close(sock);
		}
		
		temp=temp->next;

	}

}



void termination (int sig) {
	ThreadNode * temp=threads.head;
	int err;

	while(temp){
		if ( err = pthread_join ( temp->t_id, NULL )) {
			perror_exit ((char*)" pthread_join " ); 
		}
		temp=temp->next;

	}
	if ( err = pthread_mutex_destroy (& mtx )) {
		perror_exit ((char*)" pthread_mutex_destroy "); 
	}
	cout<<"Program terminated"<<endl;
	pthread_exit(NULL);

	
}

void convert_to_binary(unsigned short port, char * b){
	unsigned short net_port;
	net_port=htons(port);
	char * ptr=(char*)&net_port;
	b[0]=*ptr;
	b[1]=*(ptr+1);
	b[2]='\0';


}
int argvCheck(int argc, char ** argv){
	int i;
	int counter=0;
	int port;
	if(argc!=3)
		return 0;
	
	for(i=1;i<argc;i+=2){
		if(!strcmp(argv[i],"-p")){
			if(!(isNumber(argv[i+1])))
				return false;
			port=atoi(argv[i+1]);
			counter++;
		}
	}
	if (counter!=1)
		return 0;
	return port;
}
bool isNumber(char * str){	//find if it's a num using isdigit()
	int len=strlen(str);
	int i;
	for(i=0,i<len;i++;)
		if(!isdigit(str[i]))
				return false;

	return true;
}
