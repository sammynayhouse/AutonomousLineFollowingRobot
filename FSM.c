// Sammy Nayhouse (san6yhk)

#include "FSM.h"
#include "Clock.h"
#include "Reflectance.h"
#include "GPIO.h"
#include "Motor.h"

//--------------------------------------------------------------------------
// Initialize FSM
//--------------------------------------------------------------------------
void InitializeFSM(FSMType *FSM) {
    FSM->CurrentState = Init;
}
//--------------------------------------------------------------------------
// Determine next FSM state
//--------------------------------------------------------------------------
    FSMState NextStateFunction(FSMType *FSM)
    {
        FSMState NextState = FSM->CurrentState;
        switch (FSM->CurrentState ){
        case Init:
            if(FSM-> i1 > 0){
                NextState = Bump;
            }
            else if(FSM->i2 == LEFT){
                NextState = Left;
            }
            else if(FSM->i2 == RIGHT){
                NextState = Right;
            }
            else if(FSM->i2 == ON_LINE){
                NextState = Center;
            }
            else if(FSM->i2 == LOST){
                NextState = Lost;
            }
            break;
        case Left:
            if(FSM->i1 > 0){
                NextState = Bump;
            }
            else if(FSM->i2 == ON_LINE){
                NextState = Center;
            }
            else if(FSM->i2 == RIGHT){
                NextState = Right;
            }
            else if(FSM->i2 == LOST) {
                NextState = HardLeft;
            }
            break;
        case Center:
            if(FSM->i1 > 0 ){
                NextState = Bump;
            }
            else if(FSM->i2 == LEFT){
                NextState = Left;
            }
            else if(FSM->i2 == RIGHT){
                NextState = Right;
            }
            else if(FSM->i2 == LOST) {
                NextState = HardLeft; // if robot gets to end of line, it will turn around and go back
            }
            break;
        case Right:
            if(FSM->i1 > 0 ){
                NextState = Bump;
            }
            else if(FSM->i2 == ON_LINE){
                NextState = Center;
            }
            else if(FSM->i2 == LEFT){
                NextState = Left;
            }
            else if(FSM->i2 == LOST){
                NextState = HardRight;
            }
            break;

        case HardRight:
            if(FSM->i1 > 0 ){
                NextState = Bump;
            }
            else if(FSM->i2 == RIGHT){
                NextState = Right;
            }
            else if(FSM->i2 == ON_LINE){
                NextState = Center;
            }
            else if(FSM->i2 == LEFT){
                NextState = Left;
            }
            //Clock_Delay1ms(1000);
            NextState = LCRC;
            break;
        case HardLeft:
            if(FSM->i1 > 0 ){
                NextState = Bump;
            }
            else if(FSM->i2 == LEFT){
                NextState = Left;
            }
            else if(FSM->i2 == ON_LINE){
                NextState = Center;
            }
            else if(FSM->i2 == RIGHT){
                NextState = Right;
            }
            //Clock_Delay1ms(1000);
            NextState = LCRC;
            break;
        case LCRC:
            if(FSM->i1 > 0 ){
                NextState = Bump;
            }
            else if(FSM->i2 == LEFT){
                NextState = Left;
            }
            else if(FSM->i2 == ON_LINE){
                NextState = Center;
            }
            else if(FSM->i2 == RIGHT){
                NextState = Right;
            }
            else if(FSM->i2 == LOST){
                NextState = Lost;
            }
        case Lost:
            if(FSM->i1 > 0 ){
                NextState = Bump;
            }
            else if(FSM->i2 == LEFT){
                NextState = Left;
            }
            else if(FSM->i2 == RIGHT){
                NextState = Right;
            }
            else if(FSM->i2 == ON_LINE){
                NextState = Center;
            }
            break;
        case Bump:
            if(FSM-> i1 > 0){
                NextState = Bump;
            }
            else if(FSM->i2 == LEFT){
                NextState = Left;
            }
            else if(FSM->i2 == RIGHT){
                NextState = Right;
            }
            else if(FSM->i2 == ON_LINE){
                NextState = Center;
            }
            else if(FSM->i2 == LOST){
                NextState = Lost;
            }
            break;
    }
    return NextState;
    }
//--------------------------------------------------------------------------
// Determine LED output based on state
//--------------------------------------------------------------------------
void OutputFunction(FSMType *FSM, uint8_t conditionSpeed)
{
    switch (FSM->CurrentState) {
    case Init:
        /*
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN5);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN7);
        */
        Motor_Stop();
        break;

    case Center:
        /*
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN5);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN7);
        */
        if(conditionSpeed == 11){
            Motor_Forward(12,12);
        }
        else if(conditionSpeed == 0){
            Motor_Stop();
        }
        else{
            //Motor_Forward(24,24);
            //Motor_Forward(32,32);
            //Motor_Forward(35,35);
            Motor_Forward(35,35);
        }
        //Motor_Forward(robotSpeed, robotSpeed);
        break;

    case Left:
        /*
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN5);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN7);
        */
        if(conditionSpeed == 11){
            Motor_Right(11,11);
        }
        else if(conditionSpeed == 0){
            Motor_Stop();
        }
        else{
            Motor_Right(23,23);
        }
        //Motor_Left(robotSpeed, robotSpeed);
        break;

    case Right:
        /*
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN7);
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN5);
        */
        if(conditionSpeed == 11){
            Motor_Left(11,11);
        }
        else if(conditionSpeed == 0){
            Motor_Stop();
        }
        else{
            Motor_Left(23,23);
        }
        //Motor_Right(robotSpeed, robotSpeed);
        break;

    case HardRight:
        /*
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN6);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN7);
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN5);
        */
        if(conditionSpeed == 11){
            Motor_Left(11,11);
        }
        else if(conditionSpeed == 0){
            Motor_Stop();
        }
        else{
            Motor_Left(23,23);
        }
        //Motor_Right(robotSpeed, robotSpeed);
        Clock_Delay1ms(500);
        break;

    case HardLeft:
        /*
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN7);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN5);
        */
        if(conditionSpeed == 11){
            Motor_Right(11,11);
        }
        else if(conditionSpeed == 0){
            Motor_Stop();
        }
        else{
            Motor_Right(23,23);
        }
        //Motor_Left(robotSpeed, robotSpeed);
        Clock_Delay1ms(500);
        break;

    case LCRC:
        /*
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN6);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN7);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN5);
        */
        break;

    case Lost:
        /*
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN5);
        GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN6);
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN7);
        */
        Motor_Stop();
        break;

    case Bump:
        /*
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN5);
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN6);
        GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN7);
        */
        Motor_Stop();
        break;
    }

}

