#include "ADS7953Q1.h"
#include <Arduino.h>
#include "component_utils.h"
#include "SPI.h"

//                            --- AD7953Q1 SPI data transmission ---
// _______________________________________________________________________________________________
// |   0    0    0    0  |   0  |   0    0    0    0  |   0  |   0  |   0  |   0    0    0    0  |
// | DI15 DI14 DI13 DI12 | DI11 | DI10 DI09 DI08 DI07 | DI06 | DI05 | DI04 | DI03 DI02 DI01 DI00 |
// |     Mode select     |      | Output pin address  | 2.5V |      | GPIO |   GPIO data output  |
// |                     |   ^  |                     | or 5V|   ^  |      |                     |
//                  Prog DI06 to DI00                        Power down
//
// SPI mode : 0

ADS7953Q1::ADS7953Q1(uint8_t csPin, int speed) : CS(csPin), SPIspeed(speed)
{
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);
}

// Function to read from ADC
uint16_t ADS7953Q1::read(uint8_t pinID, bool _5VinputRange = true, bool powerDown = false, bool debug = false)
{
    if (pinID >= CHX)
    {
        Serial.println("Invalid ADC read pin for ADS7953Q1");
        return NULL;
    }

    buffer = 0;
    buffer += 0b0001 << 12;
    buffer += 0b0001 << 11;
    buffer += pinID << 7;
    buffer += _5VinputRange << 6;
    buffer += powerDown << 5;

    SPI.beginTransaction(SPISettings(SPIspeed, MSBFIRST, SPI_MODE0));
    digitalWrite(CS, LOW);
    uint16_t response = SPI.transfer16(buffer);
    digitalWrite(CS, HIGH);
    SPI.endTransaction();

    uint16_t return_val = response;

    received_data[0] = response >> 12;
    received_data[1] = response &= 0x0FFF;

    return return_val;
}

uint16_t ADS7953Q1::getLastID()
{
    return received_data[0];
}

uint16_t ADS7953Q1::getLastValue()
{
    return received_data[1];
}

// Function to get voltage reading
double ADS7953Q1::getVoltageReading(int16_t reading)
{
    uint16_t lsb12 = (reading & 0x0FFF);
    return (lsb12 / 4095.0) * 5.0;
}

// Debugging function for ADC
void ADS7953Q1::debug(bool print = true)
{
    for (int i = 0; i < CHX; i++)
    {
        if (print)
        {
            Serial.println("Reading...");
            uint16_t reading = read(i, true, false, true);
            Serial.print("Output : ");
            Serial.println(i);
            Serial.println("-----------------------------------------");
            Serial.print("Data sent        : ");
            printPadded(buffer, "binary", 16);
            Serial.println("-----------------------------------------");
            Serial.print("Data receive     : ");
            printPadded(reading, "binary", 16);
            Serial.print("ID (BIN)         : ");
            printPadded(getLastID(), "binary", 4);
            Serial.print("ID (DEC)         : ");
            Serial.println(getLastID());
            Serial.print("Value (BIN)      : ");
            printPadded(getLastValue(), "binary", 12);
            Serial.print("Value (DEC)      : ");
            Serial.println(getLastValue());
            Serial.println("-----------------------------------------");
            Serial.print("Voltage reading : ");
            Serial.println(getVoltageReading(reading));
            Serial.println("\n");
        }
        else
        {
            read(i, true, false, false);
        }
        delay(5000);
    }
}
