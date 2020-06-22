OBJS = whoServer.o Circle_Buffer.o List.o
SOURCE = whoServer.cpp Circle_Buffer.cpp List.cpp
HEADER = whoServer.hpp Circle_Buffer.hpp List.hpp
OUT = whoServer
CC = g++
FLAGS = -g -c 


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) -lpthread

whoServer.o: whoServer.cpp
	$(CC) $(FLAGS) whoServer.cpp

Circle_Buffer.o: Circle_Buffer.cpp
	$(CC) $(FLAGS) Circle_Buffer.cpp

List.o: List.cpp
	$(CC) $(FLAGS) List.cpp

clean:
	rm -f $(OBJS) $(OUT)