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
        states state = Init;
        int32_t calibDistance = 0;
        float weight = 0;
        //EEPROM
        int16_t tensionPositionAddr0 = 0;
        int16_t tensionPositionAddr1 = 1;
        int16_t tensionPositionAddr2 = 2;
        int16_t tensionPositionAddr3 = 3;
        int32_t tensionPosition = 0;
        int16_t tensionSetisTrueAddr = 4;
        int16_t tensionSetisTrue = 0;
        
        
    public:
        void fsm(void);
        void init(void);
    private:
        void calibratePosition(void);

        void run(void);
        void idle(void);
        void commandList(void);

        void setTensionPosition(void);
        void MoveStepper(int32_t steps);
        void getForce(void);
        void getTension(void);

};
#endif
