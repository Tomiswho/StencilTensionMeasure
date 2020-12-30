#include "src/AD7190/AD7190_cell.h"
#include "src/AccelStepper-1.61.0/AccelStepper.h"


#define PIN_SPI_LOADCELL_MISO 50
#define LOAD_CELL_CS 7
AD7190 LC(LOAD_CELL_CS);

void setup() {
  Serial.begin(9600);
  LC.init();
  Serial.println("Start");
}

void loop() {
  LC.update();
  Serial.println(LC.getWeight());
  // put your main code here, to run repeatedly:

}
