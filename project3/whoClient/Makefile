OBJS = whoClient.o 
SOURCE = whoClient.cpp 
HEADER = whoClient.hpp  
OUT = whoClient
CC = g++
FLAGS = -g -c 


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) -lpthread

whoClient.o: whoClient.cpp
	$(CC) $(FLAGS) whoClient.cpp

clean:
	rm -f $(OBJS) $(OUT)