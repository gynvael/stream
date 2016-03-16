LDFLAGS = -lws2_32
 
all: xoxoxo
 
xoxoxo: xoxoxo.cc NetSock.cpp console.cc
	g++ xoxoxo.cc NetSock.cpp console.cc $(LDFLAGS) -o xoxoxo

test: xoxoxo
	xoxoxo 127.0.0.1 31337
