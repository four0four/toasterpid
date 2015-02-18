## AVR atmega168 pin listing

### Port B
+ Bit 0 - NC
+ Bit 1 - NC 
+ Bit 2 - uSD CS
+ Bit 3 - SPI MOSI
+ Bit 4 - SPI MISO
+ Bit 5 - SPI CLK
+ Bit 6 - AVR clk input (from enc28j60)
+ Bit 7 - SSR output (can we do this with clkin on b6?)

### Port C
+ Bit 0 - NC
+ Bit 1 - LCD EN
+ Bit 2 - LCD RS
+ Bit 3 - LCD R/W
+ Bit 4 - PHY reset
+ Bit 5 - PHY interrupt
+ Bit 6 - AVR reset
+ Bit 7 - Not pinned out

### Port D
+ Bit 0 - LCD bus bit 4 (0 in 4bit)
+ Bit 1 - LCD bus bit 5 (1 in 4bit)
+ Bit 2 - LCD bus bit 6 (2 in 4bit)
+ Bit 3 - LCD bus bit 7 (3 in 4bit)
+ Bit 4 - PHY CS
+ Bit 5 - Daughter CS (Thermo amp CS)
+ Bit 6 - Daughter LED 1
+ Bit 7 - Daughter LED 2
