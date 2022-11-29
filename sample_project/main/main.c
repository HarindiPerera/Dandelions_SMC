/*******************
*   H.Perera & J.Sheath
*   2022
*   Dandelions
*
*************************/

#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"          // Allows for delays / tasks 
#include "driver/gpio.h"            // need to include this to be able to use GPIO's
#include "driver/i2c.h"             // for the ADC 
#include "unity_test_runner.h"      // added for i2c functionality
#include "sdkconfig.h"              // added for i2c functionality

#include "Dand_SMC.h"               // Definitions of all the defined constants and basic motor functionality functions
#include "ADC.h"                    // ADC functions

int error = 0;                      // Decaire error variable
int pulses =500;                    // change this based on the number of pulses in either direction we want on TVAC day

//___________________________________________APP__MAIN_______________________________________________

void app_main(void){
// SETUP 
char ch ='0';
setupGpios();                   // Setup function for all the GPIOS in the SMC_D2 configuration
i2c_master_cmd_set();           // Setup the i2c bus 
printf("Dandelions SMCD2: Ready in idle state.\nIDLE: ");               // User instructions
printf("Press 'r' to run experiment once and 'q' to quit/restart.\n");  // User instructions
gpio_set_level(MVEN, 1);        // Enable Motor voltage from E-Fuse


// INFINITE LOOP // 
    while(1){   
        //ch = getchar();                                 // if 'r' is pressed then the experiment will be run
        if (getchar() == 'r'){
            printf("RUN: Running Experiment\n");

            error = driveMotor(1,pulses);              // Spin motor one way for 500 pulses

            /*if error != SUCCESS then go to fault condition, else...*/
            adc_pwr(1);                                     //power adc
            data_read = (uint8_t *) malloc(DATA_LENGTH);    //Allocate size to buffer
            memset(data_read, 0, DATA_LENGTH);
            printf("RUN: Create buffer for read data\n");
            i2c_master_read_slave();        //reads adc and populates buffer
            print_buffer(data_read);        // Display the buffer
            adc_pwr(0);                     // Power down the ADC's
 
            if (error==SUCCESS){                            // if all good... then spin the other way and read ADC's
                printf("RUN: Reversing direction\n");       // Tell everyone we are half way
                error  = driveMotor(0,pulses);                 // Spin motor other way for 500 pulses

                /* if error != success -> handel that issue, else ... */

                adc_pwr(1);                                     //power adc
                data_read = (uint8_t *) malloc(DATA_LENGTH);    //Allocate size to buffer
                memset(data_read, 0, DATA_LENGTH);
                printf("RUN: Create buffer for read data\n");
                i2c_master_read_slave();                        //reads adc and populates buffer
                print_buffer(data_read);                        // Display the buffer
                adc_pwr(0);                                     // Power down the ADC's

                if(error == SUCCESS){
                    printf("RUN: Experiment Successful! returning to idle.\n");         // print reverse spin success
                }else if (error == MOTOR_FAULT || error == FUSE_FAULT){
                    printf("ERROR: Experiment was auto interupted (2nd half).\n");      // Print that there was an "interupt" from somwehere. (user or hardware)
                    printf("ERROR: Restarting.\n");
                    break;
                }
            }else if(error  ==MOTOR_FAULT || error == FUSE_FAULT){
               printf("ERROR: Experiment was auto interupted (1st half).\n");           // Print that there was an "interupt" from somwehere. (user or hardware)
               printf("ERROR: Restarting.\n");
            }
            
                                                                    // [TODO] -> center experiment. 
                                                                    // [TODO] -> enter sleep mode. 

            printf("IDLE: Press'r' to run experiment once and 'q' to quit/restart.\n");

        }else if (ch == 'q'){                                                   // if 'q' is pressed in idel mode then 
            printf("IDLE: Program quit by user from idle state, restarting.\n");      
            break;
        }else if (getchar() =='\033'){    // first value was escape 
            getchar();                    // skip the next entry 
            switch(getchar()){            // the real value
                case 'A':               // Arrow up
                    driveMotor(1,10);   // 10 is pulse length
                break;
                case 'B':               // Arrow down
                    driveMotor(0,10);   // 
                break;
            }
            
        }else {
            vTaskDelay(100/ portTICK_PERIOD_MS);    // wait 0.1 second if in idle state
            pingWatchdog();                             // ping the watchdog if in idle state
        }
    }

    /*CLEANUP*/
    adc_pwr(0);         // Power down the ADC's (if they were on at all)
    enMotor(0);         // Disable all motor functions
    fflush(stdout);     // Clear terminal window
    esp_restart();      // Restart the esp (esp_system.c F12+click takes you to source)
}
