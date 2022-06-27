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
  : serial(rxPin, txPin) {
    // TODO Turn this into a define
    MSerial5.begin(9600);
    reset();
    //WRITE(RS485_RX_ENABLE_PIN, HIGH);
}

void RS485Bus::reset() {
  buffer_s = 0;
  buffer[0] = 0;
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
  WRITE(RS485_TX_ENABLE_PIN, HIGH);
  for(uint8_t i=0; i<buffer_s; i++) {
    serial.write(buffer[i]);
  }
  WRITE(RS485_TX_ENABLE_PIN, LOW);
  
  return 0;
}

int RS485Bus::receive() {
  if(! serial.available()) {
    return -1;
  }

  SERIAL_ECHOLNPGM("Received:");
  while(serial.available()) {
    SERIAL_ECHOLN(serial.read());
  }
  return 0;
}

#endif