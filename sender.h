#ifndef PF_SENDER_H
#define PF_SENDER_H

#include "pxt.h"

#define COMBO_DIRECT_MODE 0x01
#define SINGLE_PIN_CONTINUOUS 0x2
#define SINGLE_PIN_TIMEOUT 0x3
#define SINGLE_OUTPUT 0x4
#define SINGLE_EXT 0x6
#define ESCAPE 0x4

#define IR_CYCLES(num) (uint16_t) ((1.0/38000.0) * 1000 * 1000 * num)

#define START_STOP IR_CYCLES(39)
#define HIGH_PAUSE IR_CYCLES(21)
#define LOW_PAUSE IR_CYCLES(10)
#define HALF_PERIOD IR_CYCLES(0.5)
#define MAX_MESSAGE_LENGTH IR_CYCLES(522) // 2 * 45 + 16 * 27

//PWM speed steps
#define PWM_FLT 0x0
#define PWM_FWD1 0x1
#define PWM_FWD2 0x2
#define PWM_FWD3 0x3
#define PWM_FWD4 0x4
#define PWM_FWD5 0x5
#define PWM_FWD6 0x6
#define PWM_FWD7 0x7
#define PWM_BRK 0x8
#define PWM_REV7 0x9
#define PWM_REV6 0xA
#define PWM_REV5 0xB
#define PWM_REV4 0xC
#define PWM_REV3 0xD
#define PWM_REV2 0xE
#define PWM_REV1 0xf

//speed
/*
#define RED_FLT 0x0
#define RED_FWD 0x1
#define RED_REV 0x2
#define RED_BRK 0x3
#define BLUE_FLT 0x0
#define BLUE_FWD 0x4
#define BLUE_REV 0x8
#define BLUE_BRK 0xC
*/

//output
#define RED 0x0
#define BLUE 0x1

namespace powerfunctions {

class Sender
{
 public:
   Sender(uint8_t pin);
    void single_pwm(uint8_t, uint8_t, uint8_t);
    void single_increment(uint8_t, uint8_t);
    void single_decrement(uint8_t, uint8_t);
    void combo_pwm(uint8_t, uint8_t, uint8_t);

  private:
    void pause(uint8_t, uint8_t);
    void send_bit();
    void send(uint8_t, uint8_t, uint8_t, uint8_t);
    void start_stop_bit();

    void toggle();
    
    DevicePin *_pin;
    uint8_t _toggle;
    bool    _debug;
};


 // TODO: move to other file
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

}
#endif
// END OF FILE