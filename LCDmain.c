// Sammy Nayhouse (san6yhk)

/* DriverLib Includes */
#include "driverlib.h"


/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* Project Includes */
#include "Clock.h"
#include "TimerA1.h"
#include "gpio.h"
#include "LaunchPad.h"
#include "PushButton.h"
#include "LED.h"
#include "PortPins.h"
#include "Switch.h"
#include "Bump.h"
#include "Motor.h"
#include "FSM.h"
#include "FSM2.h"
#include "Reflectance.h"
#include "msp.h"
#include "UART0.h"
#include "Nokia5110.h"
#include "Booster_Pack/bme280_support.h"
#include "Booster_Pack/bmi160_support.h"
#include "Booster_Pack/opt3001.h"
#include "Booster_Pack/i2c_driver.h"
#include <math.h>

/*
// bitmap of an enemy from Space Invaders
const uint8_t Enemy[] = {
                         0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
                         0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
                         0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F,
                         0x0F, 0x00, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
                         0xFF, 0x0F, 0xF0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
};


void main(void){
    Clock_Init();
    Nokia5110_Init();
    while(1){
        Nokia5110_Clear();
        Nokia5110_SetCursor(0, 1);
        Nokia5110_OutString("Hello world");
    }
}
*/

// Calibration off-sets
int8_t accel_off_x;
int8_t accel_off_y;
int8_t accel_off_z;
s16 gyro_off_x;
s16 gyro_off_y;
s16 gyro_off_z;

// BMI160
BMI160_RETURN_FUNCTION_TYPE returnValue;
struct bmi160_gyro_t        s_gyroXYZ;
struct bmi160_accel_t       s_accelXYZ;
struct bmi160_mag_xyz_s32_t s_magcompXYZ;

// Variables
uint32_t counter = 0;
FSMType Timer_FSM;
FSM2Type Sensor_FSM2;
uint8_t a;
uint8_t b;
uint8_t conditionSpeed;
uint16_t rawData;
float light;
float convertedLight;

void Task(void){
    // On every tenth interrupt, starting with the first, ISR calls Refecltance_Start()
    if (counter%10 == 1){
        Reflectance_Start();
    }
    // On every tenth interrupt, starting with the second, IRS calls Reflectance_End()
    else if(counter%10 == 2){
        Timer_FSM.i2 = Reflectance_End();
        b = Reflectance_End(); // Stores data in a shared global variable
    }
    // On every tenth interrupt, starting with the third, IRS calls Bump_Read()
    else if(counter%10 == 3){
        Timer_FSM.i1 = Bump_Read();
        a = Bump_Read(); // Stores data in a shared global variable
    }
    // On remaining interrupts, counter is incremented
    counter++;
}

int main(void){

    // Initialize switch buttons (S1 and S2)
    SwitchDefine PushButtonS1, PushButtonS2;
    InitializePushButtonPortPins();
    InitializeSwitch(&PushButtonS1, (uint8_t *)&(PUSHBUTTON_S1_PORT->IN), (uint8_t) PUSHBUTTON_S1_BIT, Active, Inactive);
    InitializeSwitch(&PushButtonS2, (uint8_t *)&(PUSHBUTTON_S2_PORT->IN), (uint8_t) PUSHBUTTON_S2_BIT, Active, Inactive);

    // Initializations
    Clock_Init();
    Reflectance_Init();
    Bump_Init();
    LaunchPad_Init();
    Motor_Init();
    Nokia5110_Init();
    TimerA1_Init(&Task, 499);
    WDT_A_holdTimer();
    initI2C();
    sensorOpt3001Init();
    bmi160_initialize_sensor();

    // Initalize Finite State Machines
    InitializeFSM(&Timer_FSM);
    Initialize2FSM(&Sensor_FSM2);

    // Set mode for BMI sensor
    int returnRslt = bmi160_config_running_mode(APPLICATION_NAVIGATION);
    // int return Rslt = bmi160_set_foc_gyro_enable(0x01, &gyro_off_x, &gyro_off_y, &gyro_off_z);

    // Main program doesn't run until Pushbutton S1 is pressed
    while(ReadSwitchStatus(&PushButtonS1)==Inactive){

    }

    // Main program is run until Pushbutton S2 is pressed
    while(ReadSwitchStatus(&PushButtonS2)==Inactive){
        // Define current state of each finite state machine (FSM.c and FSM2.c)
        Timer_FSM.CurrentState = NextStateFunction(&Timer_FSM);
        Sensor_FSM2.CurrentState = NextState2Function(&Sensor_FSM2);

        // Define relevant variables for robot acceleration value
        uint8_t bumpi1 = Bump_Read();
        uint8_t robotUp;
        float convertedLux;
        sensorOpt3001Read(&rawData);
        sensorOpt3001Convert(rawData, &convertedLux);
        light = convertedLux;

        // Output robot acceleration value
        bmi160_read_accel_xyz(&s_accelXYZ);

        // Determine if robot is picked up or not using acceleration value
        if (s_accelXYZ.x > 4000 | s_accelXYZ.y > 4000 | s_accelXYZ.y < -4000 | s_accelXYZ.x < -4000){
            robotUp = 1; // Robot is picked up
        } else {
            robotUp = 0; // Robot is not picked up
        }

        // Set values to variables to be used in Sensor_FSM2 (from FSM2.c)
        Sensor_FSM2.robotUp = robotUp;
        Sensor_FSM2.bumpi1 = a;
        Sensor_FSM2.brightness = light;

        // Current state of Sensor_FSM2 (from FSM2.c)
        Sensor_FSM2.CurrentState = NextState2Function(&Sensor_FSM2);

        // Output of Sensor_FSM (FSM2.c) set to variable
        conditionSpeed = Output2Function(&Sensor_FSM2);

        // Output function of Timer_FSM (FSM.c) using output of Sensor_FSM above (conditionSpeed)
        OutputFunction(&Timer_FSM, conditionSpeed);

        // Display current conditions on the LCD (Normal, TakeItSlow, Stop)
        Nokia5110_Clear();
        Nokia5110_SetCursor(0, 1);
        if(Sensor_FSM2.CurrentState == TakeItSlow){
            Nokia5110_OutChar('T');
            Nokia5110_OutChar('a');
            Nokia5110_OutChar('k');
            Nokia5110_OutChar('e');
            Nokia5110_OutChar('I');
            Nokia5110_OutChar('t');
            Nokia5110_OutChar('S');
            Nokia5110_OutChar('l');
            Nokia5110_OutChar('o');
            Nokia5110_OutChar('w');
        }
        else if(Sensor_FSM2.CurrentState == Stop){
            Nokia5110_OutChar('S');
            Nokia5110_OutChar('t');
            Nokia5110_OutChar('o');
            Nokia5110_OutChar('p');
        }
        else{
            Nokia5110_OutChar('N');
            Nokia5110_OutChar('o');
            Nokia5110_OutChar('r');
            Nokia5110_OutChar('m');
            Nokia5110_OutChar('a');
            Nokia5110_OutChar('l');
        }

        // Display line sensor reading on LCD (Center, Left, Right, Lost)
        Nokia5110_SetCursor(0, 3);
        if(Timer_FSM.i2 == ON_LINE){
            Nokia5110_OutChar('C');
            Nokia5110_OutChar('e');
            Nokia5110_OutChar('n');
            Nokia5110_OutChar('t');
            Nokia5110_OutChar('e');
            Nokia5110_OutChar('r');
        }
        else if(Timer_FSM.i2 == LEFT){
            Nokia5110_OutChar('L');
            Nokia5110_OutChar('e');
            Nokia5110_OutChar('f');
            Nokia5110_OutChar('t');
        }
        else if(Timer_FSM.i2 == RIGHT){
            Nokia5110_OutChar('R');
            Nokia5110_OutChar('i');
            Nokia5110_OutChar('g');
            Nokia5110_OutChar('h');
            Nokia5110_OutChar('t');
        }
        else if(Timer_FSM.i2 == LOST){
            Nokia5110_OutChar('L');
            Nokia5110_OutChar('o');
            Nokia5110_OutChar('s');
            Nokia5110_OutChar('t');
        }

        }

    // Main program is terminated and motor is stopped after Pushbutton S2 is pressed
    Motor_Stop();
}
