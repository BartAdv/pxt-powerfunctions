#include "pxt.h"
#include "sender.h"

// Lego Power Functions Infrared Control for PXT
// Based on https://github.com/jurriaan/Arduino-PowerFunctions
// and on https://github.com/philipphenkel/pxt-powerfunctions

namespace powerfunctions
{

// Constructor
Sender::Sender(uint8_t pin)
{
  _toggle = 0;
  _pin = lookupPin(pin);

  if(_pin) {
    //pinMode(_pin, OUTPUT);
    _pin->setDigitalValue(0);
  }

}

// Single output mode PWM
void Sender::single_pwm(uint8_t output, uint8_t channel, uint8_t pwm) {
  send(channel, _toggle | channel, SINGLE_OUTPUT | output, pwm);
  toggle();
}

void Sender::single_increment(uint8_t output, uint8_t channel){
  send(channel, _toggle | channel, SINGLE_EXT | output, 0x4);
  toggle();
}

void Sender::single_decrement(uint8_t output, uint8_t channel){
  send(channel, _toggle | channel, SINGLE_EXT | output, 0x5);
  toggle();
}

// Combo PWM mode
void Sender::combo_pwm(uint8_t blue_speed, uint8_t red_speed, uint8_t channel)
{
  send(channel, ESCAPE | channel, blue_speed, red_speed);
}

//
// Private methods
//

// Pause function see "Transmitting Messages" in Power Functions PDF
void Sender::pause(uint8_t channel, uint8_t count)
{
  uint8_t pause = 0;

  if(count == 0) {
    pause = 4 - (channel + 1);
  } else if(count < 3) { // 1, 2
    pause = 5;
  } else {  // 3, 4, 5
    pause = 6 + 2 * channel;
  }
  sleep_us(pause * 16000); //MAX_MESSAGE_LENGTH
}

// Send the start/stop bit
void Sender::start_stop_bit()
{
  send_bit();
  sleep_us(START_STOP); // Extra pause for start_stop_bit
}

// Send a bit
void Sender::send_bit() {
  for(uint8_t i = 0; i < 6; i++) {
    _pin->setDigitalValue(1);
    sleep_us(HALF_PERIOD);
    _pin->setDigitalValue(0);
    sleep_us(HALF_PERIOD);
  }
}

void Sender::send(uint8_t channel, uint8_t nib1, uint8_t nib2, uint8_t nib3)
{
  uint8_t i, j;

  uint16_t message = nib1 << 12 | nib2 << 8 | nib3 << 4 | (0xf ^ nib1 ^ nib2 ^ nib3);
  for(i = 0; i < 5; i++)
  {
    pause(channel, i);
    start_stop_bit();
    for(j = 0; j < 16; j++) {
      send_bit();
      sleep_us((0x8000 & (message << j)) != 0 ? HIGH_PAUSE : LOW_PAUSE);
    }
    start_stop_bit();
  }
  pause(channel, 5);
}

inline void Sender::toggle(){
  _toggle ^= 0x8;
}

}