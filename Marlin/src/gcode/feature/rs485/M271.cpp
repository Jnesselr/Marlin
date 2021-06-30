#include "../../../inc/MarlinConfig.h"

#if ENABLED(RS485_ENABLE)

#include "../../gcode.h"

#include "../../../feature/rs485bus.h"

uint16_t timeout = 100;

void GcodeSuite::M271() {
  if (parser.seen('T')) timeout = parser.value_int();

  if (! parser.seen('S')) return;
  char* byteString = parser.value_string();

  if(byteString[0] != '0' || !(byteString[1] == 'x' || byteString[1] == 'X')) {
    SERIAL_ERROR_START();
    SERIAL_ECHOLNPGM("S parameter must start with 0x");
    return;
  }

  if(strlen(byteString) % 2 != 0) {
    SERIAL_ERROR_START();
    SERIAL_ECHOLNPGM("S parameter must be an even number of bytes");
    return;
  }

  // TODO Replace 32 with RS485BUS_BUFFER_SIZE
  if(strlen(byteString) > 32 * 2 + 2) {
    SERIAL_ERROR_MSG("S parameter can not have more than ", 32, " bytes");
    return;
  }

  rs485Bus.reset();

  for(uint8_t i=2; i<strlen(byteString); i += 2) {
    unsigned char byte = HEXCHR(byteString[i]) << 4 | HEXCHR(byteString[i+1]);
    rs485Bus.push(byte);
  }

  rs485Bus.send();

  //delay(timeout);

  int result = rs485Bus.receive();
  if(result == -1) {
    SERIAL_ECHOLNPGM("Didn't receieve anything");
  } else {
    SERIAL_ECHOLNPGM("We got something!");
  }

  // Once we do that, we can figure out timeouts and everything
}

#endif