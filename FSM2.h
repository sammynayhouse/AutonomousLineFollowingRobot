// Sammy Nayhouse (san6yhk)

#ifndef FSM2_H_
#define FSM2_H_

#include "FSM.h"
#include <stdint.h>

// Type Definitions
typedef enum {
    NormalOperation,
    TakeItSlow,
    Stop
} FSM2State;

typedef struct {
    FSM2State   CurrentState;      // Current state of the FSM
    uint8_t     bumpi1;    // Current input of the FSM S1
    uint8_t     robotUp;    // Current input of the FSM S2
    float       brightness;    // Current input of the FSM S3
} FSM2Type;

// Function Prototypes
void Initialize2FSM(FSM2Type *FSM2);
FSM2State NextState2Function(FSM2Type *FSM2);
uint8_t Output2Function(FSM2Type *FSM2);

#endif /* FSM_H_2_ */
