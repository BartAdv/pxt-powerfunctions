#include "sender.h"
#include "receiver.h"

#define PF_RECEIVER_COMPONENT_ID 0x666

namespace powerfunctions {

class TheSender : public Sender {
public:
  TheSender() : Sender(PIN(IR_OUT)) {
  }
};
SINGLETON(TheSender);

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
    auto pf = getTheSender();
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
    auto pf = getTheSender();
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
    auto pf = getTheSender();
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
    auto pf = getTheSender();
    pf->single_pwm(getOutput(motor), getChannel(motor), PWM_FLT);
  }

  //////////////////
  // Receiving
  /////////////////

  class TheReceiver : public Receiver {
  public:
    TheReceiver() : Receiver(PF_RECEIVER_COMPONENT_ID, PIN(IR_IN)) {
    }
  };
  SINGLETON(TheReceiver);

  /**
   * Run action after a PF message is recieved.
   */
  //% parts="pf"
  void onPFMessageRaw(Action body) {
    getTheReceiver();
    registerWithDal(PF_RECEIVER_COMPONENT_ID, MESSAGE_EVENT, body);
  }


  /**
   * Get most recent message.
   */
  //% parts="pf"
  uint16_t pfMessage() {
    auto w = getTheReceiver();
    return w->getDecodedMessage();
  }

  /**
   * for debugging
   */
  //% parts="pf"
  void writeDmesg() {
    pxt::dumpDmesg();
  }
}
