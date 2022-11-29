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
#include "Dandelions.h"             // This is our custom header file for the payload

//___Function prototypes
void pingWatchdog();            
int driveMotor(bool, int);    
void enMotor(bool en);          
int poll();      
void setupGpios();               


//___Function Definitions 

// Enable the motor with a single variable
void enMotor(bool en){
    gpio_set_level(MOTEN, !en);        // Enable the motor
    gpio_set_level(MSLEEP, en);     // Disable sleep mode (Active low)
    
}

// drive the motor with a diretion and a number of pulses
// returns error state
int driveMotor(bool dir, int pulses){
    enMotor(1);                     // call function to enable motor 
    gpio_set_level(MOTDIR,dir);     // Set direction 
    int error =0;
    for (int i =0; i< pulses; i++){
        if (i % 10){                        // ping watchdog every 10 motor pulses 
            pingWatchdog();
        }
        gpio_set_level(MOTSTEP,1);          // Motor step high
        vTaskDelay(10/portTICK_PERIOD_MS);  // wait for 10ms
        gpio_set_level(MOTSTEP,0);          // Motor step low
        vTaskDelay(10/portTICK_PERIOD_MS);  // Wait for 10ms
        error =poll();
        if(error!=1){                      // if all coditions are met withing poll then do nothing
            //printf("ERROR: Poll error: %d - Quiting from experiment.\n", error);
            return error;   
        }
    }
    enMotor(0);                            // Disable motor after use
    return 1;   // happy return 
}

// Simple function to ping the hardware watchdog.
void pingWatchdog(){
    gpio_set_level(WDI,1);              // WDOG High 
    vTaskDelay(1/portTICK_PERIOD_MS);   // Wait 1ms
    gpio_set_level(WDI,0);              // WDOG Low
}

// Check the motor fault, the Fuse falt and the q button on the keybord. 
// returns error code defined in Dandelions.h
int poll(){
    if(gpio_get_level(MFAULT) ==0){
        // Asserts Low in fault condition
        printf("ERROR: Motor Fault Detected. Returning to IDLE.\n");
        return MOTOR_FAULT;             // Motor Fault code
    }
    if(gpio_get_level(FFAULT) == 0){
        //Asserts low in fault condition
        printf("ERROR: Fuse Fault Detected. Returning to IDLE.\n");
        return FUSE_FAULT;           // fuse fualt code

    }
    char c = getchar();
    if(c == 'q'){
        printf("RUN: Quit by user\n");
        printf("RUN: Returning to idle state.\n");
        return FORCE_QUIT;          // quit experiment on keypress condition
    }
    return 1;               // for a success 
}

// Void return function that sets up the ESP32 in the SMC_D2 configuration
void setupGpios(){
    // [TODO] ->  change this to be the better bitmask function implemnetation
    // Set all the pin directions 
    gpio_set_direction(MFAULT,GPIO_MODE_INPUT);
    gpio_set_direction(FFAULT, GPIO_MODE_INPUT);
    gpio_set_direction(MOTEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(MVEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(WDI, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTDIR, GPIO_MODE_OUTPUT);
    gpio_set_direction(MSLEEP, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTSTEP, GPIO_MODE_OUTPUT);
    // Set all outputs to be pulldown resistors. 
    // Set all inputs to be pullup
    gpio_pullup_en(MFAULT);
    gpio_pullup_en(FFAULT);
    gpio_pulldown_en(MOTEN);
    gpio_pulldown_en(MVEN);
    gpio_pulldown_en(WDI);
    gpio_pulldown_en(MOTDIR);
    gpio_pulldown_en(MSLEEP);
    gpio_pulldown_en(MOTSTEP);
}

int error = 0;              // Decaire error variable

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
                    printf("RUN: Experiment Successful! returning to idle.\n");                                  // print reverse spin success
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
    esp_restart();      // Restart the esp
}
