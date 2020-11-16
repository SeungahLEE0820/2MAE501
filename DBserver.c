/* Data Base server */
/* compile: gcc -Wall -o DBserver DBserver.c */
/* execute: ./DBserver */
/* Compatible with clientDB and the executble in IoTsensor in raspberry*/

#include <stdio.h>
#include <netdb.h> 
#include <unistd.h>
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>


#define MAX 80 

#define PORT 8080 
#define SA struct sockaddr 


// Function designed for chat between client and server. 
void func(int sockfd) 
{ 
	char buff[MAX]; 
	//int n; 

	FILE *DBp; 
        DBp = fopen("data1.txt", "a");

        /* check file pointer */
        if(DBp == NULL)
        {
		printf("Unable to open file.\n");
		exit(EXIT_FAILURE);
        }

	for(;;){
	
		// Read the message from client and copy it in buffer
		bzero(buff, MAX);
		read(sockfd, buff, sizeof(buff)); 
		
		if (strncmp("exit", buff, 4) == 0) { 
			printf("Server Exit...\n"); 
			break; 
		} else {
			/* Write data to file */
			fputs(buff, DBp);

		}
	}


	/* Close file to save file data */
	fclose(DBp);
} 


// Driver function 
int main() {
 
	int sockfd, connfd; 
	socklen_t len;
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 

		printf("socket creation failed...\n"); 
		exit(0); 

	} else {

		printf("Socket successfully created..\n"); 
	}

	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; // TCP
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
        //servaddr.sin_addr.s_addr = inet_addr("10.0.2.15");
        servaddr.sin_addr.s_addr = inet_addr("192.168.31.97");
	servaddr.sin_port = htons(PORT); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
 
		printf("socket bind failed...\n"); 
		exit(0); 

	} else {

		printf("Socket successfully binded..\n"); 
	}

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 

		printf("Listen failed...\n"); 
		exit(0); 

	} else {

		printf("Server listening..\n"); 
	}

	len = sizeof(cli); 

	// Accept the data packet from client and verification 
	connfd = accept(sockfd, (SA*)&cli, &len); 

	if (connfd < 0) { 

		printf("server acccept failed...\n"); 
		exit(0); 
	} else {

		printf("server acccept the client...\n"); 
	}

	// Function for chatting between client and server 
	func(connfd);

	// After chatting close the socket 
	close(sockfd); 
} 

