#include "ADS7953Q1.h"

//                            --- AD7953Q1 SPI data transmission ---
// _______________________________________________________________________________________________
// |   0    0    0    0  |   0  |   0    0    0    0  |   0  |   0  |   0  |   0    0    0    0  |
// | DI15 DI14 DI13 DI12 | DI11 | DI10 DI09 DI08 DI07 | DI06 | DI05 | DI04 | DI03 DI02 DI01 DI00 |
// |     Mode select     |      | Output pin address  | 2.5V |      | GPIO |   GPIO data output  |
// |                     |   ^  |                     | or 5V|   ^  |      |                     |
//                  Prog DI06 to DI00                        Power down
//
// SPI mode : 0


void ADS7953Q1_init(ADS7953Q1* ads7953q1, uint16_t CS, GPIO_TypeDef* peripheral, SPI_HandleTypeDef spi_handler,
		uint16_t(*read)(ADS7953Q1*), double(*getVoltageReading)(ADS7953Q1*), uint16_t(*getLastAddress)(ADS7953Q1*), uint16_t(*getLastValue)(ADS7953Q1*)) {

	HAL_GPIO_WritePin(peripheral, CS, GPIO_PIN_SET);

	ads7953q1->CHX = 16;
	ads7953q1->CS = CS;
	ads7953q1->peripheral = peripheral;
	ads7953q1->spi_handler = spi_handler;

	ads7953q1->read = read;
	ads7953q1->getVoltageReading = getVoltageReading;
	ads7953q1->getLastAddress = getLastAddress;
	ads7953q1->getLastValue = getLastValue;
}

// Function to read from ADC
uint16_t ADS7953Q1_read(ADS7953Q1* ads7953q1, uint8_t pinID, int _5VinputRange, int powerDown)
{
    if (pinID >= ads7953q1->CHX) { return -1; }

    uint8_t pinNumMSB = (pinID & 0b1110);	// 3 bits MSB of address pin
    pinNumMSB = pinNumMSB >> 1;				// Shift to get the three at DI10, 9 and 8
    uint8_t pinNumLSB = (pinID & 0b0001);	// 1 bit LSB of address pin

    uint8_t t_buffer[2];
    uint8_t r_buffer[2];

    // Store bytes as little-endian
    t_buffer[0] = 0;
    t_buffer[1] = 0;

    t_buffer[0] += (1 << 4);
    t_buffer[0] += (1 << 3);
    t_buffer[0] += (pinNumMSB);

    t_buffer[1] += (pinNumLSB << 7);
    t_buffer[1] += (_5VinputRange << 6);
    t_buffer[1] += (powerDown << 5);

    HAL_GPIO_WritePin(ads7953q1->peripheral, ads7953q1->CS, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&(ads7953q1->spi_handler), &t_buffer, &r_buffer, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(ads7953q1->peripheral, ads7953q1->CS, GPIO_PIN_SET);

    uint16_t return_val = r_buffer;

    uint8_t MSB = r_buffer[0];
    uint8_t LSB = r_buffer[1];

    ads7953q1->addr = (MSB & 0b11110000);
    ads7953q1->addr = ads7953q1->addr >> 4;
    ads7953q1->val = (MSB & 0b00001111) << 8;
    ads7953q1->val += LSB;

    return return_val;
}

uint16_t ADS7953Q1_getLastAddress(ADS7953Q1* ads7953q1)
{
    return ads7953q1->addr;
}

uint16_t ADS7953Q1_getLastValue(ADS7953Q1* ads7953q1)
{
    return ads7953q1->val;
}

// Function to get voltage reading
double ADS7953Q1_getVoltageReading(ADS7953Q1* ads7953q1)
{
    return (ads7953q1->val / 4096.0) * 5.0;
}
