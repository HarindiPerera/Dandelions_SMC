
#ifndef DAND_SMC_H_INCLUDED
#define DAND_SMC_H_INCLUDED

/*
     Dandelions D1 GPIO LIST
    GPIO    Func            IO          O_mask  I_Mask  PDmask  PU mask
    IO00 = boot =            = Input    0       0       
    IO01 =                              0       0
    IO02 = Spare/whatever               0       0
    IO03 =                              0       0
    IO04 = DBCON             =          0       0
    IO05 = ESPIO05           = Spare    1       0
    IO06 =                              0       0
    IO07 =                              0       0    
    IO08 =                              0       0
    IO09 =                              0       0
    IO10 =                              0       0
    IO11 =                              0       0
    IO12 = ESPIO12                      0       0
    IO13 = ESPIO13                      0       0
    IO14 = ESPIO14                      0       0
    IO15 = ESPIO15                      0       0
    IO16 = ADC_L_PWR        = Output    1       0
    IO17 = ADC_R_PWR        = Output    1       0
    IO18 = Spare            = x         0       0
    IO19 = Spare            = x         0       0
    IO20 =                              0       0
    IO21 = SDA              =           0       0
    IO22 = SCL              =           0       0
    IO23 = WDI              = OUTOUT    1       0
    IO24 =                              0       0
    IO25 = Motor Direction  = Output    1       0
    IO26 = Motor Step       = Output    1       0
    IO27 = Motor Enable     = Output    1       0
    IO28 =                              0       0
    IO29 =                              0       0
    IO30 =                              0       0
    IO31 =                              0       0
    IO32 = Motor Voltage EN = Output    1       0
    IO33 = Motor sleep      = Output    1       0
    IO34 = FUSE FAULT       = Input     0       1      Interupt
    IO35 = Motor fault      = Input     0       1      Interupt
    */



// Defining gpio names as constant numbers. SMC D2

#define ADCLPWR 16
#define ADCRPWR 17
#define MOTDIR 25
#define MOTSTEP 26
#define MOTEN 27
#define MVEN 32
#define MSLEEP 33
#define FFAULT 34
#define MFAULT 35
#define WDI 23
// For these masks each bit coresponds to a gpio, bit 4 = gpio 4 ect
#define OUTPUT_MASK 0x30E030020
#define INPUT_MASK  0xC0E000000

// Dandelion Error Codes
#define SUCCESS 1
#define MOTOR_FAULT -1
#define FUSE_FAULT -2
#define FORCE_QUIT -3

// Function protypes

// Takes no @param, returns void, pulses watchdog for 1ms
void pingWatchdog();       

// @param boo dir , int  = number of steps
// drives motor in direction for x number of steps. 
// returns Dandelion error code.
int driveMotor(bool, int);   

// @param en =1 : enable motor, en=0 : Motor driver put into sleep mode. 
void enMotor(bool en);  

int poll();  

void setupGpios(); 

#endif