// Sammy Nayhouse (san6yhk)

/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* Project Includes */
#include "TimerA1.h"

// Configuration parameter for TimerA1
Timer_A_UpModeConfig Config =
{
TIMER_A_CLOCKSOURCE_SMCLK,
TIMER_A_CLOCKSOURCE_DIVIDER_24,
0,
TIMER_A_TAIE_INTERRUPT_DISABLE,
TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,
TIMER_A_DO_CLEAR
};

// Pointer to user function that gets called on timer interrupt
void (*TimerA1Task)(void);

// Initializes Timer A1 in up mode and triggers a periodic interrupt at a desired frequency
void TimerA1_Init(void(*task)(void), uint16_t period){
    TimerA1Task = task; // Set to task in order to call function in ISR
    Config.timerPeriod = period; // Set timer period
    Timer_A_configureUpMode(TIMER_A1_BASE, &Config); // Initialize TimerA1 base module in Up mode
    Interrupt_enableInterrupt(INT_TA1_0); // Enable timer interrupt
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE); // Start the timer's counter
}

// Stops Timer A1 (changes timer module from Up mode to Stop mode, leaving all other configuration options unchanged)
void TimerA1_Stop(void){
    TIMER_A1->CTL & 0X0030;
}

// ISR function for Timer A1 periodic interrupt
void TA1_0_IRQHandler(void){
    (*TimerA1Task)(); // Calls pointer to user function
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0); // Clears the capture-compare interrupt flag

}
