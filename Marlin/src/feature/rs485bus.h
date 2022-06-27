#pragma once

#if ENABLED(RS485_ENABLE)

#include <stdint.h>

#include "Stream.h"

#define RS485_BAUD_RATE 57600

class RS485Bus {
  public:
    RS485Bus(HardwareSerial *serial, uint16_t rxEnablePin, uint16_t txEnablePin);

    void init();
    void reset();
    int push(unsigned char byte);
    int send();
    void receive();
    int available();

  private:
    HardwareSerial *serial;
    uint8_t buffer_s = 0;

    uint8_t buffer[RS485BUS_BUFFER_SIZE];

    uint16_t rx_enable_pin;
    uint16_t tx_enable_pin;
};

extern RS485Bus rs485Bus;

#endif