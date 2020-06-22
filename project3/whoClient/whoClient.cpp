#include "whoClient.hpp"

Parameters params;
pthread_mutex_t print_mutex;

pthread_t * threads;

int isNumber(char* number){ //tsekarw an dinw ari8mo apo to pliktrologio
    int i;
    for (i=0;i<strlen(number);i++){
        if (number[i]<'0' || number[i]>'9') //koitaw an ka8e xaraktiras den einai anamesa stous xaraktires 0 kai 9
            return 0; // an den einai gurnaw 0
    }
    return 1; //alliws gurnaw 1
}   

inline bool file_exists (string& name) { //checkarei an to arxeio pou edwsa uparxei an uparxei gurnaei true
    if (FILE *file = fopen(name.c_str(), "r")) { //alliws false 
        fclose(file);
        return true;
    } else {
        return false;
    }   
} 

int number_of_lines_in_file(){

    ifstream myfile(params.queryFile);

    // new lines will be skipped unless we stop it from happening:    
    myfile.unsetf(std::ios_base::skipws);

    // count the newlines with an algorithm specialized for counting:
    unsigned line_count = count(istream_iterator<char>(myfile),istream_iterator<char>(),'\n') + 1; 

    cout << "Lines: " << line_count << endl;

    myfile.close();
    return line_count;

}

void getArgs(int argc,char **argv) {
    
    int q=0, w=0, sp=0 ,sip=0;
    string file;
    cout << "You have entered " << argc << " arguments:" << "\n"; 
  
    if (argc != 9){
        cout << "La8os ari8mos orismatwn" << endl;
        exit(1);
    }

    for(int i=1; i < 9; i+=2){
        if (string(argv[i]) == "-q"){ 
            strcpy(params.queryFile,argv[i+1]);
            file += params.queryFile; //metatropi tou char queryFile se string gia na koita3w an uparxei to file auto
            if (file_exists(file) == false){
               cout << "The file doesn't exist" << endl;
               exit(1);
            }
            q = 1;
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
        if (string(argv[i]) == "-sp"){ 
            if (isNumber(argv[i+1]) == 0){
                cout << "-sp must be a number" <<endl;
                exit(1);
            }
            else{
                params.servPort = atoi(argv[i+1]);
                sp = 1;
            }
        }
        if (string(argv[i]) == "-sip"){ 
            strcpy(params.servIP,argv[i+1]);
            sip = 1;
        }
        
    }
    
    //tsekarw an do8ikan ola ta orismata
    if (q != 1 || w != 1 || sp != 1 || sip != 1){ //einai oi counter twn orismatwn opote kai an oloi exoun par8ei mia fora tote 8a prepei na einai oloi 1 alliws einai la8os
        cout << "Wrong arguments" << endl << "./whoServer –q queryFile -w numThreads –sp servPort –sip servIP" <<endl;
        exit(1);
    }
}


void printArgs(void) { //kanoume print ta arguments pou dinontai apo ti grammi entolwn
    cout << "queryFile " << params.queryFile << endl ; //perissotero to kanoume gia na epivevaiwsoume oti pernane swsta ta orismata
    cout << "numThreads " << params.numThreads << endl ;
    cout << "servPort " << params.servPort << endl ;
    cout << "servIP " << params.servIP << endl ;
}

void readQueries(){
    char *query;
    int counter = 0; //metraei ta threads pou ftiaxame
    int threads_creation_times = 0; //krataei poses fores exoume dimiourgisei ta numThreads,etsi wste otan ta query pou exoun apomeinei 
    //gia diavasma einai ligotera apo to params.numThreads ,na ftia3w Threads gia osa query exoun apomeinei
    int number_of_lines = number_of_lines_in_file();
    int remaining_lines;
    //sti periptwsi pou oi grammes einai ligoteres apo ta threads
    if (number_of_lines < params.numThreads){
    	params.numThreads = number_of_lines;
    }

    ifstream file(params.queryFile);
    int error;
    if (file.is_open()){
        string line;
        while (getline(file,line)){
            //dimiourgia threads kai katastrofi tous
            //tous stelnoume ta query se ola kai perimenoume tin apantisi tous gia na ti tupwsoume
            query = new char[line.size() + 1];
            strcpy(query,line.c_str());
            if ( error = pthread_create(&threads[counter],NULL,send_receive_Query,query)){
                perror_exit((char*)"pthread create error");
            }
            counter++;
            if (counter == params.numThreads && threads_creation_times < (number_of_lines / params.numThreads)){
                counter = 0;
                for (int i = 0; i <params.numThreads; i++){
                    if (error = pthread_join(threads[i],NULL)){
                        perror_exit((char*)"pthread join error");
                    }
                }
                threads_creation_times++;
                //an exoun meinei 5 entoles enw ta threads einai 10 tote apla kanei ton ari8mo twn threads 5 kai ftiaxnei 5 threads
                remaining_lines = number_of_lines - (threads_creation_times * params.numThreads);
                if (remaining_lines < params.numThreads){
                    params.numThreads = remaining_lines;
                }

            }
            if(number_of_lines == 0 || params.numThreads == 0) break;
            if (counter == (number_of_lines % params.numThreads) && threads_creation_times == (number_of_lines / params.numThreads)){
                counter = 0;
                for (int i = 0; i < counter; i++){
                    if (error = pthread_join(threads[i],NULL)){
                        perror_exit((char*)"pthread join error");
                    }
                }
            }
        }
        file.close();
    }
}

void* send_receive_Query(void* args){

	pthread_mutex_lock(&print_mutex);
    pthread_mutex_unlock(&print_mutex);
    char* result = NULL; //edw 8a epistrafei to apotelesma tou query

    int server_fd;
    struct sockaddr_in server;
    struct sockaddr * serverptr = (struct sockaddr*) &server;
    struct hostent *rem;
    //char sname[20] = "127.0.0.1"; //pairnei to params.serverip apo ta arguments

    char* query = (char*) args;

    create_whoServer_connection(&serverptr,&rem,params.servIP,&server,&server_fd,params.servPort);

    Send_Message(server_fd,query);
    result = Receive_Message(server_fd);
    pthread_mutex_lock(&print_mutex);
    cout << result << endl;
    pthread_mutex_unlock(&print_mutex);

    //delete [] result;
    delete [] query;
    pthread_exit(NULL);

}

void create_whoServer_connection(struct sockaddr **serverptr,struct hostent **rem,char* sname,struct sockaddr_in *server, int *sock,int port){
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

void Send_Message(int fd,char *message){ 
    int size = strlen(message); //size tou message pou 9a steilei o sender
    char temp[10]; //ftiaxnw ena temp
    sprintf(temp,"%d$",size); // to temp krataei to mege8os tou message px 1024$
    write(fd,temp,9); //kanw write sto fifo to 1024$
    write(fd,message,size); //kanw write to message
}

char* Receive_Message(int fd){

    int bufferSize = BUFFERSIZE;
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
        //cout << "Disease n: "<< n << bufferSize << endl;
        if (counter + n > size){
            n = size - (counter + n);
        }
        counter += n; //au3anoume to counter n xaraktires
    }
    input_message[size] = 0; //kanoume to teleutaio xaraktira tou message 0
    return input_message;
}

void initialize_All(){

    pthread_mutex_init(&print_mutex,NULL);
    threads = new pthread_t[params.numThreads];

}

void perror_exit(char *message){
    perror(message);
    pthread_exit(NULL);
}

// ./whoClient -q queryFile.txt -w 10 -sp 9000 -sip 127.0.0.1
int main(int argc,char **argv){
	getArgs(argc, argv);
    printArgs();
    initialize_All();
    readQueries();
}