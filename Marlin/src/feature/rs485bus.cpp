#include "../inc/MarlinConfig.h"

#include "rs485bus.h"

RS485Bus rs485Bus(
  RS485_RX_PIN,
  RS485_RX_ENABLE_PIN,
  RS485_TX_PIN,
  RS485_TX_ENABLE_PIN
);

#if ENABLED(RS485_ENABLE)

RS485Bus::RS485Bus(uint16_t rxPin, uint16_t rxEnablePin, uint16_t txPin, uint16_t txEnablePin)
  : serial(rxPin, txPin), rx_enable_pin(rxEnablePin), tx_enable_pin(txEnablePin) {
    //WRITE(RS485_RX_ENABLE_PIN, HIGH);
    WRITE(rx_enable_pin, HIGH);
    WRITE(tx_enable_pin, LOW);
}

void RS485Bus::init() {
  _SET_OUTPUT(rx_enable_pin);
  _SET_OUTPUT(tx_enable_pin);
  serial.begin(9600);
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

  size_t ret = serial.write(buffer, buffer_s);
  serial.flush();
  delay(5);

  WRITE(tx_enable_pin, LOW);
  WRITE(rx_enable_pin, LOW);
  delay(5);
  
  return ret;
}

int RS485Bus::receive() {
  unsigned char data;
  //if(! serial.available()) {
  //  return -1;
  //}

  //SERIAL_ERROR_MSG("Received ", serial.available(), " bytes");

  SERIAL_ECHOLNPGM("Received:");
  while(serial.readBytes(&data, 1) > 0) {
    SERIAL_ECHO((data < 0x10) ? " 0x0" : " 0x");

    SERIAL_PRINT(data, PrintBase::Hex);
  }
  SERIAL_ECHOLN("");
  //while(serial.available()) {
  //  SERIAL_PRINT(serial.read(), PrintBase::Hex);
  //}
  return 0;
}

#endif