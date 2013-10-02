#include <avr/io.h>
#include "serial.h"

int main() {
  serialInit();

  serialString("testy!\n\r");
}
