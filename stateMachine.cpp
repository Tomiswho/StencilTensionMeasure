#include "stateMachine.h"
#include "stepperMotor.h"
#include "PIO.h"
#include "config.h"
#include "src/AD7190/AD7190_cell.h"
#include "comms.h"
//#include <EEPROM.h>

StepperMotor motor1;
Limitswitch limitSwitch;
AD7190 loadcell(LOAD_CELL_CS);
Communications Comms; 

void StateMachine::fsm(void){
    motor1.Update();
    Comms.checkCommand();
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
            idle();
        break;
        case Run:
            run();
        break;
        case Error:
          Serial.println("ERROR");
        break;
        default:
        break;
        
    }
}

void StateMachine::init(void){
    Comms.init(BAUD_RATE);
    motor1.init(SM_STEP_PIN, SM_DIR_PIN);
    limitSwitch.init(LS_STEPPER_CALIBRATE_PIN);
    loadcell.init();
    //tensionSetisTrue = EEPROM.read(tensionSetisTrueAddr);
    //tensionPosition = EEPROM.read(tensionPositionAddr);
    tensionPosition = TENSIONPOSITION;
    motor1.setSpeed(SM_SPEED);
    state = Calibrate;  
}

void StateMachine::calibratePosition(void){
    if(!limitSwitch.read(LS_STEPPER_CALIBRATE_PIN) && hasHitLimitSwitch == false && isCalibrated == false){
        motor1.stop();
        motor1.resetPosition();
        motor1.setSpeed(SM_SPEED);
        motor1.gotoPosition(HOME_POSITION);
        hasHitLimitSwitch = true;
    }
    else if(motor1.isMoving == false && hasHitLimitSwitch == false && isCalibrated == false){
        motor1.setSpeed(SM_SPEED);
        motor1.gotoPosition(INT32_MIN);
    }
    else if(hasHitLimitSwitch == true){
        if(motor1.getPosition() == motor1.targetPos){
            isCalibrated = true;
            motor1.stop();
            state = Idle;
        }
    }
}

void StateMachine::commandList(void){
    switch(Comms.inputCommands){
        case SetPosition:
            MoveStepper(Comms.arg1);
            break;
        case Tare:
            loadcell.tare();
            break;
        case GetWeight:
            loadcell.update();
            Serial.println(loadcell.getWeight());
            break;
        case MeasureTension:
            state = Run;
            break;
        case SetTension:
            setTensionPosition();
            break;
        case Home:
            MoveStepper(HOME_POSITION);
            break;
        case GetPosition:
            Serial.println("getPosition");
            Serial.println(motor1.getPosition());
            break;
        default:
        break;
    }
}

void StateMachine::run(void){
    MoveStepper(tensionPosition);
    if(motor1.getPosition() == motor1.targetPos){
        isCalibrated = true;
        motor1.stop();
        state = Idle;
    }
}

void StateMachine::idle(void){
    if(Comms.commandisTrue){
        commandList();
        Comms.commandisTrue = false;
    }
    else{
        if(motor1.isMoving == false){
          getTension();
        }
    }
}

void StateMachine::MoveStepper(int32_t steps){
    motor1.gotoPosition(steps);
}

void StateMachine::setTensionPosition(void){
    //tensionSetisTrue = EEPROM.update(tensionSetisTrueAddr, 1);
    //tensionPosition = EEPROM.update(tensionPositionAddr, (motor1.getPosition() << 0);
}

void StateMachine::getForce(void){
    loadcell.update();
    weight = loadcell.getWeight();
    Serial.println(weight);
}

void StateMachine::getTension(void){
    loadcell.update();
    weight = loadcell.getWeight();
    Serial.print("Weight: ,");
    Serial.println(weight);
    if(weight >= MAX_LOAD){
        motor1.stop();
        state = Error;
    }
}
