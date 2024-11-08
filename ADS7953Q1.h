#ifndef ADS7953Q1_H_
#define ADS7953Q1_H_

#include <stdint.h>
#include "stm32l4xx_hal.h"

typedef struct ADS7953Q1 {
	uint8_t CHX; // Number of ADC channels (ADS7953Q1)
	uint16_t CS; // Chip select pin for ADC
	uint8_t addr;
	uint16_t val;
	GPIO_TypeDef* peripheral;
	SPI_HandleTypeDef spi_handler;

    // Function to read from ADC
    uint16_t (*read)(struct ADS7953Q1* ads7953q1, uint8_t pinID, int _5VinputRange, int powerDown);
    // Function to get voltage reading
    double (*getVoltageReading)(struct ADS7953Q1* ads7953q1);
    uint16_t (*getLastAddress)(struct ADS7953Q1* ads7953q1);
    uint16_t (*getLastValue)(struct ADS7953Q1* ads7953q1);

} ADS7953Q1;

void ADS7953Q1_init(ADS7953Q1* ads7953q1, uint16_t CS, GPIO_TypeDef* peripheral, SPI_HandleTypeDef spi_handler,
		uint16_t(*read)(ADS7953Q1*), double(*getVoltageReading)(ADS7953Q1*), uint16_t(*getLastID)(ADS7953Q1*), uint16_t(*getLastValue)(ADS7953Q1*));
uint16_t ADS7953Q1_read(ADS7953Q1* ads7953q1, uint8_t pinID, int _5VinputRange, int powerDown);
double ADS7953Q1_getVoltageReading(ADS7953Q1* ads7953q1);
uint16_t ADS7953Q1_getLastAddress(ADS7953Q1* ads7953q1);
uint16_t ADS7953Q1_getLastValue(ADS7953Q1* ads7953q1);

#endif
