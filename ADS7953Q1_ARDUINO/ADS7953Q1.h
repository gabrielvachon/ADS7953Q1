#ifndef ADS7953Q1_H_
#define ADS7953Q1_H_
#include <stdint.h>

class ADS7953Q1 {
  private:
    const uint8_t CHX = 16; // Number of ADC channels (ADS7953Q1)
    uint8_t CS; // Chip select pin for ADC
    int SPIspeed;
    uint16_t buffer;
    uint16_t received_data[2];

  public:
    // Constructor to initialize the ADC object
    ADS7953Q1(uint8_t csPin, int speed);
    // Function to read from ADC
    uint16_t read(uint8_t pinID, bool _5VinputRange = true, bool powerDown = false, bool debug = false);
    // Function to get voltage reading
    static double getVoltageReading(int16_t reading);
    uint16_t getLastID();
    uint16_t getLastValue();
    // Debugging function for ADC
    void debug(bool print = true);

};

#endif
