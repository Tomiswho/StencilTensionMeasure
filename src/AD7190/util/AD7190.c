#include <stdint.h>
#include "AD7190.h"
#include "AD7190spi.h"


/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param registerAddress - Address of the register.
 * @param registerValue - Data value to write.
 * @param bytesNumber - Number of bytes to be written.
 * @param modifyCS - Allows Chip Select to be modified.
 *
 * @return none.
 *
 * @changed by Karl Zeilhofer to be endianness independent!
*******************************************************************************/
void AD7190_SetRegisterValue(uint8_t registerAddress,
                             uint32_t registerValue,
                             uint8_t bytesNumber,
                             uint8_t modifyCS)
{
    uint8_t writeCommand[5] = {0, 0, 0, 0, 0};
    uint8_t bytesNr         = bytesNumber;
    
    writeCommand[0] = AD7190_COMM_WRITE |
                      AD7190_COMM_ADDR(registerAddress);

    int shift=0;
    while(bytesNr>0)
    {
    	writeCommand[bytesNr] = (registerValue >> shift) & 0xff; // LSByte is last in writeCommand.
    	shift += 8;
    	bytesNr--;
    }
    SPI_Write(writeCommand, bytesNumber + 1);
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param registerAddress - Address of the register.
 * @param bytesNumber - Number of bytes that will be read.
 * @param modifyCS    - Allows Chip Select to be modified.
 *
 * @return buffer - Value of the register.
*******************************************************************************/
uint32_t AD7190_GetRegisterValue(uint8_t registerAddress,
                                      uint8_t bytesNumber,
                                      uint8_t modifyCS)
{
    uint8_t registerWord[5] = {0, 0, 0, 0, 0};
    uint32_t buffer          = 0x0;
    uint8_t i               = 0;
    
    registerWord[0] = AD7190_COMM_READ |
                      AD7190_COMM_ADDR(registerAddress);
    SPI_Read(registerWord, bytesNumber + 1);
    for(i = 1; i < bytesNumber + 1; i++) 
    {
        buffer = (buffer << 8) + registerWord[i];
    }
    
    return buffer;
}

/***************************************************************************//**
 * @brief Checks if the AD7190 part is present.
 *
 * @return status - Indicates if the part is present or not.
*******************************************************************************/
unsigned char AD7190_Init(void)
{
	unsigned char status = 1;
	unsigned char regVal = 0;

    //if(mySPI_Init(0,1000000),0,0)==0)
    //{
    //    return 100;
    //}

	//AD7190_Reset();
	/* Allow at least 500 us before accessing any of the on-chip registers. */
	delay(1);
	regVal = AD7190_GetRegisterValue(AD7190_REG_ID, 1, 0);
	if ((regVal & AD7190_ID_MASK) != ID_AD7190)
	{
		status = 0;
	}
	return status;
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return none.
*******************************************************************************/
void AD7190_Reset(void)
{
    uint8_t registerWord[7];
    
    // TODO: values arn't transmitted correctly (measured with oszi)


    // Test-Write, to bring all signals in order. 
    uint32_t a = 0xaaaaaaaa;
    SPI_Write(&a, 4);

    delay(1);

    registerWord[0] = 0x01;
    registerWord[1] = 0xFF;
    registerWord[2] = 0xFF;
    registerWord[3] = 0xFF;
    registerWord[4] = 0xFF;
    registerWord[5] = 0xFF;
    registerWord[6] = 0xFF;
    SPI_Write(registerWord, 7);

}

/***************************************************************************//**
 * @brief Set device to idle or power-down.
 *
 * @param pwrMode - Selects idle mode or power-down mode.
 *                  Example: 0 - power-down
 *                           1 - idle
 *
 * @return none.
*******************************************************************************/
void AD7190_SetPower(uint8_t pwrMode)
{
     uint32_t oldPwrMode = 0x0;
     uint32_t newPwrMode = 0x0;
 
     oldPwrMode = AD7190_GetRegisterValue(AD7190_REG_MODE, 3, 1);
     oldPwrMode &= ~(AD7190_MODE_SEL(0x7));
     newPwrMode = oldPwrMode | 
                  AD7190_MODE_SEL((pwrMode * (AD7190_MODE_IDLE)) |
                                  (!pwrMode * (AD7190_MODE_PWRDN)));
     AD7190_SetRegisterValue(AD7190_REG_MODE, newPwrMode, 3, 1);
}

/***************************************************************************//**
 * @brief Waits for RDY pin to go low.
 *
 * @return none.
*******************************************************************************/
void AD7190_WaitRdyGoLow(void)
{
    uint32_t timeOutCnt = 0xFFFFF;
    timeOutCnt=0x8FFFF;
    
    while(AD7190_RDY_STATE && timeOutCnt--)
    {
        ;
    }
}

/***************************************************************************//**
 * @brief Selects the channel to be enabled.
 *
 * @param channel - Selects a channel.
 *  
 * @return none.
*******************************************************************************/
void AD7190_ChannelSelect(uint16_t channel)
{
    uint32_t oldRegValue = 0x0;
    uint32_t newRegValue = 0x0;
     
    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_CONF, 3, 1);
    oldRegValue &= ~(AD7190_CONF_CHAN(0xFF));
    newRegValue = oldRegValue | AD7190_CONF_CHAN(1 << channel);   
    AD7190_SetRegisterValue(AD7190_REG_CONF, newRegValue, 3, 1);
}

/***************************************************************************//**
 * @brief Performs the given calibration to the specified channel.
 *
 * @param mode - Calibration type.
 * @param channel - Channel to be calibrated.
 *
 * @return none.
*******************************************************************************/
void AD7190_Calibrate(uint8_t mode, uint8_t channel)
{
    uint32_t oldRegValue = 0x0;
    uint32_t newRegValue = 0x0;
    
    AD7190_ChannelSelect(channel);
    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_MODE, 3, 1);
    oldRegValue &= ~AD7190_MODE_SEL(0x7);
    newRegValue = oldRegValue | AD7190_MODE_SEL(mode);

    AD7190_SetRegisterValue(AD7190_REG_MODE, newRegValue, 3, 0); // CS is not modified.
    AD7190_WaitRdyGoLow();
}

/***************************************************************************//**
 * @brief Selects the polarity of the conversion and the ADC input range.
 *
 * @param polarity - Polarity select bit. 
                     Example: 0 - bipolar operation is selected.
                              1 - unipolar operation is selected.
* @param range - Gain select bits. These bits are written by the user to select 
                 the ADC input range.     
 *
 * @return none.
*******************************************************************************/
void AD7190_RangeSetup(uint8_t polarity, uint8_t range)
{
    uint32_t oldRegValue = 0x0;
    uint32_t newRegValue = 0x0;
    
    oldRegValue = AD7190_GetRegisterValue(AD7190_REG_CONF,3, 1);
    oldRegValue &= ~(AD7190_CONF_UNIPOLAR |
                     AD7190_CONF_GAIN(0x7));
    newRegValue = oldRegValue | 
                  (polarity * AD7190_CONF_UNIPOLAR) |
                  AD7190_CONF_GAIN(range); 
    AD7190_SetRegisterValue(AD7190_REG_CONF, newRegValue, 3, 1);
}

/***************************************************************************//**
 * @brief Returns the result of a single conversion.
 *
 * @return regData - Result of a single analog-to-digital conversion.
*******************************************************************************/
uint32_t AD7190_SingleConversion(void)
{
    uint32_t command = 0x0;
    uint32_t regData = 0x0;
 
    command = AD7190_MODE_SEL(AD7190_MODE_SINGLE) | 
              AD7190_MODE_CLKSRC(AD7190_CLK_INT) |
              AD7190_MODE_RATE(0x060);    
    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3, 0); // CS is not modified.
    AD7190_WaitRdyGoLow();
    regData = AD7190_GetRegisterValue(AD7190_REG_DATA, 3, 0);
    
    return regData;
}

/***************************************************************************//**
 * @brief Returns the average of several conversion results.
 *
 * @return samplesAverage - The average of the conversion results.
*******************************************************************************/
uint32_t AD7190_ContinuousReadAvg(uint8_t sampleNumber)
{
    uint32_t samplesAverage = 0x0;
    uint8_t count = 0x0;
    uint32_t command = 0x0;
    
    command = AD7190_MODE_SEL(AD7190_MODE_CONT) | 
              AD7190_MODE_CLKSRC(AD7190_CLK_INT) |
              AD7190_MODE_RATE(0x060);
    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3, 0); // CS is not modified.
    for(count = 0;count < sampleNumber;count ++)
    {
        AD7190_WaitRdyGoLow();
        samplesAverage += AD7190_GetRegisterValue(AD7190_REG_DATA, 3, 0); // CS is not modified.
    }
    samplesAverage = samplesAverage / sampleNumber;
    
    return samplesAverage ;
}

/***************************************************************************//**
 * @brief Read data from temperature sensor and converts it to Celsius degrees.
 *
 * @return temperature - Celsius degrees.
*******************************************************************************/
uint32_t AD7190_TemperatureRead(void)
{
    uint8_t temperature = 0x0;
    uint32_t dataReg = 0x0;
    
    AD7190_RangeSetup(0, AD7190_CONF_GAIN_1);
    AD7190_ChannelSelect(AD7190_CH_TEMP_SENSOR);
    dataReg = AD7190_SingleConversion();
    dataReg -= 0x800000;
    dataReg /= 2815;   // Kelvin Temperature
    dataReg -= 273;    //Celsius Temperature
    temperature = (uint32_t) dataReg;
    
    return temperature;
}

uint32_t AD7190_ContinuousRead()
{
    uint32_t regData = 0x0;
    AD7190_WaitRdyGoLow();
    regData = AD7190_GetRegisterValue(AD7190_REG_DATA, 3, 0);
    return regData;

}

void AD7190_SetContinuousRead()
{
    uint32_t command = 0x0;
    uint32_t regData = 0x0;
 
    command = AD7190_MODE_SEL(AD7190_MODE_CONT) | 
              AD7190_MODE_CLKSRC(AD7190_CLK_INT) |
              AD7190_MODE_RATE(0x060);    
    AD7190_SetRegisterValue(AD7190_REG_MODE, command, 3, 0);
}