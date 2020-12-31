#include "stepperMotor.h"
#include <Arduino.h>

void StepperMotor::init(uint8_t step, uint8_t dir){
    pinMode(step, OUTPUT);
    pinMode(dir, OUTPUT);
    stepPin = step;
    dirPin = dir;
}

void StepperMotor::gotoPosition(int32_t target){
    targetPos = target;
    if(isMoving == false && currentPos != targetPos){
        if(target <= currentPos){
            direction = true;
        }
        else{
            direction = false;
        }
        prevTime = micros();
    }
}

void StepperMotor::setSpeed(int32_t sp){
    speed = sp;
}

void StepperMotor::stop(void){
    targetPos = currentPos;
    isMoving = false;
}

int32_t StepperMotor::getPosition(void){
    return currentPos;
}

bool StepperMotor::Update(void){
    if(targetPos != currentPos){
        isMoving = true;
        if(micros() - prevTime >= speed){
            digitalWrite(dirPin, direction);
            if(pulseState == 0){
                digitalWrite(stepPin, HIGH);
                pulseState = 1;
            }
            else{
                digitalWrite(stepPin, LOW);
                pulseState = 0;
            }
            if(direction == true){
               currentPos--;
            }
            else{
               currentPos++;
            }
            prevTime = micros();
            return true;
        }
    }
    else{
        isMoving = false;
        return false;
    }
}

void StepperMotor::resetPosition(void){
  currentPos = 0;
  targetPos = 0;
}
