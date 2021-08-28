#pragma once

#if ENABLED(RS485_ENABLE)

#include <stdint.h>

#include "SoftwareSerial.h"
#include "Stream.h"

#define BAUD_RATE 9600

template<class T>
class RS485Bus {
  public:
    RS485Bus(T *serial, uint16_t rxEnablePin, uint16_t txEnablePin);

    void init();
    void reset();
    int push(unsigned char byte);
    int send();
    void receive();

  private:
    T *serial;
    uint8_t buffer_s = 0;

    uint8_t buffer[RS485BUS_BUFFER_SIZE];

    uint16_t rx_enable_pin;
    uint16_t tx_enable_pin;
};

template<class T>
RS485Bus<T>::RS485Bus(T *serial, uint16_t rxEnablePin, uint16_t txEnablePin)
  : serial(serial), rx_enable_pin(rxEnablePin), tx_enable_pin(txEnablePin) {
}

template<class T>
void RS485Bus<T>::init() {
  _SET_OUTPUT(rx_enable_pin);
  _SET_OUTPUT(tx_enable_pin);

  serial->begin(BAUD_RATE);

  reset();
}

template<class T>
void RS485Bus<T>::reset() {
  buffer_s = 0;
  buffer[0] = 0;

  WRITE(rx_enable_pin, LOW);
  WRITE(tx_enable_pin, LOW);
}

template<class T>
int RS485Bus<T>::push(unsigned char byte) {
  if(buffer_s == RS485BUS_BUFFER_SIZE) {
    return -1; // Buffer is full, won't add this byte
  }
  buffer[buffer_s] = byte;
  buffer_s++;

  return buffer_s;
}

template<class T>
int RS485Bus<T>::send() {
  WRITE(tx_enable_pin, HIGH);
  WRITE(rx_enable_pin, HIGH);
  delay(5);

  size_t ret = serial->write(buffer, buffer_s);
  serial->flush();
  delay(5);

  WRITE(tx_enable_pin, LOW);
  WRITE(rx_enable_pin, LOW);
  delay(5);

  return ret;
}

template<class T>
void RS485Bus<T>::receive() {
  unsigned char data;

  SERIAL_ECHO("rs485-reply: ");

  if(! serial->available()) {
    SERIAL_ECHOLN("TIMEOUT");
    return;
  }

  // SERIAL_ECHO("Received ");
  // SERIAL_ECHO(serial.available());
  // SERIAL_ECHO(" bytes");
  // SERIAL_ECHOLN("");
  

  // SERIAL_ECHOLNPGM("Received:");
  while(serial->readBytes(&data, 1) > 0) {
    SERIAL_ECHO((data < 0x10) ? "0" : "");

    SERIAL_PRINT(data, PrintBase::Hex);
  }
  SERIAL_EOL();
}

#ifdef RS485_SERIAL_PORT
  static RS485Bus<HardwareSerial> rs485Bus(
    &MSerial1,
    RS485_RX_ENABLE_PIN,
    RS485_TX_ENABLE_PIN
  );
#else
  static SoftwareSerial rs485SoftwareSerial(RS485_RX_PIN, RS485_TX_PIN);
  static RS485Bus<SoftwareSerial> rs485Bus(
    &rs485SoftwareSerial,
    RS485_RX_ENABLE_PIN,
    RS485_TX_ENABLE_PIN
  );
#endif

#endif

/*

I think the gcode command might have to deal with timeout stuff so it can call the idle method.

It'd be even better if I could even send the command and get a response back after X time, but the biggest problem I think is that marlin will respond with an "ok" for the command so we really do gotta make the delay in this command. And it can't be in the rs485 bus stuff because we can't call idle.

Point is, we gotta think through this API a bit. It can't just be a perfect copy of twibus.

*/