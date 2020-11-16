/*
  Comple：make
  Run: ./bme280
  
  This Demo is tested on Raspberry PI 3B+
  you can use I2C or SPI interface to test this Demo
  When you use I2C interface,the default Address in this demo is 0X77
  When you use SPI interface,PIN 27 define SPI_CS
*/
#include "bme280.h"
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

//Raspberry 3B+ platform's default SPI channel
#define channel 0  

//Default write it to the register in one time
#define USESPISINGLEREADWRITE 0 

//This definition you use I2C or SPI to drive the bme280
//When it is 1 means use I2C interface, When it is 0,use SPI interface
#define USEIIC 1

#include <string.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
//Raspberry 3B+ platform's default I2C device file
#define IIC_Dev  "/dev/i2c-1"
	
int fd;


#include <netdb.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>


#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

void user_delay_ms(uint32_t period)
{
  usleep(period*1000);
}

int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  write(fd, &reg_addr,1);
  read(fd, data, len);
  return 0;
}

int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  int8_t *buf;
  buf = malloc(len +1);
  buf[0] = reg_addr;
  memcpy(buf +1, data, len);
  write(fd, buf, len +1);
  free(buf);
  return 0;
}


void print_sensor_data(int sockfd, struct bme280_data *comp_data)
{
  char buff[MAX], data[6]; 
  
  bzero(buff, sizeof(buff)); 
  sprintf(data, "%0.2f", comp_data->temperature);
  strcat( buff, data);
  strcat( buff, " | ");
  sprintf(data, "%0.2f", comp_data->pressure/100);
  strcat( buff, data);
  strcat( buff, " | ");
  sprintf(data, "%0.2f", comp_data->humidity);
  strcat( buff, data);
  strcat( buff, " \n ");
  
  write(sockfd, buff, sizeof(buff)); 	
		
}


int8_t stream_sensor_data_normal_mode(int sockfd, struct bme280_dev *dev)
{
	int8_t rslt;
	uint8_t settings_sel;
	struct bme280_data comp_data;

	/* Recommended mode of operation: Indoor navigation */
	dev->settings.osr_h = BME280_OVERSAMPLING_1X;
	dev->settings.osr_p = BME280_OVERSAMPLING_16X;
	dev->settings.osr_t = BME280_OVERSAMPLING_2X;
	dev->settings.filter = BME280_FILTER_COEFF_16;
	dev->settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;
	rslt = bme280_set_sensor_settings(settings_sel, dev);
	rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, dev);

	//printf("Temperature           Pressure             Humidity\r\n");
	for (int i = 0; i< 100; i++){
		/* Delay while the sensor completes a measurement */
		dev->delay_ms(100);
		rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
		print_sensor_data(sockfd, &comp_data);
	}

        char buff[MAX] = "exit";
        write(sockfd, buff, sizeof(buff));
	
	return rslt;
}



int main(int argc, char* argv[])
{
  /*FIRST TRY TO CREATE CLIENT SOCKET */
  int sockfd; 
  struct sockaddr_in servaddr; 
  // socket create and varification 
  sockfd = socket(AF_INET, SOCK_STREAM, 0); 
  if (sockfd == -1) { 
	  printf("socket creation failed...\n"); 
	  exit(0); 
  } else {
	  printf("Socket successfully created..\n"); 
  }
  // assign IP, PORT 
  bzero(&servaddr, sizeof(servaddr)); 
  servaddr.sin_family = AF_INET; 
  servaddr.sin_addr.s_addr = inet_addr("192.168.31.97");
  servaddr.sin_port = htons(PORT); 
  // connect the client socket to server socket 
  if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
	  printf("connection with the server failed...\n"); 
	  exit(0); 
  } else {
	  printf("connected to the server..\n");
  }

  /* SECOND INIT SENSOR*/  
  struct bme280_dev dev;
  int8_t rslt = BME280_OK;

  if ((fd = open(IIC_Dev, O_RDWR)) < 0) {
    printf("Failed to open the i2c bus %s", argv[1]);
    exit(1);
  }
  if (ioctl(fd, I2C_SLAVE, 0x77) < 0) {
    printf("Failed to acquire bus access and/or talk to slave.\n");
    exit(1);
  }
  //dev.dev_id = BME280_I2C_ADDR_PRIM;//0x76
  dev.dev_id = BME280_I2C_ADDR_SEC; //0x77
  dev.intf = BME280_I2C_INTF;
  dev.read = user_i2c_read;
  dev.write = user_i2c_write;
  dev.delay_ms = user_delay_ms;
  rslt = bme280_init(&dev);
  printf("\r\n BME280 Init Result is:%d \r\n",rslt);
  
  /* START READING-SENDING DATA */
  //
  stream_sensor_data_normal_mode(sockfd, &dev);
  // close the socket 
  close(sockfd); 
}
