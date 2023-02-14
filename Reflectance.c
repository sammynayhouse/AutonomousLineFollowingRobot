// Sammy Nayhouse (san6yhk)

/*
 * Reflectance.c
 * Provides functions to
 * 1. Initialize Pololu reflectance sensor
 * 2. Read Pololu reflectance sensor
 * 3. Determine robot position
 * 4. Determine robot status
 *
 */

#include <driverlib.h>
#include <stdint.h>
#include <msp.h>
#include "Clock.h"
#include "Reflectance.h"

#define ALLBITS  0xFF
#define BITSHIFT 0x01

//------------Reflectance_Init------------
// Initialize sensor array to GPIO, set LEDs (P5.3 and P9.2)
// as output and sensors (P7.0-P7.7) as output
// Input: none
// Output: none
void Reflectance_Init(void){
    // LED P5.3 set as output and set initially low
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);

    // LED P9.2 set as output and set initially low
    GPIO_setAsOutputPin(GPIO_PORT_P9, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P9, GPIO_PIN2);

    // Sensors P7.0-P7.7 set as outputs
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
}

//------------Reflectance_Read------------
// Read reflectance sensor
// Input: the delay time in us
// Output: result the sensor readings, bit 0 corresponds
//         to the rightmost sensor, bit 7 to the leftmost sensor
uint8_t Reflectance_Read(uint32_t time){
    // 1. Turn on even and odd LEDs
    // 2. Charge reflectance sensor capacitors (set as output and high on P7.0-P7.7)
    // 3. Wait 10us for capacitors to charge
    // 4. Set reflectance sensor (P7.0-P7.7) as input
    // 5. Wait @param time
    // 6. Read reflectance sensor values and assign to result
    // 8. Turn off even and odd LEDs

    // 1
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P9, GPIO_PIN2);

    // 2
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
    GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);

    // 3
    Clock_Delay1us(10);

    // 4
    GPIO_setAsInputPin(GPIO_PORT_P7, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);

    // 5
    Clock_Delay1us(time);

    // 6
    uint8_t pval = P7->IN;

    // 7
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P9, GPIO_PIN2);

    // 8
    return pval;

}

//------------Reflectance_Center------------
// Determine robot's status over the line
// Input: the delay time in us
// Output: result the robot location status (LOST/RIGHT/LEFT/ON LINE)
uint8_t Reflectance_Center(uint32_t time){
    //Get reflectance read data
    //Get value from left center sensor
    //Get value from right center sensor

    /* Check robot status truth table
     * INPUT (L,R) | OUTPUT
     * ------------|------------
     *      11     | ON_LINE (3)
     *      10     | LEFT    (2) // This should be 01? (if robot is on left, only right side should output 1)
     *      01     | RIGHT   (1) // This should be 10? (if robot is on right, only left side should output 1)
     *      00     | LOST    (0)
     */

    // Read the line sensor (get reflectance read data)
    uint8_t pval = Reflectance_Read(time);

    // Creating variables
    //uint8_t on_line = 3;
    //uint8_t left = 2;
    //uint8_t right = 1;
    //uint8_t lost = 0;

    // Series of if statements to determine robot's position
    if ((pval & 0x18) == 0x18) {
        return ON_LINE;
    }
    else if ((pval & 0x10) == 0x10) {
        return RIGHT;
    }
    else if ((pval & 0x8) == 0x8) {
        return LEFT;
    }
    else {
        return LOST;
    }
}

//------------Reflectance_Position------------
// Determine robot's status over the line
// Input: the collected sensor data
// Output: the position value between +345 (most left)
//         to -345 (most right)
int32_t Reflectance_Position(uint8_t data){
    // Define a weight vector
    int W_i[8] = {334, 238, 142, 48, -48, -142, -238, -334};

    // Define b_i to be 0 (white) or 1 (black) for each binary bit in data
    int b_i[8];
    int i;
    for(i=0; i<8;i++) {
        b_i[i] = data >> i;
    }

    // Sum of b_i and W_i
    int sumb_i = 0;
    int sumW_i = 0;
    for(i=0;i<8;i++) {
        sumb_i+=b_i[i];
        sumW_i+=b_i[i]*W_i[i];
    }

    // Check if b_i is 0
    if(sumb_i == 0){
        return 1000;
    }

    // Use the equation below to calculate and return d, distance
    int32_t d = sumW_i/sumb_i;
    return d;
}

/*
 * Start collecting reflectance sensor data by turning on the LEDs, charging the capacitors, waiting,
 * and then set the sensor pins as input
 * @param none
 * @return none
 */
void Reflectance_Start(void){
        // Turn on even and odd LEDs
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
        GPIO_setOutputHighOnPin(GPIO_PORT_P9, GPIO_PIN2);

        // Charge reflectance sensor capacitors (set as output and high on P7.0-P7.7)
        GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
        GPIO_setOutputHighOnPin(GPIO_PORT_P7, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);

        //Wait 10us for capacitors to charge
        Clock_Delay1us(10);

        // Set reflectance sensor (P7.0-P7.7) as input
        GPIO_setAsInputPin(GPIO_PORT_P7, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
}

/* Finish collecting reflectance sensor data by reading the result from the P7 input register and turning off the LEDs
 * @param none
 * @return result the sensor data
 */
uint8_t Reflectance_End(void){
    // Read result form P7 input register
    uint8_t pval = P7->IN;

    // Turn off LEDs
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P9, GPIO_PIN2);

    // Return the result of the sensor data
    if ((pval & 0x18) == 0x18) {
        return ON_LINE;
    }
    else if ((pval & 0x10) == 0x10) {
        return RIGHT;
    }
    else if ((pval & 0x8) == 0x8) {
        return LEFT;
    }
    else {
        return LOST;
    }

}
