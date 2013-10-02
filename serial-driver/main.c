#include <avr/io.h>
#include "serial.h"

int main() {
  serialInit();

  serialWrite('!');
  serialWrite('!');
  serialWrite('!');
  serialWrite('!');

  serialString("testy!\n");
}
