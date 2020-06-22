#include "MainUtils.hpp"

Parameters params ;

int MainUtils::isNumber(char* number){ //tsekarw an dinw ari8mo apo to pliktrologio
    int i;
    for (i=0;i<strlen(number);i++){
        if (number[i]<'0' || number[i]>'9') //koitaw an ka8e xaraktiras den einai anamesa stous xaraktires 0 kai 9
            return 0; // an den einai gurnaw 0
    }
    return 1; //alliws gurnaw 1
}   

inline bool MainUtils::file_exists (string& name) { //checkarei an to arxeio pou edwsa uparxei an uparxei gurnaei true
    if (FILE *file = fopen(name.c_str(), "r")) { //alliws false 
        fclose(file);
        return true;
    } else {
        return false;
    }   
} 

void MainUtils::getArgs(int argc,char **argv) {
    
    params.bufferSize = atoi(argv[1]);
    strcpy(params.fifo_read,argv[2]);
    strcpy(params.fifo_write,argv[3]);
    params.diseaseHashtableNumOfEntries = 101;
    params.countryHashtableNumOfEntries = 101;
    params.bucketSize = 50;

}

void MainUtils::printArgs(void) { //kanoume print ta arguments pou dinontai apo ti grammi entolwn
    cout << "Patients record file " << params.patientRecordsFile << endl ; //perissotero to kanoume gia na epivevaiwsoume oti pernane swsta ta orismata
    cout << "h1 " << params.diseaseHashtableNumOfEntries << endl ;
    cout << "h2 " << params.countryHashtableNumOfEntries << endl ;
    cout << "b " << params.bucketSize << endl ;
}

string MainUtils::getCountry(string token){

    string path = token;
    size_t botDirPos = path.find_last_of("/");
    // get directory
    string dir = path.substr(0, botDirPos);
    // get country
    string country = path.substr(botDirPos + 1, path.length());
    return country;

}

void MainUtils::getDirectories_of_Cases(){

    char *token,buff[1024],delims[] = "$"; //to token kanei parse to folder me tis xwres kai krataei to directory tous
    //to buff krataei ta arxeia me tis imerominies diladi kati san /input_dir/Germany/15-02-2014
    char * countries_directory = Receive_Message(params.read_filedesc);
    string country , date ;
    string recordID,patientFirstName, patientLastName, diseaseID, entry_exit_key, entry , exit, age;
    
    token = strtok( countries_directory, delims ); //to prwto token pou dinw sto message einai to workerPort tou
    params.workerPort = atoi(token);
    token = strtok( NULL, delims ); //to deutero token pou dinw sto message einai to serverIP
    strcpy(params.serverIP,token);
    token = strtok( NULL, delims ); //to trito token pou dinw sto message einai to serverPort
    params.serverPort = atoi(token);
    token = strtok( NULL, delims );
    while( token != NULL  ) 
    {
        ifstream fin;
        country = getCountry(token);
        //pairnw to directory twn arxeiwn me ta dates 
        DIR *dir_ptr;
        struct dirent *dirent_ptr;
        struct stat filestat;
        if ((dir_ptr = opendir(token)) != NULL){
            /* print all the files and directories within directory */
            while ((dirent_ptr = readdir(dir_ptr)) != NULL){
                if (strcmp(dirent_ptr->d_name,".") != 0 && strcmp(dirent_ptr->d_name,"..") != 0){
                    sprintf(buff,"%s/%s",token,dirent_ptr->d_name);
                    //cout << buff << " " << country << " " << dirent_ptr->d_name << endl;
                    date = string(dirent_ptr->d_name);

                    // If the file is a directory (or is in some way invalid) we'll skip it 
                    if (stat( buff, &filestat )) continue;
                    if (S_ISDIR( filestat.st_mode ))         continue;

                    fin.open( buff );
                    while (fin >> recordID >> entry_exit_key >> patientFirstName >> patientLastName >> diseaseID >> age){
                        //cout << recordID << " " << entry_exit_key << " " << patientFirstName << " " << patientLastName << " " << diseaseID << " " << age << endl;
                        add_Cases(recordID,entry_exit_key,patientFirstName,patientLastName,country,date,diseaseID,age);
                    }
                    fin.close();
                    
                }
            }
            closedir (dir_ptr);
        }
        else {
            /* could not open directory */
            perror ("");
            return;
        }
        token = strtok( NULL, delims ); 
    }

}

void MainUtils::add_Cases(string recordID,string entry_exit_key,string patientFirstName,string patientLastName,string country,string date,string diseaseID,string age)
{
    string entry,exit;

    //cout << country << " " << diseaseID << endl;
    if (entry_exit_key == "ENTER"){
        entry = date;
        exit = "-";
        // successfully extracted one line, data is in recordID, patientFirstName, patientLastName, diseaseID, country, entry, exit.
        if (caselist->AddNode(recordID,patientFirstName,patientLastName,diseaseID,country,entry,exit,age) == 2){
            cout << "H periptwsi tou as8eni uparxei idi sto arxeio, diakoptetai i diadikasia" << endl;
            return;
        }

        diseaseHashtable->insert_item(diseaseID,caselist->tail->patient->entryDate.date_size,caselist->tail);
        countryHashtable->insert_item(country,caselist->tail->patient->entryDate.date_size,caselist->tail);
    }
    if (entry_exit_key == "EXIT"){
        exit = date;
        caselist->UpdateNode(recordID,exit);
    }

}

void MainUtils::parseCommands(int fd,char *buffer){

    char *token = strtok(buffer, " ");
    if (token == NULL) return; //gia tin periptwsi pou o xristis exei patisei mono enter
    if (!strcmp(token,"/diseaseFrequency"))
    {
        char *results = new char[1024];
        token = strtok(NULL," \t\n");

        if (token ==NULL)
        {
            Send_Message(fd,(char*)"Error");
            //cout<<"Error"<<endl;
            return;
        }

        int counter = 0;
        char virus[80],xwra[80],Date1[80],Date2[80];
        string virusName,country,entryDate1,entryDate2;
        
        while (token != NULL){
            if (counter == 0)
                strcpy(virus, token);
            else if (counter == 1)
                strcpy(Date1, token);
            else if (counter == 2)
                strcpy(Date2, token);
            else if (counter == 3)
                strcpy(xwra, token);
            counter += 1;
            token = strtok(NULL, " \t\n");
        }
        if (counter == 3){
            virusName = string(virus);
            entryDate1 = string(Date1); //den uparxei to country ara i timi pou apo8ikeuei i xwra einai to Date1
            entryDate2 = string(Date2); //kai to Date1 einai to Date2
            //cout<<"Kaloume tin sunartisi recordPatientExit me tis times"<< virusName << entryDate1 << entryDate2 <<endl;
            Date d1(Date1),d2(Date2);
            strcpy(results,diseaseHashtable->diseaseFrequency(virusName,d1.date_size,d2.date_size));
            Send_Message(fd,results);
        }
        else if (counter == 4){
            virusName = string(virus);
            entryDate1 = string(Date1);
            entryDate2 = string(Date2);
            country = string(xwra);
            //cout<<"Kaloume tin sunartisi recordPatientExit me tis times"<< virusName << country  << entryDate1 << entryDate2 <<endl;
            Date d1(Date1),d2(Date2);
            strcpy(results,diseaseHashtable->diseaseFrequency(virusName,d1.date_size,d2.date_size,country));
            Send_Message(fd,results);
        }
        else{
            Send_Message(fd,(char*)"Error");
            //cout<<"Error"<<endl;
            return;
        }
        delete []results;
    }
    else if (!strcmp(token,"/listCountries"))
    {
        char * results = new char[1024];
        strcpy(results,countryHashtable->listCountries());
        Send_Message(fd,results);
        delete []results;
    }
    else if (!strcmp(token,"/topk-AgeRanges"))
    {
        char *results = new char[1024];
        token = strtok(NULL," \t\n");

        if (token ==NULL)
        {
            //cout<<"Error"<<endl;
            Send_Message(fd,(char*)"Error");
            return;
        }

        int k,counter = 0;
        char key[20],xwra[80],as8eneia[80],Date1[80],Date2[80];
        string country,disease;
        
        while (token != NULL){
            if (counter == 0)
                strcpy(key, token);
            else if (counter == 1)
                strcpy(xwra, token);
            else if (counter == 2)
                strcpy(as8eneia, token);
            else if (counter == 3)
                strcpy(Date1, token);
            else if (counter == 4)
                strcpy(Date2, token);
            counter += 1;
            token = strtok(NULL, " \t\n");
        }
        if (counter == 5){
            if (isNumber(key) == 0){
                Send_Message(fd,(char*)"Error");
                //cout<<"Error"<<endl;
                return;
            }
            k = atoi(key);
            country = string(xwra);
            disease = string(as8eneia);
             //vgazei ta apotelesmata anamesa stis imerominies
            Date d1(Date1),d2(Date2);
            strcpy(results,countryHashtable->topkAgeRanges(k,country,disease,d1.date_size,d2.date_size));
            Send_Message(fd,results);
        }
        else{
            //cout<<"Error"<<endl;
            Send_Message(fd,(char*)"Error");
            return;
        }
        delete []results;
    }
    else if (!strcmp(token,"/searchPatientRecord"))
    {
        char * results = new char[1024];
        token = strtok(NULL," \t\n");

        if (token ==NULL)
        {
            Send_Message(fd,(char*)"Error");
            //cout<<"Error"<<endl;
            return;
        }
    
        char id[80];

        strcpy(id, token);
        int recordID = atoi(id);
        strcpy(results,caselist->searchPatientRecord(recordID));
        Send_Message(fd,results);
        delete []results;
    }
    else if (!strcmp(token,"/numPatientAdmissions"))
    {
        char * results = new char[1024];
        token = strtok(NULL," \t\n");

        if (token ==NULL)
        {
            Send_Message(fd,(char*)"Error");
            //cout<<"Error"<<endl;
            return;
        }

        int counter = 0;
        char virus[80],xwra[80],Date1[80],Date2[80];
        string virusName,country,entryDate1,entryDate2;
        
        while (token != NULL){
            if (counter == 0)
                strcpy(virus, token);
            else if (counter == 1)
                strcpy(Date1, token);
            else if (counter == 2)
                strcpy(Date2, token);
            else if (counter == 3)
                strcpy(xwra, token);
            counter += 1;
            token = strtok(NULL, " \t\n");
        }
        if (counter == 3){
            virusName = string(virus);
            entryDate1 = string(Date1); //den uparxei to country ara i timi pou apo8ikeuei i xwra einai to Date1
            entryDate2 = string(Date2); //kai to Date1 einai to Date2
            //cout<<"Kaloume tin sunartisi recordPatientExit me tis times"<< virusName << entryDate1 << entryDate2 <<endl;
            Date d1(Date1),d2(Date2);
            strcpy(results,countryHashtable->numPatientAdmissions(virusName,d1.date_size,d2.date_size));
            Send_Message(fd,results);

        }
        else if (counter == 4){
            virusName = string(virus);
            entryDate1 = string(Date1);
            entryDate2 = string(Date2);
            country = string(xwra);
            //cout<<"Kaloume tin sunartisi recordPatientExit me tis times"<< virusName << country  << entryDate1 << entryDate2 <<endl;
            Date d1(Date1),d2(Date2);
            strcpy(results,countryHashtable->numPatientAdmissions(virusName,d1.date_size,d2.date_size,country));
            Send_Message(fd,results);
        }
        else{
            Send_Message(fd,(char*)"Error");
            //cout<<"Error"<<endl;
            return;
        }
        delete []results;
    }
    else if (!strcmp(token,"/numPatientDischarges"))
    {
        char * results = new char[1024];
        token = strtok(NULL," \t\n");

        if (token ==NULL)
        {
            Send_Message(fd,(char*)"Error");
            //cout<<"Error"<<endl;
            return;
        }

        int counter = 0;
        char virus[80],xwra[80],Date1[80],Date2[80];
        string virusName,country,entryDate1,entryDate2;
        
        while (token != NULL){
            if (counter == 0)
                strcpy(virus, token);
            else if (counter == 1)
                strcpy(Date1, token);
            else if (counter == 2)
                strcpy(Date2, token);
            else if (counter == 3)
                strcpy(xwra, token);
            counter += 1;
            token = strtok(NULL, " \t\n");
        }
        if (counter == 3){
            virusName = string(virus);
            entryDate1 = string(Date1); //den uparxei to country ara i timi pou apo8ikeuei i xwra einai to Date1
            entryDate2 = string(Date2); //kai to Date1 einai to Date2
            //cout<<"Kaloume tin sunartisi recordPatientExit me tis times"<< virusName << entryDate1 << entryDate2 <<endl;
            Date d1(Date1),d2(Date2);
            strcpy(results,countryHashtable->numPatientDischarges(virusName,d1.date_size,d2.date_size));
            Send_Message(fd,results);
        }
        else if (counter == 4){
            virusName = string(virus);
            entryDate1 = string(Date1);
            entryDate2 = string(Date2);
            country = string(xwra);
            //cout<<"Kaloume tin sunartisi recordPatientExit me tis times"<< virusName << country  << entryDate1 << entryDate2 <<endl;
            Date d1(Date1),d2(Date2);
            strcpy(results,countryHashtable->numPatientDischarges(virusName,d1.date_size,d2.date_size,country));
            Send_Message(fd,results);
        }
        else{
            Send_Message(fd,results);
            //cout<<"Error"<<endl;
            return;
        }
        delete []results;
    }
    else{
        Send_Message(fd,(char*)"Error");
        //cout<<"Error\n"<<endl;
        return;
    }

}

void MainUtils::Create_Open_fifos(){

    struct stat Stat;
    if (stat(params.fifo_read,&Stat) == -1){
        if (mkfifo(params.fifo_read,0666) < 0){
            printf("fifo has failed \n");
            exit(1);
        }
    }
    params.read_filedesc = open(params.fifo_read,O_RDONLY);
    //params.write_filedesc = open(params.fifo_write,O_WRONLY);
    //cout << "Worker fifo_write: " << params.fifo_write << endl;
}

void MainUtils::Send_Message(int fd,char *message){
    
    int size = strlen(message); //size tou message pou 9a steilei o sender
    char temp[10]; //ftiaxnw ena temp
    sprintf(temp,"%d$",size); // to temp krataei to mege8os tou message px 1024$
    write(fd,temp,9); //kanw write sto fifo to 1024$
    write(fd,message,size); //kanw write to message

}

char * MainUtils::Receive_Message(int fd){
    
    int bufferSize = params.bufferSize;
    char *input_message; // to message
    char temp[10]; //to mege8os tou message
    char *token; //8a apomonwsei ton ari8mo apo to dollario
    read(fd,temp,9); //diavazw to temp pou einai to mege8os tou message
    token = strtok(temp,"$"); //aponwnw ton ari8mo gia na 3erw posa bytes 8a diavasw
    int size = atoi(token); //te metatrepw se ari8mo
    input_message = (char*) malloc(size * sizeof(char) + 1); //o pinakas pou 8a valw mesa to message
    if (size < bufferSize){
        bufferSize = size;
    }
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

void MainUtils::Worker_Server_communication(){

    int port = params.workerPort, fd_sock, fd_newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen = sizeof(struct sockaddr_in);
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;

    creating_passive_endpoint(&serverptr,&clientptr,&server,&client,&fd_sock,port,&clientlen);

    if (getsockname(fd_sock, (struct sockaddr *)&server, &clientlen) != 0){
        cout << "getsockname Failed" << endl; // pairnei to proto diathesimo port apo to sistima kai tos vazei sto server
    }
    port = ntohs(server.sin_port);

    send_Statistics_and_Port_to_whoServer(port); //edw stelnw sto whoServer to port mas

    char * commands;
    while (true) {
        // accept connection 
        if ((fd_newsock = accept(fd_sock, clientptr, &clientlen)) < 0) 
            perror_exit((char*)"Accept whoServer failed");
        //edw dimiourgw neo socket gia ta queries pou 8a dextw
        commands = Receive_Message(fd_newsock);
        parseCommands(fd_newsock,commands);
    }

}

void MainUtils::send_Statistics_and_Port_to_whoServer(int worker_port){

    int fd_sock,length;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;

    create_whoServer_connection(&serverptr,&rem,params.serverIP,&server,&fd_sock,params.serverPort);

    char buffer[20];
    sprintf(buffer,"%d",worker_port);

    Send_Message(fd_sock,buffer);
    close(fd_sock);

}

void MainUtils::creating_passive_endpoint(struct sockaddr**sptr,struct sockaddr**cptr, struct sockaddr_in *server, struct sockaddr_in *client,int *sock,int port,socklen_t *clientlen){

    *sptr = (struct sockaddr *)server;
    *cptr = (struct sockaddr *)client;
    if ((*sock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        perror_exit((char *)"socket");
    }
    server->sin_family = AF_INET;       // Internet domain 
    server->sin_addr.s_addr = htonl(INADDR_ANY);
    server->sin_port = htons(port);    //printf("eimai edw bind\n");  // The given port 
    if (bind(*sock, *sptr, sizeof(*server)) < 0){
        perror_exit((char*)"bind");
    }
    // Listen for connections
    if (listen(*sock, 5) < 0) perror_exit((char*)"listen");
}

void MainUtils::create_whoServer_connection(struct sockaddr **serverptr,struct hostent **rem,char* sname,struct sockaddr_in *server, int *sock,int port){
    *serverptr = (struct sockaddr*)server;
    if ((*sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit((char*)"socket");   
    if ((*rem = gethostbyname(sname)) == NULL) {    
        perror((char*)"gethostbyname"); exit(1);
    }
    server->sin_family = AF_INET;       // Internet domain 
    memcpy(&server->sin_addr, (*rem)->h_addr, (*rem)->h_length);
    server->sin_port = htons(port);         // Server port 
    // Initiate connection 
    if (connect(*sock, *serverptr, sizeof(*server)) < 0){
        perror_exit((char*)"connect");
    }
    printf("Connecting to %s port %d\n", sname, port);
} 

void MainUtils::perror_exit(char *message){
    perror(message);
    pthread_exit(NULL);
}

//na 8umi8w na tis dinw ena buffer ws orisma gia na tupwnei ta statistika
void MainUtils::Sent_results_from_Countries(){
 
    Send_Message(params.write_filedesc,(char*)"Counries Received");

}

MainUtils::MainUtils(int argc,char **argv){

    static struct sigaction sa;
    sa.sa_handler = exitFromSignal;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGKILL, &sa, NULL);

    getArgs(argc, argv);
    printArgs();
    caselist = new CasesList(); //ftiaxnw ti lista me ta cases
    countryHashtable = new Hashtable(params.countryHashtableNumOfEntries,params.bucketSize); //O hashtable opou vazoume tis upo8eseis me vasi ti xwres
    diseaseHashtable = new Hashtable(params.diseaseHashtableNumOfEntries,params.bucketSize); //O hashtable opou vazoume tis upo8eseis me vasi tis as8eneies

    Create_Open_fifos();

    getDirectories_of_Cases(); //pairnw o ka8e worker tis xwres pou exei na epe3ergas8ei

    cout << "Worker caselist counter = " << caselist->counter << " " << params.serverIP << " " << params.serverPort << endl;

    Worker_Server_communication();

    delete caselist; //diagrafw ti lista kai ta hashtable kai vgainw apo to programma
    delete countryHashtable;
    delete diseaseHashtable;
    
}

void exitFromSignal(int signo){
	
    //signal(SIGKILL, exitFromSignal);
	close(params.read_filedesc);
	close(params.write_filedesc);
	unlink(params.fifo_read);
    delete mainutils;
	exit(0);
	
}

// ./diseaseAggregator -w 2 -b 50 -s 127.0.0.1 -p 9000 -i ../input_dir
int main(int argc,char **argv){

	//signal(SIGKILL, exitFromSignal);
    mainutils = new MainUtils(argc,argv);
}