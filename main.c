#include "bme280.h"
#include "majority.h"
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <strings.h>

#define PORT 8080 
#define SA struct sockaddr

// Identifies the ID of the factory
#define FAC 1

// GPIO pins used for the LED and the relay
#define LED 1		// Simulate the alarm
#define RELAY 0		// Simulate the fan

//Default write it to the register in one time
#define USESPISINGLEREADWRITE 0

//Raspberry 3B+ platform's default SPI channel
#define channel 0 

// Struct used by each thread reading its sensor
struct input{
  /*! Sensor's predefined structure */
  struct bme280_dev dev; 
  /*! ID given to the thread*/
  int index;
};

// Struct used by this program to communicate data from the factory to the data management
struct send_data {
	/*! Factory identifier */
	int fac;
	/*! Time of data */
	long long int timeStamp; 
	/*! Compensated pressure */
	double pressure;
	/*! Compensated temperature */
	double temperature;
	/*! Compensated humidity */
	double humidity;
	/*! Alarm ON/OFF - simulated as a LED */
	int alarmON : 1;
	/*! Fan ON/OFF - simulated as a relay */
	int fanON : 1;
};

// Number of sersors to be read and pins used for them
#define n_pin 3
int PINS[] = {27,28,29};

// Threshold for temperature, pressure and humidity
float low_t = 0, high_t =40, low_p = 500, high_p =2000, low_h = 20, high_h =80;

// Priority given to the sensor reading task
int priority = 50;

// Array of semaphores used to check if all sensors have been read (checked by the "merge_and_send" task)
sem_t synchro[n_pin];

// Array of sensor's data structure
struct bme280_data comp_data[n_pin];

int sockfd = -1;

// The struct to send data is protected by a mutex as it is used by two independent threads
struct send_data data;
pthread_mutex_t mutex_data;


/* Sporadic task used to simulate the alarm through a blinkg LED */
void* LED_blink (void* none)
{
  printf ("Alarm activated\n") ;

  pinMode (LED, OUTPUT) ;
  for (int i = 0; i < 20; i++)
  {
    digitalWrite (LED, HIGH) ; // On
    delay (500) ; // mS
    digitalWrite (LED, LOW) ; // Off
    delay (500) ;
  }
  pthread_mutex_lock(&mutex_data);
  data.alarmON = 0;
  pthread_mutex_unlock(&mutex_data);
  
  return NULL ;
}

/* Initialize the TCP connection with the data management server */
int init_connection() 
{ 
	int sockfd; 
	struct sockaddr_in servaddr; 

	// socket create and varification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		return -1; 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr("192.168.43.243"); 
	servaddr.sin_port = htons(PORT); 

	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		return -1; 
	} 
	else
		printf("connected to the server..\n"); 

	return sockfd;
}

/* Function listening for commands from the data management server */
void* receive_command(void* none) {
  
  int read_size;
  // The data management sends commands as a string of integer values
  // As only two factories are considered the string has 4 useful chars and one '\n':
  // "factory_1_fan factory_1_alarm factory_2_fan factory_2_alarm \n"
  char command[5];
  
  // Thread to be used to let the alarm blink in case of necessity
  pthread_t t_alarm;
  pthread_attr_t tattr1;
  struct sched_param param1;
  pthread_attr_init (&tattr1);
  pthread_attr_getschedparam (&tattr1, &param1);
  param1.sched_priority = 10;
  pthread_attr_setschedparam (&tattr1, &param1);
  
  while( (read_size = recv(sockfd , (void*) &command , 5*sizeof(char) , 0)) > 0 )
  {
    pthread_mutex_lock (&mutex_data);
    if ((int) (command[1] - 48) || (int) (command[3] - 48)) {	// If the alarm is running in one of the factories and it is not
      if (data.alarmON == 0) {					// already running in the current one, activates the thread (48 is the ASCII of '0')
	data.alarmON = 1;
	pthread_create(&t_alarm, &tattr1, LED_blink, NULL);
      }
    }
    if ((int) (command[FAC * 2 - 2] - 48)) {			// Activates the fan of the command is received and the fan is not already ON
      printf("Relay ON\n");					//(FAN*2 - 2 allows to select the right position in the string by using the factory ID)
      if (!data.fanON) {
	digitalWrite (RELAY, LOW);
	data.fanON = 1;
      }
    }
    else {
      printf("Relay OFF\n");
      if (data.fanON) {
	digitalWrite (RELAY, HIGH);
	data.fanON = 0;
      }
    }
    
    pthread_mutex_unlock (&mutex_data);
  }
  return NULL;
}


void SPI_BME280_CS_High(void) {
	digitalWrite(27,1);
}


void SPI_BME280_CS_Low(void) {
	digitalWrite(27,0);
}


void user_delay_ms(uint32_t period) {
  usleep(period*1000);
}


int8_t user_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len) {

	int8_t rslt = 0;

	SPI_BME280_CS_High();
	SPI_BME280_CS_Low();

	wiringPiSPIDataRW(channel,&reg_addr,1);

	#if(USESPISINGLEREADWRITE)

    for(int i=0; i < len ; i++)	{
	  wiringPiSPIDataRW(channel,reg_data,1);
	  reg_data++;
	}
	#else
	wiringPiSPIDataRW(channel,reg_data,len);
	#endif
	
	SPI_BME280_CS_High();

	return rslt;
}


int8_t user_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len) {

	int8_t rslt = 0;

	SPI_BME280_CS_High();
	SPI_BME280_CS_Low();

	wiringPiSPIDataRW(channel,&reg_addr,1);

	#if(USESPISINGLEREADWRITE)
	for(int i = 0; i < len ; i++) {
		wiringPiSPIDataRW(channel,reg_data,1);
		reg_data++;
	}
	#else
	wiringPiSPIDataRW(channel,reg_data,len);
	#endif

	SPI_BME280_CS_High();

	return rslt;
}


void print_sensor_data(struct bme280_data *comp_data) {
#ifdef BME280_FLOAT_ENABLE
	printf("temperature:%0.2f*C   pressure:%0.2fhPa   humidity:%0.2f%%\r\n",comp_data->temperature, comp_data->pressure/100, comp_data->humidity);
#else
	printf("temperature:%ld*C   pressure:%ldhPa   humidity:%ld%%\r\n",comp_data->temperature, comp_data->pressure/100, comp_data->humidity);
#endif
}


/* Funtion used to add two time structure - useful for period tasks */
void add_timespec (struct timespec *s, const struct timespec *t1, const struct timespec *t2) {
  s->tv_sec  = t1->tv_sec  + t2->tv_sec;
  s->tv_nsec = t1->tv_nsec + t2->tv_nsec;
  s->tv_sec += s->tv_nsec/1000000000;
  s->tv_nsec %= 1000000000;
}

/* Periodic task reading sensor data */
void* stream_sensor_data_normal_mode(void* inputs) {

  struct input* in = (struct input*) inputs;
  int8_t rslt;
  uint8_t settings_sel;

  /* Recommended mode of operation: Indoor navigation */
  in->dev.settings.osr_h = BME280_OVERSAMPLING_1X;
  in->dev.settings.osr_p = BME280_OVERSAMPLING_16X;
  in->dev.settings.osr_t = BME280_OVERSAMPLING_2X;
  in->dev.settings.filter = BME280_FILTER_COEFF_16;
  in->dev.settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

  settings_sel = BME280_OSR_PRESS_SEL;
  settings_sel |= BME280_OSR_TEMP_SEL;
  settings_sel |= BME280_OSR_HUM_SEL;
  settings_sel |= BME280_STANDBY_SEL;
  settings_sel |= BME280_FILTER_SEL;

  rslt = bme280_set_sensor_settings(settings_sel, &(in->dev));
  rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, &(in->dev));

  struct timespec trigger;                      /* Stores next dispatch time */
  struct timespec period;                              /* Period of the task */
  sem_t timer;

  struct bme280_data temp_data;
  int status;

  // Defines the period of the current task
  period.tv_nsec = 0 * 1000 * 1000;
  period.tv_sec  = 2;
  /* Not shared among processes and already allocated semaphore */
  sem_init(&timer, 0, 0);
  clock_gettime(CLOCK_REALTIME, &trigger);  
  
  while (1) {
    
    sem_getvalue(&synchro[in->index],&status);
    
    if(!status) {
      rslt = bme280_get_sensor_data(BME280_ALL, comp_data + in->index, &(in->dev));
      //print_sensor_data(comp_data + in->index);
      
      sem_post(&synchro[in->index]);	// Semaphore increased to communicate the reading of a specific sensor
    }
    else {
      rslt = bme280_get_sensor_data(BME280_ALL, &temp_data, &(in->dev));    // If the sensor was already read, the data is stored in another struct,
									    // so that the "merge_and_send" would read data of the same timestamp
    }

    add_timespec(&trigger, &trigger, &period);
    
    /* Wait until the next period for activation */
    sem_timedwait(&timer, &trigger);

  }

  return NULL;
}

/* Task waiting for sensor data to reach a consensus and send information to the server */
void* merge_and_send(void* no_input) {
  int status[n_pin];
  int size;
  float temperature[n_pin], pressure[n_pin], humidity[n_pin];
  
  pthread_t t_alarm;
  pthread_attr_t tattr1;
  struct sched_param param1;
  pthread_attr_init (&tattr1);
  pthread_attr_getschedparam (&tattr1, &param1);
  param1.sched_priority = 10;
  pthread_attr_setschedparam (&tattr1, &param1);
  
  struct timespec timestamp;
  
  pthread_mutex_init(&mutex_data, NULL);
	
  while(1) {
    for(int i=0; i<n_pin; i++) {
      status[i] = sem_wait(&synchro[i]);
      /*status[i] = sem_timedwait (&synchro[i], &period);
      if(status[i]) {
	printf("Sensor in PIN %d unreacheable, alarm sent\n", PINS[i]);
	// SEND ALARM
      }*/
    }
    
    size = 0;
    for(int i=0; i<n_pin; i++) {			// Read sensors' data, no need for mutex as it is not modified until this thread ends its cycle
      if(!status[i]) {
        temperature[i] = comp_data[i].temperature;
	pressure[i] = comp_data[i].pressure/100;
        humidity[i] = comp_data[i].humidity;
	size ++;
      }
    }
    pthread_mutex_lock (&mutex_data);
    data.fac = FAC;
    data.temperature = merge(temperature, size, low_t, high_t, 0.5);	// Function called to reach a consensus among the different values measured
    data.pressure = merge(pressure,    size, low_p, high_p, 5.0);
    data.humidity = merge(humidity,    size, low_h, high_h, 1.0);
    
    clock_gettime(CLOCK_REALTIME, &timestamp); 
    
    data.timeStamp = timestamp.tv_sec;
    
    printf("FINAL temperature:%f*C   pressure:%fhPa   humidity:%f%% \n  alarm: %d    fan: %d\n", data.temperature, data.pressure, data.humidity, data.alarmON, data.fanON);
    
    write(sockfd, &data, sizeof(struct send_data));
    
    if (data.alarmON != 1 && (data.temperature < low_t || data.temperature > high_t || data.pressure < low_p || data.pressure > high_p || data.humidity < low_h || data.humidity > high_h)) {
      data.alarmON = 1;
      pthread_create(&t_alarm, &tattr1, LED_blink, NULL);	// If the alarm is not already on and the consensus of measurement is out of the thresholds, the alarm is triggered
    }
    
    pthread_mutex_unlock (&mutex_data);
  
  }

  return NULL;
}


int main(int argc, char* argv[]) {

  int8_t rslt;
  /* Define the variable for the sensors reading threads */
  struct input inputs[n_pin];
  pthread_t tid[n_pin];
  pthread_attr_t tattr[n_pin];
  struct sched_param param[n_pin];
  
  
  sockfd = init_connection();
  if(sockfd == -1)
    return -1;

  if(wiringPiSetup() < 0)
    return 1;
  
  
  for(int i=0; i<n_pin; i++) {
    pinMode (PINS[i],OUTPUT) ;
    
    pthread_attr_init (&tattr[i]);
    /* stores in param the current setting for scheduling */
    pthread_attr_getschedparam (&tattr[i], &param[i]);
    /* change only the priority */
    param[i].sched_priority = priority;
    /* select the new scheduling setting */
    pthread_attr_setschedparam (&tattr[i], &param[i]);
  }
  

  SPI_BME280_CS_Low();  // once pull down means use SPI Interface
  
  wiringPiSPISetup(channel,2000000);

  for(int i=0; i<n_pin; i++) {
    inputs[i].dev.dev_id = 0;
    inputs[i].dev.intf = BME280_SPI_INTF;
    inputs[i].dev.read = user_spi_read;
    inputs[i].dev.write = user_spi_write;
    inputs[i].dev.delay_ms = user_delay_ms;
    inputs[i].index = i;

    rslt = bme280_init(&inputs[i].dev);
    printf("\r\n BME280 Init Result of %d is:%d \r\n", i, rslt);

    /* start the new thread */
    
    pthread_create(&tid[i], &tattr[i], stream_sensor_data_normal_mode,(void *) (inputs + i));
  }

  pthread_t T3;
  pthread_attr_t tattr1;
  struct sched_param param1;
  pthread_attr_init (&tattr1);
  pthread_attr_getschedparam (&tattr1, &param1);
  param1.sched_priority = 60;
  pthread_attr_setschedparam (&tattr1, &param1);
  pthread_create(&T3, &tattr1, merge_and_send, NULL);
  
  pthread_t T4;
  pthread_attr_t tattr2;
  struct sched_param param2;
  pthread_attr_init (&tattr2);
  pthread_attr_getschedparam (&tattr2, &param2);
  param1.sched_priority = 60;
  pthread_attr_setschedparam (&tattr2, &param2);
  pthread_create(&T4, &tattr2, receive_command, NULL);

  pthread_join(tid[0], NULL);
  
  // close the socket 
  close(sockfd); 

}
