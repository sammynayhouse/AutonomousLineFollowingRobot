// Sammy Nayhouse (san6yhk)

#include "FSM2.h"
#include "Clock.h"
#include "Reflectance.h"
#include "GPIO.h"
#include "Motor.h"

//--------------------------------------------------------------------------
// Initialize FSM
//--------------------------------------------------------------------------
void Initialize2FSM(FSM2Type *FSM2){
    FSM2->CurrentState = NormalOperation;
}
//--------------------------------------------------------------------------
// Determine next FSM state
//--------------------------------------------------------------------------
FSM2State NextState2Function(FSM2Type *FSM2)
    {
    FSM2State NextState = FSM2->CurrentState;
    switch (FSM2->CurrentState){
    case NormalOperation:
        // Transition to Stop when a bumper is hit, or the robot is picked up
        if (FSM2->bumpi1 > 0 || FSM2->robotUp==1)
            NextState = Stop;
        // Transition to TakeItSlow when the light is not bright but also not too dim (10-40)
        else if (FSM2->brightness < 40 && FSM2->brightness > 10 && FSM2->robotUp==0 )
            NextState = TakeItSlow;
        // Transition to Stop when it gets too dark (<10)
        else if (FSM2->brightness <= 10)
            NextState = Stop;
        // If none of the above conditions are met, stay in NormalOperation state
        else
            NextState = NormalOperation;
        break;
    case TakeItSlow:
        // Transition to Stop when a bumper is hit, or the robot is picked up
        if (FSM2->bumpi1 > 0 || FSM2->robotUp==1)
            NextState = Stop;
        // Transition to NormalOperation if the light is bright (>40)
        else if (FSM2->brightness >= 40 && FSM2->robotUp==0)
            NextState = NormalOperation;
        // Transition to Stop when it gets too dark (<10)
        else if (FSM2->brightness <= 10)
            NextState = Stop;
        // If none of the above conditions are met, stay in TakeItSlow state
        else
            NextState= TakeItSlow;
        break;
    case Stop:
        // Stay in the Stop state if a bumper is hit, the robot is picked up, or it is too dark (<10)
        if (FSM2->bumpi1 > 0 || FSM2->robotUp==1 || FSM2->brightness <= 10)
            NextState = Stop;
        // Transition to NormalOperation if the light is bright (>40)
        else if (FSM2->brightness >= 40)
            NextState = NormalOperation;
        // Transition to TakeItSlow when the light is not bright but also not too dim (10-40)
        else if (FSM2->brightness < 40 && FSM2->brightness > 10 && FSM2->robotUp==0 )
            NextState = TakeItSlow;
        break;
    }
    return NextState;
}

//--------------------------------------------------------------------------
// Determine LED output based on state
//--------------------------------------------------------------------------
uint8_t Output2Function(FSM2Type *FSM2)
{
    switch (FSM2->CurrentState) {
    case NormalOperation:
        return 23;
    case TakeItSlow:
        return 11;
    case Stop:
        return 0;
    }
    //return;
}


