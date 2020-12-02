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

/* Global variables for temporal memory*/
#define MEM_SIZE 5
typedef struct TempMem TempMem;
struct TempMem {
	pthread_mutex_t MemMutex;
	long long int time[MEM_SIZE] = {0};
	double temp[MEM_SIZE] = {0};
	double press[MEM_SIZE] = {0};
	double hum[MEM_SIZE] = {0};
	int alarmOn:1;
        int fanOn:1;
};


TempMem MemFac1;
TempMem MemFac2;

void swip(double *array, double m){

	for (int i = 0; i < (MEM_SIZE-1); i++) {

		array[i] = array[i+1];
	}

	array[MEM_SIZE-1] = m;
}

typedef struct GUIcommands GUIcommands;
struct GUIcommands{
	int Fan1 = 0;
	int Fan2 = 0;
};

GUIcommands commands;


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
	server1.sin_addr.s_addr = inet_addr("192.168.43.245");
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
	server2.sin_addr.s_addr = inet_addr("192.168.43.245");
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
	struct data_receive message;
	char client_message;
	std::ofstream myfile;
  	char command2fac[5];
  	char buff3[2];		
	//while( (read_size = recv(sock , (void*) &message , sizeof(struct data_receive) , 0)) > 0 ) {
	
	for(;;){
		
		// Build the string to send the information about Fan and Alarms
		bzero(command2fac, sizeof(command2fac));
		sprintf(buff3, "%d", commands.Fan1);
		strcat( command2fac, buff3);
		sprintf(buff3, "%d", MemFac1.alarmOn);
	    strcat( command2fac, buff3);
	    sprintf(buff3, "%d", commands.Fan2);
	    strcat( command2fac, buff3);
	    sprintf(buff3, "%d", MemFac2.alarmOn);
		strcat( command2fac, buff3);
		write(sock, command2fac, sizeof(command2fac));
		
		// Wait for the reply
		read(sock , (void*) &message , sizeof(struct data_receive));
		/* Update the temporal memory */
		if (message.fac == 1){
			
			pthread_mutex_lock( &(MemFac1.MemMutex) );       // Protect the variable
			swip(&(MemFac1.temp[0]), message.temperature);   // Update temperature buffer
			swip(&(MemFac1.press[0]), message.preassure);    // Update pressure buffer
			swip(&(MemFac1.hum[0]), message.humidity);       // Update humidity buffer
			MemFac1.alarmOn = message.alarmOn;               // Current state of Alarm
			MemFac1.fanOn = message.fanOn;			 // Current state of Actuator
			pthread_mutex_unlock ( &(MemFac1.MemMutex));     // End of critical Section
			
			
		} else if (message.fac == 2){

			pthread_mutex_lock( &(MemFac2.MemMutex) );       // Protect the variable
			swip(&(MemFac2.temp[0]), message.temperature);   // Update temperature buffer
			swip(&(MemFac2.press[0]), message.preassure);    // Update pressure buffer
			swip(&(MemFac2.hum[0]), message.humidity);       // Update humidity buffer
			MemFac2.alarmOn = message.alarmOn;               // Current state of Alarm
			MemFac2.fanOn = message.fanOn;			 // Current state of Actuator
			pthread_mutex_unlock ( &(MemFac2.MemMutex));     // End of critical Section

		}					

		/* Build the message that will be stored in the txt file */
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

	int sock = *(int*)socket_desc;    // Socket Descriptor
	char commandsBuff[3];             // message to receive
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
		
		
		/* Do the job  */
		char buff1[200];
		char buff2[19];
		bzero(buff1, sizeof(buff1));
		
		pthread_mutex_lock( &(MemFac1.MemMutex) );       // Protect the variable    		 
		
		sprintf(buff2, "%0.2f", MemFac1.temp[MEM_SIZE-1]);
		strcat( buff1,buff2);
		strcat( buff1, ",");
		
		sprintf(buff2, "%0.2f", MemFac1.press[MEM_SIZE-1]);
		strcat( buff1,buff2);
		strcat( buff1, ",");
		
		sprintf(buff2, "%0.2f", MemFac1.hum[MEM_SIZE-1]);
		strcat( buff1,buff2);
		strcat( buff1, ",");
		
		sprintf(buff2, "%d", MemFac1.alarmOn);
		strcat( buff1,buff2);
		strcat( buff1, ",");
		
		sprintf(buff2, "%d", MemFac1.fanOn);
		strcat( buff1,buff2);
		
		pthread_mutex_unlock ( &(MemFac1.MemMutex));
				
		pthread_mutex_lock( &(MemFac2.MemMutex) );       // Protect the variable    		 
		
		strcat( buff1, ",");
		sprintf(buff2, "%0.2f", MemFac2.temp[MEM_SIZE-1]);
		strcat( buff1,buff2);
		strcat( buff1, ",");
		
		sprintf(buff2, "%0.2f", MemFac2.press[MEM_SIZE-1]);
		strcat( buff1,buff2);
		strcat( buff1, ",");
		
		sprintf(buff2, "%0.2f", MemFac2.hum[MEM_SIZE-1]);
		strcat( buff1,buff2);
		strcat( buff1, ",");
		
		sprintf(buff2, "%d", MemFac2.alarmOn);
		strcat( buff1,buff2);
		strcat( buff1, ",");
		
		sprintf(buff2, "%d", MemFac2.fanOn);
		strcat( buff1,buff2);
		strcat( buff1, ",0.0,0.0,0.0,0.0,0.0,0.0"); //predictions not ready sorry
		
		pthread_mutex_unlock ( &(MemFac2.MemMutex));
		
		// Send the data to GUI	
		write(sock, buff1, sizeof(buff1));
		
		// Receive the Commands
		recv(sock , commandsBuff , sizeof(commandsBuff) , 0);
		commands.Fan1 = (int) commandsBuff[0] - 48;
		commands.Fan2 = (int) commandsBuff[1] - 48;
		printf("Fan1 = %d\n",commands.Fan1);
		printf("Fan2 = %d\n",commands.Fan2);
		

		/* Compute next task arrival */
		add_timespec( &trigger, &trigger, &period );

		/* Wait until the end of the period */
		sem_timedwait( &timer, &trigger );

	}

	return 0;
}
