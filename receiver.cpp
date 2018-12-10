#include "pxt.h"
#include "receiver.h"

namespace powerfunctions
{

  Receiver::Receiver(uint16_t id, int pin)
  {
    this->id = id;
    this->pin = lookupPin(pin);

    if (this->pin) {
      devMessageBus.listen(this->pin->id, DEVICE_PIN_EVT_PULSE_HI, this, &Receiver::pulseGap,
                           MESSAGE_BUS_LISTENER_IMMEDIATE);
      devMessageBus.listen(this->pin->id, DEVICE_PIN_EVT_PULSE_LO, this, &Receiver::pulseMark,
                           MESSAGE_BUS_LISTENER_IMMEDIATE);

      this->pin->getDigitalValue(); // ???
      this->pin->eventOn(DEVICE_PIN_EVENT_ON_PULSE);
    }
  }

  /*
    Quoting spec:

    To ensure correct detection of IR messages six 38 kHz cycles are transmitted as mark.
    Low bit consists of 6 cycles of IR and 10 “cycles” of pause, high bit of 6 cycles IR
    and 21 “cycles” of pause and start bit of 6 cycles IR and 39 “cycles” of pause.
  */

  void Receiver::pulseGap(Event ev)
  {
    int t = (int)ev.timestamp;

    if (this->receiveState == ReadData) {
      // time calculations performed without taking mark time into account
      // as this allows to just measure gaps. Mark time is checked in `pulseMark`
      if(t >= 316 - MARK_TIME && t <= 526 - MARK_TIME) {
        // low bit
        readBit(false);
      } else if (t >= 526 - MARK_TIME && t <= 947 - MARK_TIME) {
        // high bit
        readBit(true);
      } else if (t >= 947 - MARK_TIME && t <= 1579 - MARK_TIME) {
        // stop bit
        if (validateLRC()) {
          processMessage();
        } else {
          terminate();
        }
      } else {
        terminate();
      }
    } else {
      if(t >= 1579 - MARK_TIME) {
        this->receiveState = ReadData;
      } else {
        terminate();
      }
    }
  }

  void Receiver::pulseMark(Event ev)
  {
    int t = (int)ev.timestamp;

    // Check if mark time is alright.
    // just fail for short marks
    if(t < 100) {
      terminate();
    }
  }

  void Receiver::readBit(bool high)
  {
    if(high) {
      this->buffer |= (1 << this->bitIdx);
    }
    this->bitIdx++;
  }

  bool Receiver::validateLRC() {
    uint16_t msg = this->buffer;

    uint8_t nib1 = msg >> 12 & 0xf;
    uint8_t nib2 = msg >> 8 & 0xf;
    uint8_t nib3 = msg >> 4 & 0xf;
    uint8_t lrc = msg & 0xf;

    return (0xf ^ nib1 ^ nib2 ^ nib3) == lrc;
  }

  void Receiver::processMessage() {
    // Check if newly arrived message differs from last one
    // as to raise event only once, and not for repeats (defined by protocol).
    // if it's the same message send after some time, the `decodedMessage` is gonna to be zeroed
    // so condition should still hold.
    if(this->decodedMessage != this->buffer) {
      Event ev(this->id, MESSAGE_EVENT);
    }
    this->decodedMessage = this->buffer;
    this->buffer = 0;
  }

  void Receiver::terminate()
  {
    if(this->receiveState != WaitStart) {
      this->receiveState = WaitStart;
      this->decodedMessage = this->buffer = 0;
      this->bitIdx = 0;
    }
  }
}
