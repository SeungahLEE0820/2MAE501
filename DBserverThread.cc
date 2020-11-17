/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc server.c -lpthread -o server
*/
 
#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>   
#include<pthread.h> 

/* Configure mutex */
pthread_mutex_t mutex;

 
//the thread function
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    // server.sin_addr.s_addr = INADDR_ANY;
    server.sin_addr.s_addr = inet_addr("10.0.2.15");
    server.sin_port = htons( 8080 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    pthread_t thread_id;
    pthread_mutex_init ( &mutex , NULL);
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
	char *ip = inet_ntoa(client.sin_addr);
        int port= htons (client.sin_port);

        printf("Connection accepted from %s %d\n", ip, port);

         
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }                 
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    const char *message = "Greetings! I am your connection handler\n";
    char client_message[2000];
     
    //Send some messages to the client
    write(sock , message , strlen(message));

  
    //int n; 

    FILE *DBp; 

      
    DBp = fopen("data1.txt", "a");

    /* check file pointer */
    if(DBp == NULL)
    {
	printf("Unable to open file.\n");
	exit(EXIT_FAILURE);
    }
 
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
	
        //end of string marker
	client_message[read_size] = '\0';

	if (strncmp("exit", client_message, 4) == 0) { 
		printf("Server Exit...\n"); 
		read_size = 0;
		break; 
	} else {
		/* Write data to file */
		pthread_mutex_lock(&mutex);	/* Lock mutex  */	
		fputs(client_message, DBp);
		pthread_mutex_unlock ( &mutex );

	}		
	//clear the message buffer
	memset(client_message, 0, 2000);
    }
    
    fclose(DBp);

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
} 
