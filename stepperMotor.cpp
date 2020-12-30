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
        if(target <= 0){
            direction = false;
        }
        else{
            direction = true;
        }
        prevTime = millis();
    }
}

void StepperMotor::setSpeed(int32_t sp){
    speed = sp;
}

void StepperMotor::stop(void){
    targetPos = currentPos;
}

int32_t StepperMotor::getPosition(void){
    return currentPos;
}

bool StepperMotor::Update(void){
    if(targetPos != currentPos){
        isMoving = true;
        if(millis() - prevTime >= speed){
            digitalWrite(dirPin, direction);
            if(pulseState = 0){
                digitalWrite(stepPin, HIGH);
                pulseState = 1;
            }
            else{
                digitalWrite(stepPin, LOW);
                pulseState = 0;
                if(direction == true){
                    currentPos++;
                }
                else{
                    currentPos--;
                }
            }
            prevTime = millis();
            return true;
        }
    }
    else{
        return false;
    }

}