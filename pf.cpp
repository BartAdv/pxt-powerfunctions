#include "pxt.h"
#include "pf.h"

// Lego Power Functions Infrared Control for PXT
// Based on https://github.com/jurriaan/Arduino-PowerFunctions
// and on https://github.com/philipphenkel/pxt-powerfunctions


// Constructor
PowerFunctions::PowerFunctions(uint8_t pin)
{
  _toggle = 0;
  _pin = lookupPin(pin);

  if(_pin) {
    //pinMode(_pin, OUTPUT);
    _pin->setDigitalValue(0);
  }

}

// Single output mode PWM
void PowerFunctions::single_pwm(uint8_t output, uint8_t channel, uint8_t pwm) {
  send(channel, _toggle | channel, SINGLE_OUTPUT | output, pwm);
  toggle();
}

void PowerFunctions::single_increment(uint8_t output, uint8_t channel){
  send(channel, _toggle | channel, SINGLE_EXT | output, 0x4);
  toggle();
}

void PowerFunctions::single_decrement(uint8_t output, uint8_t channel){
  send(channel, _toggle | channel, SINGLE_EXT | output, 0x5);
  toggle();
}

// Combo PWM mode
void PowerFunctions::combo_pwm(uint8_t blue_speed, uint8_t red_speed, uint8_t channel)
{
  send(channel, ESCAPE | channel, blue_speed, red_speed);
}

//
// Private methods
//

// Pause function see "Transmitting Messages" in Power Functions PDF
void PowerFunctions::pause(uint8_t channel, uint8_t count)
{
  uint8_t pause = 0;

  if(count == 0) {
    pause = 4 - (channel + 1);
  } else if(count < 3) { // 1, 2
    pause = 5;
  } else {  // 3, 4, 5
    pause = 5 + (channel + 1) * 2;
  }
  sleep_us(pause * 77); //MAX_MESSAGE_LENGTH
}

// Send the start/stop bit
void PowerFunctions::start_stop_bit()
{
  send_bit();
  sleep_us(START_STOP); // Extra pause for start_stop_bit
}

// Send a bit
void PowerFunctions::send_bit() {
  for(uint8_t i = 0; i < 6; i++) {
    _pin->setDigitalValue(1);
    sleep_us(HALF_PERIOD);
    _pin->setDigitalValue(0);
    sleep_us(HALF_PERIOD);
  }
}

void PowerFunctions::send(uint8_t channel, uint8_t nib1, uint8_t nib2, uint8_t nib3)
{
  uint8_t i, j;

  uint16_t message = nib1 << 12 | nib2 << 8 | nib3 << 4 | (0xf ^ nib1 ^ nib2 ^ nib3);
  for(i = 0; i < 6; i++)
  {
    pause(channel, i);
    start_stop_bit();
    for(j = 0; j < 16; j++) {
      send_bit();
      sleep_us((0x8000 & (message << j)) != 0 ? HIGH_PAUSE : LOW_PAUSE);
    }
    start_stop_bit();
  } // for
}

inline void PowerFunctions::toggle(){
  _toggle ^= 0x8;
}

enum class PowerFunctionsMotor {
    //% block="red | channel 1"
    Red1 = 0,
    //% block="red | channel 2"
    Red2 = 1,
    //% block="red | channel 3"
    Red3 = 2,
    //% block="red | channel 4"
    Red4 = 3,
    //% block="blue | channel 1"
    Blue1 = 4,
    //% block="blue | channel 2"
    Blue2 = 5,
    //% block="blue | channel 3"
    Blue3 = 6,
    //% block="blue | channel 4"
    Blue4 = 7
};

uint8_t getOutput(PowerFunctionsMotor motor) {
  switch(motor) {
  case PowerFunctionsMotor::Red1:
  case PowerFunctionsMotor::Red2:
  case PowerFunctionsMotor::Red3:
  case PowerFunctionsMotor::Red4:
    return RED;
  case PowerFunctionsMotor::Blue1:
  case PowerFunctionsMotor::Blue2:
  case PowerFunctionsMotor::Blue3:
  case PowerFunctionsMotor::Blue4:
    return BLUE;
  }
}

uint8_t getChannel(PowerFunctionsMotor motor) {
  switch(motor) {
  case PowerFunctionsMotor::Red1: return 0;
  case PowerFunctionsMotor::Red2: return 1;
  case PowerFunctionsMotor::Red3: return 2;
  case PowerFunctionsMotor::Red4: return 3;

  case PowerFunctionsMotor::Blue1: return 0;
  case PowerFunctionsMotor::Blue2: return 1;
  case PowerFunctionsMotor::Blue3: return 2;
  case PowerFunctionsMotor::Blue4: return 3;
  }
}

namespace powerfunctions {

class PfWrap : public PowerFunctions {
public:
  PfWrap() : PowerFunctions(PIN(IR_OUT)) {
  }
};
SINGLETON(PfWrap);

  /**
   * Move a motor forward.
   */
  //% blockId=pf_move_forward
  //% block="move forward | with motor %motor at speed %speed"
  //% weight=100
  //% motor.fieldEditor="gridpicker" motor.fieldOptions.columns=4 motor.fieldOptions.tooltips="false"
  //% speed.min=1 speed.max=7
  void moveForward(PowerFunctionsMotor motor, uint8_t speed) {
    // TODO: clamp speed ?
    auto pf = getPfWrap();
    pf->single_pwm(getOutput(motor), getChannel(motor), speed);
  }

  /**
   * Move a motor backward.
   */
  //% blockId=pf_move_backward
  //% block="move backward | with motor %motor at speed %speed"
  //% weight=90
  //% motor.fieldEditor="gridpicker" motor.fieldOptions.columns=4 motor.fieldOptions.tooltips="false"
  //% speed.min=1 speed.max=7
  void moveBackward(PowerFunctionsMotor motor, uint8_t speed) {
    // TODO: clamp speed ?
    auto pf = getPfWrap();
    pf->single_pwm(getOutput(motor), getChannel(motor), 0xf + 1 - speed);
  }

  /**
   * Brake then float.
   * The motor's power is quickly reversed and thus the motor will stop abruptly.
   */
  //% blockId=powerfunctions_brake
  //% block="brake| motor %motor"
  //% weight=80
  //% motor.fieldEditor="gridpicker" motor.fieldOptions.columns=4 motor.fieldOptions.tooltips="false"
  void brake(PowerFunctionsMotor motor) {
    auto pf = getPfWrap();
    pf->single_pwm(getOutput(motor), getChannel(motor), PWM_BRK);
  }

  /**
   * Float a motor to stop.
   * The motor's power is switched off and thus the motor will roll to a stop.
   */
  //% blockId=pf_leeway
  //% block="float | motor %motor | to stop"
  //% weight=70
  //% motor.fieldEditor="gridpicker" motor.fieldOptions.columns=4 motor.fieldOptions.tooltips="false"
  void leeway(PowerFunctionsMotor motor) {
    auto pf = getPfWrap();
    pf->single_pwm(getOutput(motor), getChannel(motor), PWM_FLT);
  }
}
