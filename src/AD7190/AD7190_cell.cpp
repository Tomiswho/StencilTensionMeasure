#include <Arduino.h>
#include "AD7190_cell.h"
#include "util/AD7190.h"

const uint8_t recursive_reboot_limit = 10;

AD7190::AD7190(uint8_t cs_pin)
{
    cs = cs_pin;
    spi_settings = SPISettings(1000000, MSBFIRST, SPI_MODE3);
    last_measurement_time = 0;
    calibrate_factor = 1;

    conf_reg =
        AD7190_CONF_CHOP | // chop enable (zero drift/offset)
        //AD7190_CONF_REFSEL |  // not set = using REF1 (Vref = AVDD)
        //AD7190_CONF_REFSEL |  // set = using REF2 (Vref = 2.5V)
        AD7190_CONF_CHAN(1 << AD7190_CH_AIN3P_AIN4M) |
        // AD7190_CONF_BURN  | //do not use burn-currents
        // AD7190_CONF_REFDET | do not use reference detection
        AD7190_CONF_BUF | // enable analog input buffer
        // AD7190_CONF_UNIPOLAR | // using bipolar mode
        AD7190_CONF_GAIN(AD7190_CONF_GAIN_128); // Gain = 1 (Vin = max. +-Vref)
    mode_reg =
        AD7190_MODE_SEL(AD7190_MODE_CONT) | // continuous sampling
        // AD7190_MODE_DAT_STA | // no status byte tranmission after data
        AD7190_MODE_CLKSRC(AD7190_CLK_INT) | // using internal clock (4.92MHz +-4%)
        AD7190_MODE_SINC3 |                  // using SINC4 filter
        // AD7190_MODE_ENPAR | // do not enable parity bit
        // AD7190_MODE_SCYCLE | // do not settle in one cycle
        // AD7190_MODE_REJ60 | // do net reject 60Hz
        AD7190_MODE_RATE(145);
    //AD7190_MODE_RATE(9);
}

void AD7190::init()
{
    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH);
    SPI2.begin();
    pinMode(cs, OUTPUT);
    digitalWrite(cs, HIGH);
    uint8_t ret;
    digitalWrite(cs, LOW);
    SPI2.beginTransaction(spi_settings);
    do
    {
        AD7190_Reset();
        delay(1);
        unsigned char ret = AD7190_GetRegisterValue(AD7190_REG_ID, 1, 1);
        Serial.print("loadcell ID is");
        Serial.println(ret, BIN);
        delay(1000);
        Serial.println(ret & AD7190_ID_MASK, HEX);
        if (ret == 100)
        { // SPI_Init() failed
            Serial.println("Can't find load cell");
        }
        else if (ret == 0)
        { // wrong ID
            Serial.println("wrong load cell ID");
        }
    } while (ret & AD7190_ID_MASK != ID_AD7190);
    AD7190_Calibrate(AD7190_MODE_CAL_INT_ZERO, AD7190_CH_AIN3P_AIN4M);

    AD7190_SetRegisterValue(AD7190_REG_CONF, conf_reg, 3, 0);
    AD7190_SetRegisterValue(AD7190_REG_MODE, mode_reg, 3, 0);
    AD7190_SetRegisterValue(AD7190_REG_GPOCON, 1 << 6, 1, 0); // enable BPDSW, bridge power down switch

    tare();

    setCalibrationFactor(1785);  //g
    setCalibrationFactor(1.785); //mg
    SPI2.endTransaction();
    digitalWrite(cs, HIGH);
}

void AD7190::tare()
{
    digitalWrite(cs, LOW);
    SPI2.beginTransaction(spi_settings);
    int32_t data_sum = 0;
    uint8_t i;
    int32_t temp;
    for (i = 0; i < tare_times + 1; i++)
    {
        temp = AD7190_ContinuousRead();
        Serial.print("meas  ");
        Serial.println(temp);
        delay(100);

        uint8_t status = AD7190_GetRegisterValue(AD7190_REG_STAT, 1, 1);
        bool condition_1 = status & 0x81;
        bool condition_2 = status & ~0x81;
        if (!condition_1 || condition_2)
        {
            recursive_reboot_level++;
            Serial.print("status:");
            Serial.println(status);
            //Serial.println("stuck");
            Serial.println(condition_1);
            Serial.println(condition_2);
            reboot();
            return;
        }

        data_sum += temp;

        dataSampleSet[data_index] = temp;
        //writes to this so filter data isn't stale
        data_index++;
        if (data_index == filter_size)
        {
            data_index = 0;
        }
    }
    /*
    Serial.println("tare info:");
    Serial.println("\t data sum");
    Serial.print("\t");
    Serial.println(data_sum);
        Serial.println("\t i");
    Serial.print("\t");
    Serial.println(i);
    */
    tare_offset = float(data_sum) / float(i);
    //Serial.print("tare is");
    //Serial.println(tare_offset);
    SPI2.endTransaction();
    digitalWrite(cs, HIGH);
}

uint8_t AD7190::update()
{

    uint32_t result = 0;
    digitalWrite(cs, LOW);
    SPI2.beginTransaction(spi_settings);
    if (!AD7190_RDY_STATE)
    {
        data_index++;
        if (data_index == filter_size)
        {
            data_index = 0;
        }
        dataSampleSet[data_index] = AD7190_ContinuousRead();
        digitalWrite(cs, HIGH);
        SPI2.endTransaction();
        return true;
    }

    else
    {
        digitalWrite(cs, HIGH);
        SPI2.endTransaction();
        return false;
    }
}

void AD7190::setCalibrationFactor(float cal)
{
    calibrate_factor = cal;
}

float AD7190::getCalibrationFactor()
{
    return calibrate_factor;
}

float AD7190::getWeight()
{
    uint32_t temp = 0;
    uint8_t i;
    for (i = 0; i < filter_size; i++)
    {
        temp += dataSampleSet[i];
        //Serial.println(i);
    }
    return ((temp / filter_size - tare_offset) / calibrate_factor);
}

float AD7190::getRawWeight()
{
    return (dataSampleSet[data_index] - tare_offset) / calibrate_factor;
}

void AD7190::reboot()
{
    Serial.println("rebooting LCELL");

    uint8_t ret;
    digitalWrite(cs, LOW);
    SPI2.beginTransaction(spi_settings);
    do
    {
        AD7190_Reset();
        delay(1);
        unsigned char ret = AD7190_GetRegisterValue(AD7190_REG_ID, 1, 1);
        uint8_t stat = AD7190_GetRegisterValue(AD7190_REG_STAT, 1, 1);
        Serial.print("cell status is");
        Serial.println(stat);
        if (ret == 100)
        { // SPI_Init() failed
            Serial.println("Can't find load cell");
        }
        else if (ret == 0)
        { // wrong ID
            Serial.println("wrong load cell ID");
        }
    } while (ret & AD7190_ID_MASK != ID_AD7190);
    AD7190_Calibrate(AD7190_MODE_CAL_INT_ZERO, AD7190_CH_AIN3P_AIN4M);
    AD7190_SetRegisterValue(AD7190_REG_CONF, conf_reg, 3, 0);
    AD7190_SetRegisterValue(AD7190_REG_MODE, mode_reg, 3, 0);
    AD7190_SetRegisterValue(AD7190_REG_GPOCON, 1 << 6, 1, 0); // enable BPDSW, bridge power down switch
    Serial.print("old tare is");
    saved_tare_offset = tare_offset;
    Serial.println(tare_offset);
    float temp = 0;
    uint8_t i;
    for (i = 0; i < filter_size; i++)
    {
        temp += dataSampleSet[i];
        //Serial.println(i);
    }

    temp = temp / i;
    Serial.print("Temporary value is");
    Serial.println(temp);
    tare();
    Serial.print("new tare is");
    Serial.println(tare_offset);
    recursive_reboot_level = 0;
    //tare_offset -= temp;
    Serial.print("new temptare is");
    Serial.println(tare_offset);
    //setCalibrationFactor(1785);
    setCalibrationFactor(1.785); //mg
    SPI2.endTransaction();
    digitalWrite(cs, HIGH);
    Serial.println("end rebooting");
    delay(2000);
}

uint8_t AD7190::getStatus()
{
    digitalWrite(cs, LOW);
    SPI2.beginTransaction(spi_settings);
    uint8_t ret = AD7190_GetRegisterValue(AD7190_REG_STAT, 1, 1);
    SPI2.endTransaction();
    digitalWrite(cs, HIGH);
    return ret;
}