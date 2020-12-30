#include "stateMachine.h"
#include "stepperMotor.h"
#include "PIO.h"
#include "config.h"
#include "src/AD7190/AD7190_cell.h"
#include "comms.h"

StepperMotor motor1;
Limitswitch limitSwitch;
AD7190 loadcell(LOAD_CELL_CS);
Communications Comms; 

void StateMachine::fsm(void){
    switch(state){
        case Init:
            init();
        break;
        case Setup:
            //TODO;
            break;
        case Calibrate:
            calibratePosition();
        break;
        case Idle:
            loadcell.update();
            loadcell.getWeight();
        break;
        case Run:
        break;
        case Error:
        break;
        default:
        break;
        
    }
}
void StateMachine::init(void){
    motor1.init(SM_STEP_PIN, SM_DIR_PIN);
    limitSwitch.init(LS_STEPPER_CALIBRATE_PIN);
    loadcell.init();
    state = Calibrate;   
}

void StateMachine::calibratePosition(void){
    if(MotorIsMoving == false && isCalibrated == false){
        if(limitSwitch.read(LS_STEPPER_CALIBRATE_PIN)){
            motor1.setSpeed(SM_SPEED);
            motor1.gotoPosition(HOME_POSITION);
            MotorIsMoving = true;
        }
        else{
            hasHitLimitSwitch = false;
            motor1.setSpeed(SM_SPEED);
            motor1.gotoPosition(INT32_MAX);
            MotorIsMoving = true;
        }
    }
    else if(MotorIsMoving == true && isCalibrated == false){
        if(hasHitLimitSwitch == false){
            if(limitSwitch.read(LS_STEPPER_CALIBRATE_PIN)){
                motor1.stop();
                MotorIsMoving = false;
                hasHitLimitSwitch = true;
            }
        }
        else{
            if(motor1.getPosition() >= HOME_POSITION){
                motor1.stop();
                MotorIsMoving = false;
                isCalibrated = true;
                state = Idle;
            }
        }
    }
    else{
        state = Error;
    }
}

void StateMachine::run(void){

}

void StateMachine::idle(void){

}