// gcc -Wall -o DummyClient2 DummyClient2.cc

#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <string.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX 200 
#define PORT 8080 
#define SA struct sockaddr 

struct data_receive{
	int fac;
	long long int timestamp;
	double preassure;
	double temperature;
	double humidity;
        int alarmOn:1;
        int fanOn:1;
};


void func(int sockfd) { 
	
	struct data_receive data;
	int i=0;
	
	data.fac=2;
	data.timestamp = 12345;
	data.temperature = 19.0;
	data.preassure = 1001.34;
	data.humidity = 43.5;

	for (;;) { 
	
		write(sockfd, &data, sizeof(struct data_receive));
		data.timestamp = 12345 + i;
		data.temperature = 19.0 + i;
		data.preassure = 1001.34 +i;
		data.humidity = 43.5 + i;
		i++;
		sleep(1);
		
	
	} 
} 

int main() 
{ 
	int sockfd; 
	struct sockaddr_in servaddr; 

	// socket create and varification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr("10.0.2.15"); 
	servaddr.sin_port = htons(PORT); 

	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("connected to the server..\n"); 

	// function for chat 
	func(sockfd); 

	// close the socket 
	close(sockfd); 
} 

