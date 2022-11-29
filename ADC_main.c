/* ADC Comms 
Author :Harindi P
DATE : 25/11/22
*/

//Libraries

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "unity_test_runner.h"
#include "ADC.h"



uint8_t *data_read;                             //declare buffer

/* Function Definition */

static i2c_config_t i2c_master_init(void){
    i2c_config_t conf_master = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,        // select GPIO 21
        .scl_io_num = I2C_MASTER_SCL_IO,        // select GPIO 22
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ, // select frequency 
       .clk_flags = 0,                          // clock select
    };
    return conf_master;
}

static void i2c_master_read_slave(void){

    //establish master command link
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    printf("establish master command link\n");

    //Populate with start bit
    i2c_master_start(cmd);
    printf("Populate with start bit\n");

    //write a bit
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) || I2C_MASTER_READ, ACK_EN);
    printf("Command link to read from the slave\n");

    //read
    i2c_master_read(cmd , data_read, RW_LENGTH, I2C_MASTER_ACK );
    printf("read\n");

    //read a bit
    i2c_master_read_byte(cmd , data_read + RW_LENGTH, I2C_MASTER_NACK);
    printf("read a byte\n");

    //End command queue
    i2c_master_stop(cmd);
    printf("End command queue\n");

    //Begin queue
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 5000 / portTICK_PERIOD_MS);
    printf("Begin queue\n");

    //delete link
    i2c_cmd_link_delete(cmd);
    printf("delete link\n");

}

void i2c_master_cmd_set(void){

    //Initialise config
    i2c_config_t conf_master = i2c_master_init();
    printf("Initialise config\n");
    
    //Installation of driver
    i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER,I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    printf("Installation of driver\n");
    
    //Configuration of parameters
    i2c_param_config(I2C_MASTER_NUM, &conf_master);
    printf("Configuration of parameters\n");
}

void wd_pulse(void){

    vTaskDelay(1000/portTICK_PERIOD_MS);
    gpio_set_level(WDI , 1);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    gpio_set_level(WDI , 0);
    printf("pulsed\n");
}

void adc_pwr(void){

    //setting pins as output for ADC
    gpio_set_direction(ADCLPWR, GPIO_MODE_OUTPUT);
    gpio_set_direction(ADCRPWR, GPIO_MODE_OUTPUT);
    printf("ADC's Pins set to outputs\n");

    //set ADC pins as high
    gpio_set_level(ADCLPWR, 1);
    gpio_set_level(ADCRPWR, 1);
    printf("ADC's powered\n");
}

void print_buffer(uint8_t *data_rd){

    printf("Display the buffer\n");

    vTaskDelay(100 / portTICK_PERIOD_MS);
    for (int i = 0; i < RW_LENGTH; i++) {
        printf("%d", data_rd[i]);
    }

}


/* Main Function */

void app_main(void){

    printf("Operations ready to go!\n");

    i2c_master_cmd_set();

    while(true){

    //Set WD as an output
    gpio_set_direction(WDI, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(WDI, GPIO_PULLDOWN_ONLY);
    printf("WD Pins set to outputs\n");

    //pulse wd
    wd_pulse();

    //power adc
    adc_pwr();
  
    //Allocate size to buffer
    data_read = (uint8_t *) malloc(DATA_LENGTH);
    memset(data_read, 0, DATA_LENGTH);
    printf("Create buffer for read data\n");

    //reads adc and populates buffer
    i2c_master_read_slave();

    //display the buffer
    print_buffer(data_read);

    }
}