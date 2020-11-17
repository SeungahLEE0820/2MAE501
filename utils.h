#ifndef UTILS_H_
#define UTILS_H_

void SPI_BME280_CS_High(void);

void SPI_BME280_CS_Low(void);

void user_delay_ms(uint32_t period);

int8_t user_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);

int8_t user_spi_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);

void print_sensor_data(struct bme280_data *comp_data);

void add_timespec (struct timespec *s, const struct timespec *t1, const struct timespec *t2);

#endif
