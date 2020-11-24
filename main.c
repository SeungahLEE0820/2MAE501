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

#define FAC 1

#define LED 1

//Default write it to the register in one time
#define USESPISINGLEREADWRITE 0

//Raspberry 3B+ platform's default SPI channel
#define channel 0 

struct input{
  struct bme280_dev dev; 
  int index;
};

struct send_data {
	/*! Identifier */
	int fac;
	/*! Time of data */
	long long int timeStamp; 
	/*! Compensated pressure */
	double pressure;
	/*! Compensated temperature */
	double temperature;
	/*! Compensated humidity */
	double humidity;
	/*! Alarm ON/OFF */
	int alarmON : 1;
	/*! Fan ON/OFF */
	int fanON : 1;
};

#define n_pin 2 //3
int PINS[] = {27,28};//,29};
float low_t = 0, high_t =40, low_p = 500, high_p =2000, low_h = 20, high_h =80;
int priority = 5;
sem_t synchro[n_pin];
struct bme280_data comp_data[n_pin];
int sockfd = -1;
struct send_data data;
pthread_mutex_t mutex_data;

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
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.220"); 
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


void add_timespec (struct timespec *s, const struct timespec *t1, const struct timespec *t2) {
  s->tv_sec  = t1->tv_sec  + t2->tv_sec;
  s->tv_nsec = t1->tv_nsec + t2->tv_nsec;
  s->tv_sec += s->tv_nsec/1000000000;
  s->tv_nsec %= 1000000000;
}


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

  period.tv_nsec = 0 * 1000 * 1000;
  period.tv_sec  = 1;
  /* Not shared among processes and already allocated semaphore */
  sem_init(&timer, 0, 0);
  clock_gettime(CLOCK_REALTIME, &trigger);  
  
  while (1) {
    
    sem_getvalue(&synchro[in->index],&status);
    
    if(!status) {
      rslt = bme280_get_sensor_data(BME280_ALL, comp_data + in->index, &(in->dev));
      // CHECK FOR ALARM
      print_sensor_data(comp_data + in->index);
      
      sem_post(&synchro[in->index]);
    }
    else {
      rslt = bme280_get_sensor_data(BME280_ALL, &temp_data, &(in->dev));
      // CHECK FOR ALARM
    }

    add_timespec(&trigger, &trigger, &period);
    
    /* Wait until the next period for activation */
    sem_timedwait(&timer, &trigger);

  }

  return NULL;
}


void* merge_and_send(void* no_input) {
  int status[n_pin];
  int size;
  float temperature[n_pin], pressure[n_pin], humidity[n_pin];
  
  pthread_t t_alarm;
  pthread_attr_t tattr1;
  struct sched_param param1;
  pthread_attr_init (&tattr1);
  pthread_attr_getschedparam (&tattr1, &param1);
  param1.sched_priority = 50;
  pthread_attr_setschedparam (&tattr1, &param1);
  
  struct timespec timestamp;                              /* Period of the task */
  
  pthread_mutex_init(&mutex_data, NULL);
	
  while(1) {
    for(int i=0; i<n_pin; i++) {
      //status[i] = sem_timedwait (&synchro[i], &period);
      status[i] = sem_wait(&synchro[i]);
      if(status[i]) {
	printf("Sensor in PIN %d unreacheable, alarm sent\n", PINS[i]);
	// SEND ALARM
      }
    }
    
    size = 0;
    for(int i=0; i<n_pin; i++) {
      if(!status[i]) {
        temperature[i] = comp_data[i].temperature;
	pressure[i] = comp_data[i].pressure/100;
        humidity[i] = comp_data[i].humidity;
	size ++;
      }
    }
    pthread_mutex_lock (&mutex_data);
    data.fac = FAC;
    data.temperature = merge(temperature, size, low_t, high_t, 0.5);
    data.pressure = merge(pressure,    size, low_p, high_p, 5.0);
    data.humidity = merge(humidity,    size, low_h, high_h, 1.0);
    
    clock_gettime(CLOCK_REALTIME, &timestamp); 
    
    data.timeStamp = timestamp.tv_nsec * 1000 * 1000 + timestamp.tv_sec * 1000;
    
    printf("FINAL temperature:%f*C   pressure:%fhPa   humidity:%f%%\r\n", data.temperature, data.pressure, data.humidity);
    
    write(sockfd, &data, sizeof(struct send_data));
    
    if (data.temperature < low_t || data.temperature > high_t || data.pressure < low_p || data.pressure > high_p || data.humidity < low_h || data.humidity > high_h)
      pthread_create(&t_alarm, &tattr1, LED_blink, NULL);
    
    pthread_mutex_unlock (&mutex_data);
  
  }

  return NULL;
}


int main(int argc, char* argv[]) {

  int8_t rslt;
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
  

  SPI_BME280_CS_Low();  //once pull down means use SPI Interface
  
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
  param1.sched_priority = 20;
  pthread_attr_setschedparam (&tattr1, &param1);
  pthread_create(&T3, &tattr1, merge_and_send, NULL);

  pthread_join(tid[0], NULL);
  
  // close the socket 
  close(sockfd); 

}
