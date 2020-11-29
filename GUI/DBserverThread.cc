/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc DBserverThread.cc -lpthread -o server -lstdc++

*/
 
#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>   
#include<pthread.h>
#include <semaphore.h>
#include<string> 

#include <iostream> 
#include <fstream> 

#define MAXCHAR 41
#define MAX 50 

/*typedef struct SensorData SensorData;
struct SensorData {
	
	int factory ;
	long int time ;
	float temp ;
	float press ;
	float hum ;

};
*/

struct data_receive{
	int fac;
	long long int timestamp;
	double preassure;
	double temperature;
	double humidity;
        int alarmOn:1;
        int fanOn:1;
};

#include <time.h>
void add_timespec (struct timespec *s,
                   const struct timespec *t1,
                   const struct timespec *t2)
{
  s->tv_sec  = t1->tv_sec  + t2->tv_sec;
  s->tv_nsec = t1->tv_nsec + t2->tv_nsec;
  s->tv_sec += s->tv_nsec/1000000000;
  s->tv_nsec %= 1000000000;
}



/* Configure mutex */
pthread_mutex_t mutex;


/* Global variables for socket communication */
int socket_desc1,socket_desc2, client_sock , c1, c2;
struct sockaddr_in server1, server2 , client;

 

void *fac_connection_handler(void *);
void *GUI_connection_handler(void *);

void *factoryCommunication (void *){
   
    /* Accept and incoming connection */
    puts("Waiting for incoming connections...");
    c1 = sizeof(struct sockaddr_in);
    
    /* Init multithreading IDs */
    pthread_t thread_id;
	
    /* Create a thread for each new connection */
    while( (client_sock = accept(socket_desc1, (struct sockaddr *)&client, (socklen_t*)&c1)) ){

	char *ip = inet_ntoa(client.sin_addr);
        int port= htons (client.sin_port);

        printf("Connection accepted from %s %d\n", ip, port);

        
        if( pthread_create( &thread_id , NULL ,  fac_connection_handler , (void*) &client_sock) < 0) {
            
	    perror("could not create thread");
            return 0;

        } else { 

	    puts("Handler assigned");
        }                     
    }
     
    if (client_sock < 0){

        perror("accept failed");
        return 0;
    }




}

void *GUICommunication (void *){

	/* Accept and incoming connection */
	puts("Waiting for GUI...");
	c2 = sizeof(struct sockaddr_in);

	/* Init multithreading IDs */
	pthread_t thread_id;

	/* Create a thread for each new connection */
	while( (client_sock = accept(socket_desc2, (struct sockaddr *)&client, (socklen_t*)&c2)) ){

		char *ip = inet_ntoa(client.sin_addr);
		int port= htons (client.sin_port);

		printf("Connection accepted from %s %d\n", ip, port);


		if( pthread_create( &thread_id , NULL ,  GUI_connection_handler , (void*) &client_sock) < 0) {

			perror("could not create thread");
		return 0;

		} else { 

			puts("GUI Handler assigned");
		}                     
	}

	if (client_sock < 0){
		perror("accept failed");
		return 0;
	}
}


int main(int argc , char *argv[]){

	/* INITIALISATION OF SOCKETS */
	/* Create socket with the first port*/
	socket_desc1 = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc1 == -1){
		printf("Could not create socket");
	}else{
		puts("Socket created");
	}
	
	/* Prepare the sockaddr_in structure */
	server1.sin_family = AF_INET;
	server1.sin_addr.s_addr = inet_addr("192.168.43.204");
	server1.sin_port = htons( 8080 );


	/* Bind the socket and Check possible errors */
	if( bind(socket_desc1, (struct sockaddr *)&server1 , sizeof(server1)) < 0){
		perror("First bind failed. Error"); 
		return 0;
	} else {
		puts("First bind done");
	}
	
	listen(socket_desc1 , 3);

	/* Create socket with the Second port */
	socket_desc2 = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc2 == -1){
		printf("Could not create socket");
	}else{
		puts("Socket2 created");
	}

	/* Prepare the sockaddr_in structure */
	server2.sin_family = AF_INET;
	server2.sin_addr.s_addr = inet_addr("192.168.43.204");
	server2.sin_port = htons( 9090 );

	/* Bind the socket and Check possible errors */
	if( bind(socket_desc2, (struct sockaddr *)&server2 , sizeof(server2)) < 0){
		perror("Second bind failed. Error"); 
		return 0;
	} else {
		puts("Second bind done ");
	}
	
	listen(socket_desc2 , 3);
	
	
	/* LAUNCH THE THREADS THAT MANAGE COMMUNICATIONS */
	pthread_t th_port1, th_port2;
	pthread_mutex_init ( &mutex , NULL);
	 	          
	pthread_create ( &th_port1, NULL, factoryCommunication, NULL);
	pthread_create ( &th_port2, NULL, GUICommunication, NULL);

	pthread_join (th_port1, NULL);

	return 0;
}


/*
 * This will handle connection for each client
 * */
void *fac_connection_handler(void *socket_desc) {
    
    int sock = *(int*)socket_desc;  // Socket Descriptor
    int read_size;                  // Size of incoming messages
    //char client_message[2000];      // buffer to store sensor data
    //FILE *DBp;                      // Pointer to Data Base File
    struct data_receive message;
    char client_message;
    std::ofstream myfile;
  
    /* Receive a message from client */
        //end of string marker
	//client_message[read_size] = '\0';
	
        // Entering Critical Section	
		


    while( (read_size = recv(sock , (void*) &message , sizeof(struct data_receive) , 0)) > 0 )
    {					
        char buff1[200];
	char buff2[19];
	bzero(buff1, sizeof(buff1));
	
	strcat( buff1, "Fac: \t");
	sprintf(buff2, "%d", message.fac);
	strcat( buff1,buff2);

	strcat( buff1, "\tUTC: \t");
	sprintf(buff2, "%Ld", message.timestamp);
	strcat( buff1,buff2);

	strcat( buff1, "\tT: \t");
	sprintf(buff2, "%0.2f", message.temperature);
	strcat( buff1,buff2);

	strcat( buff1, "\tP: \t");
	sprintf(buff2, "%0.2f", message.preassure);
	strcat( buff1,buff2);

	strcat( buff1, "\tH: \t");
	sprintf(buff2, "%0.2f", message.humidity);
	strcat( buff1,buff2);

	strcat( buff1, "\tA: \t");
	sprintf(buff2, "%d", message.alarmOn);
	strcat( buff1,buff2);

	strcat( buff1, "\tF: \t");
	sprintf(buff2, "%d", message.fanOn);
	strcat( buff1,buff2);
	strcat( buff1, "\n");

	// Begining of critical section
        pthread_mutex_lock( &mutex );
	myfile.open ("data1.txt", std::ofstream::in | std::ofstream::out | std::ofstream::app);
	myfile << buff1;	             
	myfile.close();
	pthread_mutex_unlock ( &mutex );
	// End of critical section
	

	//clear message buffer
	//memset(client_message, 0, 2000);
    
    }

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



void *GUI_connection_handler(void *socket_desc){

	int sock = *(int*)socket_desc;  // Socket Descriptor
	//SensorData **data = NULL;    
        char line[MAXCHAR];
	char prev_line[MAXCHAR];
	char guiMesage[200];
        FILE *DBp;                        // Pointer to Data Base File

	struct timespec trigger;          // Stores next dispatch time
  	struct timespec period;           // Period of the task */
  	sem_t timer;                      // Semaphore to make the task periodic
  	
  
	/* Define the period from task parameters argument */
	period.tv_nsec = 0; //5000 * 1000 * 1000;
        period.tv_sec  = 5;

	/* Init the semaphore */
	sem_init(&timer, 0, 0);

	/* Get the current time */
	clock_gettime (CLOCK_REALTIME, &trigger );

	/* Infinite loop to define periodic task */
	for (;;){
		

		pthread_mutex_lock( &mutex );
		DBp = fopen("data1.txt", "r");
		while (fgets(line, MAXCHAR, DBp) != NULL){
			prev_line = line;		
		}
		fclose(DBp);
		pthread_mutex_unlock ( &mutex );

		/* Do the job  */
		write(sock, guiMesage, sizeof(guiMesage));
		printf("sent\n");

		/* Compute next task arrival */
		add_timespec( &trigger, &trigger, &period );

		/* Wait until the end of the period */
		sem_timedwait( &timer, &trigger );

	}

	return 0;
}
