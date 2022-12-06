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
#include "esp_intr_alloc.h"         // allows for interupts allocation
#include "Dand_SMC.h"               // Definitions of all the defined constants and basic motor functionality functions
#include "ADC.h"                    // ADC functions
#include "soc/soc.h"                // This defines the external interrupt sources. as ETS_<>_INTR_SOURCE
int error = 0;                      // Decaire error variable
int pulses =500;                    // change this based on the number of pulses in either direction we want on TVAC day


/* Non internal interrupts slots in both CPU cors are wired to an interrupts multiplexer which can be use to route any external interrupts source to any of these intrrupts slots
Allocating an external intrrupts will alwasy allocate it on the core that does the allocation
Freeing is also bound to the core. 
Disableing and enableing external interupts can be done from either core. 
Multiple external interrupts sources can share an interrupts slot by passing ESP_INTR_FLAG_SHARED as a flag to esp_inter_alloc()

Definately think i am going to steal from the interrupt example program. 

so lets have lunch and then do that. 
i am freaking starving. 


*/ 




// attempts at interupts 

void Boi(){
    printf("You have interupted me you absolute ding dong\n");

}

intr_handler_t InterruptBoi = Boi; // intr




//___________________________________________APP__MAIN_______________________________________________



void app_main(void){

esp_intr_alloc(ETS_GPIO_INTR_SOURCE, 0, InterruptBoi ,NULL, NULL);
/*The interupt will always be allocatted on the core that runs this function
this finds an interrupts that matches the restrictions given in the flags parameters, maps the given interrupts source to it and hoks up the given handler
(with optional arguments), it can also return a handle for the interrupts as well. 
if ESP_INTR_FLAG_IRAM flag is use and the andler address is not in IRAM or RTC_FAST_MEM then ESP_ERROR_INVALID_ARG is returned. 

source - The interupts source-> needs to be one of the ETS_<>_INTR_SOURCE sources defined in the header. 
flags - flags - and ORread mask of the ESP_INTR_FLAG_<> defines. Thes restrict the choice of interrupts that this routine can choose from. 
    if the value is 0 it will default to allocating non-shared intrrupts level 1, 2 or 3. 
handler- Must be NULL when an intrrupts of level >3 is requested because these types of intrrupts arnt't C -callable. 
*/

char ch ='0';
setupGpios();                   // Setup function for all the GPIOS in the SMC_D2 configuration
i2c_master_cmd_set();           // Setup the i2c bus 
printf("Dandelions SMCD2: Ready in idle state.\n");               // User instructions
printf("IDLE: Press 'r' to run experiment once and 'q' to quit/restart.\n");  // User instructions
gpio_set_level(MVEN, 1);        // Enable Motor voltage from E-Fuse

/*
temp_sensor_config_t temp_sens = {
    range_min =0.0;
    range_min =60.0;
};
temp_sensor_set_config(temp_sens);
float tsens =0;              // temprture value 
*/
// Test code for an internal temp sensor. 
/*
temperature_* temp_handle = NULL; 
temperature_sensor_config_t temp_sensor = {
    .range_min = 0.0; 
    .range_max = 80.0;
};
ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor, &temp_handle));
float tsen_out; 
temperature_sensor_install();
*/


// INFINITE LOOP // 
    while(1){   
        ch = getchar();                                 // if 'r' is pressed then the experiment will be run
        if (ch == 'r'){
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
                    getchar();
                break;
                case 'B':               // Arrow down
                    driveMotor(0,10);   // 
                    getchar();
                break;
            }
            
        }else {
            vTaskDelay(100/ portTICK_PERIOD_MS);    // wait 0.1 second if in idle state
            pingWatchdog();                             // ping the watchdog if in idle state

           // temp_sensor_read_celsius(&tsens);
           // printf("IDLE: Current core temp = %f\n",tsens);

            /*       Test code for checking tempreture stuff 
            ESP_ERROR_CHEKC(temperature_sensor_enable(temp_handle));    // Enable the temperaure sensor
            ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_handle,&tsens_out));    // get tempreture data
            ESP_ERROR_CHECK(temperature_sensor_disable(temp_handle));   // disable temp sensor to save power
            */
        }

    }

    /*CLEANUP*/
    adc_pwr(0);         // Power down the ADC's (if they were on at all)
    enMotor(0);         // Disable all motor functions
    fflush(stdout);     // Clear terminal window
    esp_restart();      // Restart the esp (esp_system.c F12+click takes you to source)
}
