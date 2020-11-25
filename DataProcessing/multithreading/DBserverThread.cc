/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc DBserverThread.cc -lpthread -o server
*/
 
#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>   
#include<pthread.h> 

#define MAXCHAR 41
#define MAX 50 

typedef struct SensorData SensorData;
struct SensorData {
	
	int factory ;
	long int time ;
	float temp ;
	float press ;
	float hum ;

};




/* Configure mutex */
pthread_mutex_t mutex;

 
//the thread function
void *connection_handler(void *);

void *factoryCommunication (void *){

    int socket_desc1, client_sock , c;
    struct sockaddr_in server1 , client;

    /* ------------------------------------------------- */
    /* Create socket with the first port*/
    socket_desc1 = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc1 == -1){
        printf("Could not create socket");
    }else{
    	puts("Socket created");
    }
    server1.sin_family = AF_INET;
    server1.sin_addr.s_addr = inet_addr("10.0.2.15");
    server1.sin_port = htons( 8080 );
     
    /* Bind the socket and Check possible errors */
    if( bind(socket_desc1, (struct sockaddr *)&server1 , sizeof(server1)) < 0){
        perror("bind failed. Error"); 
        return 0;
    } else {
    	puts("bind done");
    }
    listen(socket_desc1 , 3);
   
    /* Accept and incoming connection */
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    

    /* Init multithreading tools */
    pthread_t thread_id;
    pthread_mutex_init ( &mutex , NULL);
	

    /* Create a thread for each new connection */
    while( (client_sock = accept(socket_desc1, (struct sockaddr *)&client, (socklen_t*)&c)) ){

	char *ip = inet_ntoa(client.sin_addr);
        int port= htons (client.sin_port);

        printf("Connection accepted from %s %d\n", ip, port);

        
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0) {
            
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

        SensorData **data = NULL;
        char line[MAXCHAR];
	char value[12];
        FILE *DBp;             // Pointer to Data Base File

	
	data = (SensorData **) malloc( MAX * sizeof(SensorData *) );


        DBp = fopen("data1.txt", "r");
	if (DBp == NULL) {               // Verification

		printf("Unable to open file.\n");
		exit(EXIT_FAILURE);
	}
	
	int i = 0;
        while (fgets(line, MAXCHAR, DBp) != NULL){     // get lines until the end of the DB

		if (strlen(line) > 2) {                // Only if the line contains information

			/* Allocate memory to save data in a new structure*/
			*(data + i) = (SensorData *) malloc(sizeof(SensorData));

			printf("%s\n",line);
			//printf("%c\n",line[4]);

			(*(data + i)) -> factory = (int) line[0] - 48;

			unsigned int j = 4;
			int k = 0;
			int l = 0;
			while (j<= strlen(line)) {
				
				
				if( (line[j] != 0x020) && (j<40) ){

					value[k] = line[j];
					j++;
					k++;

				} else {
					
					if (l == 0) {

						(*(data + i)) -> time = atoi(value);

					} else if (l == 1){

						(*(data + i)) -> temp = (float) strtod(value,NULL); 

					} else if (l == 2){

						(*(data + i)) -> press = (float) strtod(value,NULL);

					} else if (l == 3){

						(*(data + i)) -> hum = (float) strtod(value,NULL);
					}


					l++;
					j += 3;
					k = 0;
					memset(value, 0, 10);
				}
			}
			i++;
			l = 0;
		}
	}
	
	for (int j=0; j<5; j++){
		printf("line %d, factory %d, time = %ld, T=%.2f, P=%.2f, h=%.2f\n", 
			j, 
			(*(data + j)) -> factory,
			(*(data + j)) -> time,
			(*(data + j)) -> temp, 
			(*(data + j)) -> press, 
			(*(data + j)) -> hum    );
	}

	 
	fclose(DBp);


	return 0;



}
 
int main(int argc , char *argv[])
{
    int socket_desc2;
    struct sockaddr_in  server2;
    
    
    /* ---------------------------------------------------- */
    /* Create socket with the Second port */
    socket_desc2 = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc2 == -1){
        printf("Could not create socket");
    }else{
    	puts("Socket2 created");
    }

     /* Prepare the sockaddr_in structure */
    server2.sin_family = AF_INET;
    server2.sin_addr.s_addr = inet_addr("10.0.2.15");
    server2.sin_port = htons( 9090 );
     
    /* Bind the socket and Check possible errors */
    if( bind(socket_desc2, (struct sockaddr *)&server2 , sizeof(server2)) < 0){
        perror("bind failed. Error"); 
        return 1;
    } else {
    	puts("bind done 2");
    }
    /* ---------------------------------------------------- */

    /* Listen */
    listen(socket_desc2 , 3);
    
    pthread_t th_port1, th_port2;

	 
                          
    pthread_create ( &th_port1, NULL, factoryCommunication, NULL);
    pthread_create ( &th_port2, NULL, GUICommunication, NULL);
    pthread_create ( &th_port2, NULL, connection_handler, &socket_desc2);

    pthread_join (th_port1, NULL);
     
    return 0;
}


/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc) {
    
    int sock = *(int*)socket_desc;  // Socket Descriptor
    int read_size;                  // Size of incoming messages
    char client_message[2000];      // buffer to store sensor data
    FILE *DBp;                      // Pointer to Data Base File
  
    /* Receive a message from client */
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 ) {

        //end of string marker
	client_message[read_size] = '\0';
	
        // Entering Critical Section	
	pthread_mutex_lock( &mutex );	
	DBp = fopen("data1.txt", "a");

	if (DBp == NULL) { // Check file pointer

		printf("Unable to open file.\n");
		exit(EXIT_FAILURE);
	}
		
	fputs(client_message, DBp);
	fclose(DBp);
	pthread_mutex_unlock ( &mutex );
	// End of critical section
	
	//clear the message buffer
	memset(client_message, 0, 2000);
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
