#ifndef STEPPERMOTOR_H_
#define STEPPERMOTOR_H_

#include "stdint.h"
class StepperMotor{
    public:
        bool isMoving = false;
        int32_t currentPos = 0;
        int32_t targetPos = 0;
        int32_t prevTime = 0;
        int32_t pulseState = 0;
        int16_t stepPin;
        int16_t dirPin;
        int32_t speed = 0;
        bool direction = false;
    public:
        void init(uint8_t step, uint8_t dir);
        void gotoPosition(int32_t target);
        void setSpeed(int32_t speed);
        void stop(void);
        int32_t getPosition(void);
        bool Update(void);
};
#endif
