// Sammy Nayhouse (san6yhk)

#ifndef FSM_H_
#define FSM_H_

#include "PortPins.h"
#include "Switch.h"
#include <stdint.h>

// Type Definitions
typedef enum {
    Init,
    Center,
    Left,
    Right,
    Lost,
    HardLeft,
    HardRight,
    LCRC,
    Bump
} FSMState;

typedef struct {
    FSMState     CurrentState;      // Current state of the FSM
    uint8_t  i1;    // Current input of the FSM S1
    uint8_t  i2;    // Current input of the FSM S2
} FSMType;

// Function Prototypes
void InitializeFSM(FSMType *FSM);
FSMState NextStateFunction(FSMType *FSM);
void OutputFunction(FSMType *FSM, uint8_t conditionSpeed);

#endif /* FSM_H_ */

