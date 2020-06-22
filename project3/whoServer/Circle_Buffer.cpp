#include "Circle_Buffer.hpp"

void Circle_Buffer::insertNode(int type_of_job,int end_point_port){

	buffer_nodes[in_position].Fill_Node(type_of_job,end_point_port);
	counter++;
	in_position = (in_position + 1) % size;

}


void Circle_Buffer::removeNode(Buffer_node & buffer_node){

	buffer_node = buffer_nodes[out_position];
	counter--;
	out_position = (out_position + 1) % size;

}