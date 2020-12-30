#include "stateMachine.h"

StateMachine SM;

void setup() {
  SM.init();
}

void loop() {
  SM.fsm();
}
