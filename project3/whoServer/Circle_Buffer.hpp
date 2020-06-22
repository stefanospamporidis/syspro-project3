#ifndef Circle_Buffer_hpp
#define Circle_Buffer_hpp

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

using namespace  std;


class Buffer_node{

public:
	int type_of_job; //to 1 einai na diavasei statistics apo worker , to 2 einai na ikanopoiisei query to whoClient
	int end_point_port;
	Buffer_node(){}
	void Fill_Node(int type_of_job,int end_point_port){
		this->type_of_job = type_of_job;
		this->end_point_port = end_point_port;
	}

};


class Circle_Buffer
{
public:
	Buffer_node* buffer_nodes;
	int size;
	int counter;
	int in_position; //i 8esi pou mporoume na valoume kati sto buffer
	int out_position; //i 8esi pou mporoume na vgaloume apo to buffer
	Circle_Buffer(int size){
		this->size = size;
		counter = 0;
		in_position = 0;
		out_position = 0;
		buffer_nodes = new Buffer_node[size];
	}
	~Circle_Buffer(){
		delete [] buffer_nodes;
	}
	bool isEmpty(){
		return counter == 0;
	}
	bool isFull(){
		return counter == size;
	}
	void insertNode(int type_of_job,int end_point_port);
	void removeNode(Buffer_node & buffer_node);
};

#endif