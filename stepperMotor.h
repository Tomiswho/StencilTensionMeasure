#ifndef STEPPERMOTOR_H_
#define STEPPERMOTOR_H_
#include "stdint.h"
class StepperMotor{
    public:
        void init(uint8_t step, uint8_t dir);
        void gotoPosition(int32_t target);
        void setSpeed(int32_t speed);
        void stop(void);
        int32_t getPosition(void);
};
#endif
