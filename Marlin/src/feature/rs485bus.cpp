#include "../inc/MarlinConfig.h"

#if ENABLED(RS485_ENABLE)

#include "rs485bus.h"

RS485Bus::RS485Bus(HardwareSerial *serial, uint16_t rxEnablePin, uint16_t txEnablePin)
  : serial(serial), rx_enable_pin(rxEnablePin), tx_enable_pin(txEnablePin) {
}

void RS485Bus::init() {
  _SET_OUTPUT(rx_enable_pin);
  _SET_OUTPUT(tx_enable_pin);

  serial->begin(RS485_BAUD_RATE);

  reset();
}

void RS485Bus::reset() {
  buffer_s = 0;
  buffer[0] = 0;

  WRITE(rx_enable_pin, LOW);
  WRITE(tx_enable_pin, LOW);
}

int RS485Bus::push(unsigned char byte) {
  if(buffer_s == RS485BUS_BUFFER_SIZE) {
    return -1; // Buffer is full, won't add this byte
  }
  buffer[buffer_s] = byte;
  buffer_s++;

  return buffer_s;
}

int RS485Bus::send() {
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

void RS485Bus::receive() {
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

int RS485Bus::available() {
  return serial->available();
}

RS485Bus rs485Bus(
  &RS485_SERIAL,
  RS485_RX_ENABLE_PIN,
  RS485_TX_ENABLE_PIN
);

#endif
