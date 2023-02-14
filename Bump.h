// Sammy Nayhouse (san6yhk)

#ifndef BUMP_H
#define BUMP_H
#include <stdint.h>

/*
 * Initialize bump sensor pins as GPIO inputs with pull up resistors
 * Port 4, Pins 0, 2, 3, 5, 6, and 7
 * @param none
 * @return none
 */
void Bump_Init(void);

/*
 * Read the state of the bump switches
 * @param none
 * @return uint8_t variable, bits 0-5 correspond to
 * the values of bump switches 0-5 (right to left on the robot)
 */
uint8_t Bump_Read(void);

void Bump_Interrupt_Init(void(*task)(uint8_t));

void PORT4_IRQHandler(void);

#endif


