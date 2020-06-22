#include "whoServer.hpp"

Parameters params;
List workers_infos_list;
pthread_mutex_t cb_mutex,list_mutex,print_mutex;
pthread_cond_t cv_producer;
pthread_cond_t cv_consumer;

pthread_t * threads;

int isNumber(char* number){ //tsekarw an dinw ari8mo apo to pliktrologio
    int i;
    for (i=0;i<strlen(number);i++){
        if (number[i]<'0' || number[i]>'9') //koitaw an ka8e xaraktiras den einai anamesa stous xaraktires 0 kai 9
            return 0; // an den einai gurnaw 0
    }
    return 1; //alliws gurnaw 1
}   

void getArgs(int argc,char **argv) {
    
    int q=0, s=0, w=0 ,b=0;
    cout << "You have entered " << argc << " arguments:" << "\n"; 
  
    if (argc != 9){
        cout << "La8os ari8mos orismatwn" << endl;
        exit(1);
    }

    for(int i=1; i < 9; i+=2){
        if (string(argv[i]) == "-q"){  
            if (isNumber(argv[i+1]) == 0){
                cout << "-q must be a number" <<endl;
                exit(1);
            }
            else{
                params.queryPortNum = atoi(argv[i+1]);
                q = 1;
            }
        }
        if (string(argv[i]) == "-s"){  
            if (isNumber(argv[i+1]) == 0){
                cout << "-s must be a number" <<endl;
                exit(1);
            }
            else{
                params.statisticsPortNum = atoi(argv[i+1]);
                s = 1;
            }
        }  
        if (string(argv[i]) == "-w"){ 
            if (isNumber(argv[i+1]) == 0){
                cout << "-w must be a number" <<endl;
                exit(1);
            }
            else{
                params.numThreads = atoi(argv[i+1]);
                w = 1;
            }
        }
        if (string(argv[i]) == "-b"){ 
            if (isNumber(argv[i+1]) == 0){
                cout << "-b must be a number" <<endl;
                exit(1);
            }
            else{
                params.bufferSize = atoi(argv[i+1]);
                b = 1;
            }
        }
        
    }
    
    //tsekarw an do8ikan ola ta orismata
    if (q != 1 || s != 1 || w != 1 || b!=1){ //einai oi counter twn orismatwn opote kai an oloi exoun par8ei mia fora tote 8a prepei na einai oloi 1 alliws einai la8os
        cout << "Wrong arguments" << endl << "./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize" <<endl;
        exit(1);
    }
}


void printArgs(void) { //kanoume print ta arguments pou dinontai apo ti grammi entolwn
    cout << "queryPortNum " << params.queryPortNum << endl ; //perissotero to kanoume gia na epivevaiwsoume oti pernane swsta ta orismata
    cout << "statisticsPortNum " << params.statisticsPortNum << endl ;
    cout << "numThreads " << params.numThreads << endl ;
    cout << "bufferSize " << params.bufferSize << endl ;
}

void initialize_All(Circle_Buffer &circle_buffer){

	pthread_mutex_init(&cb_mutex,NULL);
	pthread_mutex_init(&list_mutex,NULL);
	pthread_mutex_init(&print_mutex,NULL);
	pthread_cond_init(&cv_producer,NULL);
	pthread_cond_init(&cv_consumer,NULL);
	threads = new pthread_t[params.numThreads];
	int error;
	for (int i = 0;i < params.numThreads; i++){
		if (error = pthread_create(&threads[i],NULL,communication_Thread,(void*) &circle_buffer)){
			cout << "pthread_Create Failed" << endl;
			pthread_exit(NULL);
		}
	}

}

void * communication_Thread(void* arguments){

	Circle_Buffer* cb = (Circle_Buffer*) arguments;
	Buffer_node buffer_node;
	while(true){
		extract_from_Buffer(*cb,buffer_node);
		if (buffer_node.type_of_job == 1){
			//na kalesei ti sunartisi gia na sunde8ei me worker
			cout << "Pairnw statistika apo ton server" << endl;
			receive_info_from_Workers(buffer_node.end_point_port);
		}
		else{
			//na kalesei ti sunartisi gia na sunde8ei me whoClient
			query_Manager(buffer_node.end_point_port);
		}
	}

}

void receive_info_from_Workers(int socket_fd){

	char * buffer = Receive_Message(socket_fd); //edw pairnw ta statistics
	pthread_mutex_lock(&print_mutex);
	cout << buffer << endl;
	pthread_mutex_unlock(&print_mutex);
	/*delete [] buffer;
	buffer = Receive_Message(socket_fd);*/
	pthread_mutex_lock(&list_mutex);
	workers_infos_list.AddNode(buffer);
	cout << "List members = " << workers_infos_list.counter << endl;
	pthread_mutex_unlock(&list_mutex);
	delete [] buffer;
	close(socket_fd);

}

void query_Manager(int socket_fd){ //auto einai to socket tou client

	char *query = Receive_Message(socket_fd); // lamvanei to query apo to whoclient
	cout << "To query eftase sto server = " << query << endl;
	char *query_result;
	int number_of_workers = workers_infos_list.counter;
	int worker_port;
	char num_of_workers[10];
	sprintf(num_of_workers,"%d",number_of_workers);
	//Send_Message(socket_fd,num_of_workers);
	ListNode *temp = workers_infos_list.head;
	for (int i = 0; i < number_of_workers; i++){
		worker_port = temp->worker_port;
		cout << worker_port << endl;
		query_result = ask_Worker(worker_port,query);
		pthread_mutex_lock(&print_mutex);
		cout << query_result << endl;
		pthread_mutex_unlock(&print_mutex);
		Send_Message(socket_fd,query_result);
		delete [] query_result;
		temp = temp->next;
	}
	delete [] query;
	close(socket_fd);

}

char* ask_Worker(int worker_port,char *query){

	int worker_fd;
	struct sockaddr_in server;
	struct sockaddr * serverptr = (struct sockaddr*) &server;
	struct hostent *rem;
	char sname[20] = "127.0.0.1";
	create_worker_connection(&serverptr,&rem,sname,&server,&worker_fd,worker_port);
	Send_Message(worker_fd,query);
	char *query_result = Receive_Message(worker_fd);
	return query_result;

}

void Send_Message(int fd,char *message){ 
	int size = strlen(message); //size tou message pou 9a steilei o sender
	char temp[10]; //ftiaxnw ena temp
	sprintf(temp,"%d$",size); // to temp krataei to mege8os tou message px 1024$
	write(fd,temp,9); //kanw write sto fifo to 1024$
	write(fd,message,size); //kanw write to message
}

char* Receive_Message(int fd){

	int bufferSize = params.bufferSize;
	char *input_message; // to message
	char temp[10]; //to mege8os tou message
	char *token; //8a apomonwsei ton ari8mo apo to dollario
	read(fd,temp,9); //diavazw to temp pou einai to mege8os tou message
	token = strtok(temp,"$"); //aponwnw ton ari8mo gia na 3erw posa bytes 8a diavasw
	int size = atoi(token); //te metatrepw se ari8mo
	input_message = (char*) malloc(size * sizeof(char) + 1); //o pinakas pou 8a valw mesa to message
	if (size < bufferSize)
		bufferSize = size;
	int counter = 0; //counter pou au3anetai mexri na ginei megalutero iso tou size
	int n = 0; //krataei posous xaraktites diavasame
	char *packet_ptr = input_message; //deixnei se poio simeio 8a gra4oume sto message
	while (counter < size){ //ekteleitai mexri na ginei megalutero iso tou size tou message
		packet_ptr += n; // einai i 8esi tou prwtou xaraktira pou 8a gra4oume sto epomeno minuma
		n = read(fd,packet_ptr,bufferSize); //oi xaraktires pou diavase
		if (counter + n > size){
			n = size - (counter + n);
		}
		counter += n; //au3anoume to counter n xaraktires
	}
	input_message[size] = 0; //kanoume to teleutaio xaraktira tou message 0
	return input_message;
}

void insert_to_Buffer(Circle_Buffer &circle_buffer,int type_of_job,int end_point_port){

	pthread_mutex_lock(&cb_mutex);
	while(circle_buffer.isFull()){
		pthread_cond_wait(&cv_producer,&cb_mutex);
	}
	circle_buffer.insertNode(type_of_job,end_point_port);
	pthread_cond_signal(&cv_consumer);
	pthread_mutex_unlock(&cb_mutex);

}

void extract_from_Buffer(Circle_Buffer &circle_buffer,Buffer_node & buffer_node){

	pthread_mutex_lock(&cb_mutex);
	while(circle_buffer.isEmpty()){
		pthread_cond_wait(&cv_consumer,&cb_mutex);
	}
	circle_buffer.removeNode(buffer_node);
	pthread_cond_signal(&cv_producer);
	pthread_mutex_unlock(&cb_mutex);

}

void create_worker_connection(struct sockaddr **serverptr,struct hostent **rem,char* sname,struct sockaddr_in *server, int *sock,int port){
	*serverptr = (struct sockaddr*)server;
    if ((*sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    	perror_exit((char*)"socket");	
    if ((*rem = gethostbyname(sname)) == NULL) {	
	   	perror((char*)"gethostbyname"); exit(1);
    }
    server->sin_family = AF_INET;       /* Internet domain */
    memcpy(&server->sin_addr, (*rem)->h_addr, (*rem)->h_length);
    server->sin_port = htons(port);         /* Server port */
    /* Initiate connection */
    if (connect(*sock, *serverptr, sizeof(*server)) < 0){
	   	perror_exit((char*)"connect");
    }
    printf("Connecting to %s port %d\n", sname, port);
}

void creating_passive_endpoint(struct sockaddr**sptr,struct sockaddr**cptr, struct sockaddr_in *server, struct sockaddr_in *client,int *sock,int port,socklen_t *clientlen){

	*sptr = (struct sockaddr *)server;
	*cptr = (struct sockaddr *)client;
	if ((*sock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        perror_exit((char *)"socket");
	}
    server->sin_family = AF_INET;       /* Internet domain */
    server->sin_addr.s_addr = htonl(INADDR_ANY);
    server->sin_port = htons(port);    //printf("eimai edw bind\n");  /* The given port */
    if (bind(*sock, *sptr, sizeof(*server)) < 0){
        perror_exit((char*)"bind");
    }
    /* Listen for connections */
    if (listen(*sock, 5) < 0) perror_exit((char*)"listen");
}

void managing_communications(Circle_Buffer &circle_buffer){

	int worker_sock, whoClient_sock, worker_newsock, whoClient_newsock;
	short worker_server_port = params.statisticsPortNum, whoClient_server_port = params.queryPortNum;

	struct sockaddr_in worker_server, worker_client, whoClient_server, whoClient_client;
	socklen_t worker_clientlen,whoClient_clientlen;
	struct sockaddr *worker_serverptr,*whoClient_serverptr;
	struct sockaddr *worker_clientptr,*whoClient_clientptr;

	//ftiaxnw ta passive endpoints gia tous worker kai to client kai akouei o server se auta
	creating_passive_endpoint(&worker_serverptr,&worker_clientptr,&worker_server,&worker_client,&worker_sock,worker_server_port,&worker_clientlen);
	creating_passive_endpoint(&whoClient_serverptr,&whoClient_clientptr,&whoClient_server,&whoClient_client,&whoClient_sock,whoClient_server_port,&whoClient_clientlen);

	struct pollfd pollfds[2];
	pollfds[0].fd = worker_sock; //dinei to fd tou passive endpoint tou worker sti prwti 8esi tou pinaka poll 
	pollfds[0].events = POLLIN;
	pollfds[1].fd = whoClient_sock; //dinei to fd tou passive endpoint tou whoClient sti prwti 8esi tou pinaka poll 
	pollfds[1].events = POLLIN;

	while(true){
		poll(pollfds,2,10);
		if (pollfds[0].revents & POLLIN){
			if ((worker_newsock = accept(worker_sock,worker_clientptr,&worker_clientlen)) < 0){
				perror_exit((char*)"Accept Worker Failed");
			}
			insert_to_Buffer(circle_buffer,1,worker_newsock);
		}
		if (pollfds[1].revents & POLLIN){
			if ((whoClient_newsock = accept(whoClient_sock,whoClient_clientptr,&whoClient_clientlen)) < 0){
				perror_exit((char*)"Accept whoClient Failed");
			}
			insert_to_Buffer(circle_buffer,2,whoClient_newsock);
		}
	}

}

void perror_exit(char *message){
    perror(message);
    pthread_exit(NULL);
}

//./whoServer -q 9000 -s 9500 -w 10 -b 200
int main(int argc,char **argv){
	getArgs(argc, argv);
    printArgs();
    Circle_Buffer circle_buffer(params.bufferSize);
    initialize_All(circle_buffer);
    managing_communications(circle_buffer);



}