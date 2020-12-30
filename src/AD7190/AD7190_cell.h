#ifndef AD7190_CELL
#define AD7190_CELL

#include <Arduino.h>
#include <SPI.h>
extern SPIClass SPILoadCell;


const int8_t tare_times = 50;
const int8_t filter_size = 10;
const int8_t update_frequency =  11;
//const uint8_t LOOPS_TILL_UNHANDLED_ERROR = 3;

class AD7190
{
    public:
    AD7190(uint8_t cs_pin);
    //implements load_cell interface
    void init();
    void tare();
    void  setCalibrationFactor(float cal);
    float getCalibrationFactor();
    uint8_t update();
    float getWeight();
    float getRawWeight();
    void reboot();
    uint8_t getStatus();


    //private:
    
    uint32_t conf_reg;
    uint32_t mode_reg;
    uint8_t cs;
    uint32_t last_measurement_time;
    float tare_offset;
    float saved_tare_offset;
    SPISettings spi_settings;
    float calibrate_factor;
    long dataSampleSet[filter_size];
    uint8_t data_index;
    uint8_t recursive_reboot_level=0;
    
};


#endif //AD7190_CELL