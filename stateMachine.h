#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include "stdint.h"

class StateMachine{
    public:

    private:
        enum states{
            Error,
            Init,
            Idle,
            Calibrate,
            Setup,
            Run
        };
        bool hasHitLimitSwitch = false;
        bool isCalibrated = false;
        bool MotorIsMoving = true;;
        int32_t currentPosition;
        states state = Init;
        int32_t calibDistance = 0;
        
    public:
        void fsm(void);
    private:
        void calibratePosition(void);
        void init(void);
        void run(void);
        void idle(void);

};
#endif
