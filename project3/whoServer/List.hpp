#ifndef List_hpp
#define List_hpp

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

class ListNode{
public:
	int worker_port;
	ListNode *next;
	//pairnw ena string pou exei tis xwres tou worker kai to port tou kai ta apomonwnw gia na ta valw
	ListNode(char *worker_port);
	//na ftia3w kai ton katastrofea
	~ListNode(){}
};

class List{
public:
	ListNode *head;
	ListNode *tail;
	int counter;
	List(){
		head = NULL;
		tail = NULL;
		counter = 0;
	}
	~List();
	void AddNode(char *countries_port);
	bool IsEmpty(){
		return counter == 0;
	}
};


#endif


/*
char *countries_port = "0666|5|Greece|Belgium|Germany|Spain|Japan|"

char port[10], num_of_countries[10], country[30];
int worker_port, countries_size;
char **countries;

char *token = strtok(countries_port, "|");
//to prwto token 8a einai to port tou worker
strcpy(port,token);
worker_port = atoi(port);
//to deutero o ari8mos twn xwrwn
token = strtok( NULL, "|");
strcpy(num_of_countries,token);
countries_size = atoi(num_of_countries);
//ta upoloipa einai oi xwres pou prepei na apo8ikeusoume
countries = new char*[countries_size];
token = strtok( NULL, "|");
counter = 0;
for (int i = 0; i < countries_size; i++){
	strcpy(countries[i],token) 
	token = strtok( NULL, "|");
}



*/