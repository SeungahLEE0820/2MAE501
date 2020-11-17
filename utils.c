#include "utils.h"

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
