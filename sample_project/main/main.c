// By Jake Sheath 
// 2022
// Dandelions 
// jsheath@wisenet.work

#include <stdio.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"            // need to include this to be able to use GPIO's
#include "Dand_SMC.h"               // Definitions of all the defined constants and basic motor functionality functions

int error = 0;                      // Decaire error variable

//___________________________________________APP__MAIN_______________________________________________

void app_main(void){
// SETUP 
char ch ='0';
setupGpios();                   // Setup function for all the GPIOS in the SMC_D2 configuration

printf("Dandelions SMCD2: Ready in idle state.\nIDLE: ");               // User instructions
printf("Press 'r' to run experiment once and 'q' to quit/restart.\n");  // User instructions

gpio_set_level(MVEN, 1);         // Enable Motor voltage from E-Fuse

// INFINITE LOOP // 
    while(1){   
        ch = getchar();                             // if 'r' is pressed then the experiment will be run
        if (ch == 'r'){
            printf("RUN: Running Experiment\n");

            error = driveMotor(1,500);              // Spin motor one way for 500 pulses

                                                    /*Read sensors here*/
 
            if (error==SUCCESS){                    // if all good... then spin the other way
                printf("RUN: Reversing direction\n");    // Tell everyone we are half way
                error  = driveMotor(0,500);         // Spin motor other way for 500 pulses

                                                    /*Read sensors here*/

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
            printf("IDLE: Program quit by user from idle state, restarting.\n");      //[TODO] -> Borrow proper power down processes from other code
            break;
        }else{
            vTaskDelay(100/ portTICK_PERIOD_MS);    // wait 0.1 second if in idle state
        }
        pingWatchdog();                             // ping the watchdog if in idle state
    }

    /*CLEANUP*/
    enMotor(0);         // Disable all motor functions
    fflush(stdout);     // Clear terminal window
    esp_restart();      // Restart the esp (esp_system.c F12+click takes you to source)
}
