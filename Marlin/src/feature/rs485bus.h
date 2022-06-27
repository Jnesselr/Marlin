#pragma once

#include <stdint.h>

#include "SoftwareSerial.h"

class RS485Bus {
  public:
    RS485Bus(uint16_t rxPin, uint16_t rxEnablePin, uint16_t txPin, uint16_t txEnablePin);

    void init();
    void reset();
    int push(unsigned char byte);
    int send();
    int receive();

  private:
    SoftwareSerial serial;
    uint8_t buffer_s = 0;

    uint8_t buffer[RS485BUS_BUFFER_SIZE];

    uint16_t rx_enable_pin;
    uint16_t tx_enable_pin;
};

extern RS485Bus rs485Bus;

/*

I think the gcode command might have to deal with timeout stuff so it can call the idle method.

It'd be even better if I could even send the command and get a response back after X time, but the biggest problem I think is that marlin will respond with an "ok" for the command so we really do gotta make the delay in this command. And it can't be in the rs485 bus stuff because we can't call idle.

Point is, we gotta think through this API a bit. It can't just be a perfect copy of twibus.

*/