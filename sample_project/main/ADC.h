/***********************
 
ADC Comms 
Author :Harindi P
DATE : 25/11/22

************************/

// Header guards
#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED


/*
#define ADCLPWR 16                              //Left ADC power on gpio 16
#define ADCRPWR 17                              //Right ADC power on gpio 17
#define I2C_MASTER_SDA_IO 21                    //ESP32 SDA 21  
#define I2C_MASTER_SCL_IO 22                    //ESP32 SCL 22
#define I2C_SLAVE_SDA_IO 
#define I2C_SLAVE_SCL_IO

#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_NUM I2C_NUM_0                // [TODO] -> This define seems strange. 
#define I2C_MASTER_TX_BUF_DISABLE  0            //i2c master does not need a buffer
#define I2C_MASTER_RX_BUF_DISABLE  0            //i2c master does not need a buffer
#define ESP_SLAVE_ADDR 0xD5                     //Device address 1101 | ADC1 : 100 | ADC2 : 010
#define ACK_EN 0x1                              //Master checks Ack enabled
#define ACK_DIS 0x0                             //Master checks Ack disabled

#define DATA_LENGTH 512                         //Max Buffer Len
#define RW_LENGTH 129                           //Reg READ_WRITE len 
*/

// Function prototypes

uint8_t *data_read;      //declare buffer

/* Function Definition */
// [TODO] -> Write function descriptions

i2c_config_t i2c_master_init(void);

void i2c_master_read_slave(void);

void i2c_master_cmd_set(void);

void adc_pwr(bool en);

void print_buffer(uint8_t *data_rd);

#endif 