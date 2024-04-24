#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/i2c2_master.h"
#include "mcc_generated_files/examples/i2c2_master_example.h"
#include "mcc_generated_files/eusart1.h"
#include <string.h>
#include <xc.h>
#include <stdint.h>

#define HIH6030_I2C_Humidity  0x27 //HIH6030 I2C address
#define TC74_ADDR 0x4C // TC74 I2C address
#define TC74_TEMP_REG 0x00 // Temperature register address

void main(void)
{
    SYSTEM_Initialize(); //Initialize System
    I2C2_Initialize(); //Initialize I2C
    EUSART1_Initialize(); //Initialize EUSART1
    SPI1_Initialize(); // Initialize SPI
    I2C2_Open(HIH6030_I2C_Humidity); //Wake Humidity Sensor
    
 
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    while (1)
    {   
       I2C2_WriteNBytes(HIH6030_I2C_Humidity, 0x0000, 2); //Writing to Humidity Sensor
       __delay_ms(50); //Delay required for buffer
       uint8_t humidityData[2]; //Declaring a 2 byte array for Humidity sensor data
       I2C2_ReadNBytes(HIH6030_I2C_Humidity, humidityData, 2); //Reading data from Humidity sensor and storing data into variable
       __delay_ms(50);
       // Combine two bytes into a single 16-bit unsigned integer to form the raw humidity value.
       uint16_t rawHumidity = (humidityData[0] << 8) | humidityData[1];
       // Convert the raw humidity value to a percentage based on the sensor's scale factor.
       //int humidityPercentage = 12;
       
       uint8_t temp_f =0;
       uint8_t temp_c = 0;
       
       uint8_t data;
       uint8_t fw = 0b11101111;
       uint8_t bw = 0b11101101;

       
       temp_c = I2C2_Read1ByteRegister(TC74_ADDR, 0x00);
       temp_f = ((temp_c*9)/5)+32;
       
       
       //Motor turns on O2 port
       if (temp_f >= 88)
       {
       int humidityPercentage = 9;
        HT_LED_SetHigh();
        __delay_ms(500);
        HT_LED_SetLow();
        __delay_ms(500);
        HT_LED_SetHigh();
        __delay_ms(500);
        HT_LED_SetLow();
        __delay_ms(500);
        HT_LED_SetHigh();
        __delay_ms(500);
        HT_LED_SetLow();
        __delay_ms(500);
        SPI1_Open(SPI1_DEFAULT);
        CSN_SetLow();
        data = SPI1_ExchangeByte(bw);  // Assuming 'bw' turns the motor on
        CSN_SetHigh();
        __delay_ms(50);  // Delays might be required for your specific motor control protocol

       }
       if (temp_f >= 85 && temp_f <= 88) 
       {
        int humidityPercentage = 11;
        SPI1_Open(SPI1_DEFAULT);
        CSN_SetLow();
        data = SPI1_ExchangeByte(fw);
        CSN_SetHigh();
        __delay_ms(500);   
        printf("Humidity: %u%%\nTemp: %u\n\r", humidityPercentage, temp_f);
       }
       if (temp_f >= 81 && temp_f <= 84) 
       {
        int humidityPercentage = 12;
        SPI1_Open(SPI1_DEFAULT);
        CSN_SetLow();
        data = SPI1_ExchangeByte(fw);
        CSN_SetHigh();
        __delay_ms(500);    
        printf("Humidity: %u%%\nTemp: %u\n\r", humidityPercentage, temp_f);
       }
        if (temp_f >= 77 && temp_f <= 80) 
        {
            
        int humidityPercentage = 13;
        SPI1_Open(SPI1_DEFAULT);
        CSN_SetLow();
        data = SPI1_ExchangeByte(fw);
        CSN_SetHigh();
        __delay_ms(500);
        printf("Humidity: %u%%\nTemp: %u\n\r", humidityPercentage, temp_f);
        }
       if (temp_f < 77)
       {
        int humidityPercentage = 14;
        SPI1_Open(SPI1_DEFAULT);
        CSN_SetLow();
        data = SPI1_ExchangeByte(fw);
        CSN_SetHigh();
        __delay_ms(500);    
        printf("Humidity: %u%%\nTemp: %u\n\r", humidityPercentage, temp_f);
       }
      
       //printf("Humidity: %u%%\nTemp: %u\n\r", humidityPercentage, temp_f);
        __delay_ms(2500);  
    }
}
