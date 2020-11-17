#include "bme280.h"
#include "majority.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>

//Raspberry 3B+ platform's default SPI channel
#define channel 0  

//Default write it to the register in one time
#define USESPISINGLEREADWRITE 0 

struct input{
  struct bme280_dev dev; 
  int index;
};

#define n_pin 3
int PINS[] = {27,28,29};
float low_t = 0, high_t =40, low_p = 500, high_p =2000, low_h = 20, high_h =80;
int priority = 5;
sem_t synchro[n_pin];
struct bme280_data comp_data[n_pin];


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
  float tem_vote, pre_vote, hum_vote;
  
  struct timespec period;                              /* Period of the task */

  period.tv_nsec = 0 * 1000 * 1000;
  period.tv_sec  = 20;
	
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
    tem_vote = merge(temperature, size, low_t, high_t, 0.5);
    pre_vote = merge(pressure,    size, low_p, high_p, 5.0);
    hum_vote = merge(humidity,    size, low_h, high_h, 1.0);
    
    printf("FINAL temperature:%0.2f*C   pressure:%0.2fhPa   humidity:%0.2f%%\r\n", tem_vote, pre_vote, hum_vote);
  
  }

  return NULL;
}


int main(int argc, char* argv[]) {

  int8_t rslt;
  struct input inputs[n_pin];
  pthread_t tid[n_pin];
  pthread_attr_t tattr[n_pin];
  struct sched_param param[n_pin];

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

}
