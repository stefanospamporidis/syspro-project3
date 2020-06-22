#include "List.hpp"

ListNode::ListNode(char *worker_port){

	//char *countries_port = "0666"
	this->worker_port = atoi(worker_port);
	next = NULL;

}

void List::AddNode(char *countries_port){ //pros8iki antikeimenou sti lista

	ListNode *newNode = new ListNode(countries_port);
	if (IsEmpty()){
		head = newNode;
		tail = newNode;
		counter = 1;
		return;
	}
	tail->next=newNode;
	tail = newNode;
	counter++;
	return;

}


List::~List(){ //destructor
	ListNode *temp;
	while(head){
		temp = head;
		head = head->next;
		delete temp;
	}
}