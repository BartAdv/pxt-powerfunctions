#ifndef PF_RECEIVER_H
#define PF_RECEIVER_H

#define MARK_TIME 158
#define MESSAGE_EVENT 0x1

namespace powerfunctions {

  enum ReceiveState : uint8_t {
    WaitStart = 0,
    ReadData = 1
  };

  class Receiver
  {
  public:
    Receiver(uint16_t id, int pin);

    // TODO: fail when not received?
    uint16_t getDecodedMessage() { return this->decodedMessage; }

  private:
    uint16_t id;
    DevicePin* pin;

    ReceiveState receiveState;
    int bitIdx;
    uint16_t buffer;
    uint16_t decodedMessage;

    void pulseGap(Event ev);
    void pulseMark(Event ev);
    void readBit(bool high);
    bool validateLRC();
    void processMessage();
    void terminate();

  };

}
#endif