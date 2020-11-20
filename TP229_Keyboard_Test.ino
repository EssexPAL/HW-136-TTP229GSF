/*
 * This a test of the HW-136 16 button keyboard unit.
 * The test can cope with 8 or 16 key modes (TP2) by 
 * altering NKEYS to be 8 or 16. 
 * 
 * The test was performed on a Nano and uses
 * Interrupt 1 (D3) for the SDO connection.  
 * The SDO triggers the interrupt to start
 * data collection when a key is pressed.
 * 
 * A pin change interrupt could also be used
 * to allow a different pin to be used for SDO.
 * Any pin that can be defined as an output
 * can be used for SCL.
 */

#define SDO 3 // data input triggers int1
#define SCL 4 // clock output
#define ONBOARDLED 13
#define NKEYS 16
//#define NKEYS 8

word volatile Kbdata = 0; // hold key data

void setup() {
  Serial.begin(9600);
  if (not Serial)
    ;

  pinMode(SCL, OUTPUT); // clock to keyboard
  digitalWrite(SCL, 1); // clock starting condition

  pinMode(SDO, INPUT);  // pin for data input
  pinMode(ONBOARDLED, OUTPUT);
  digitalWrite(ONBOARDLED, 0); 
  
  SREG |= 0x80;       // global interrupt enable
  EIMSK = 1 << INT1;  // int 1 enable
  EICRA &= 03;        // interrupt trigger on low level
}

void loop() {
  // put your main code here, to run repeatedly:
}

ISR(INT1_vect) {
  byte len = 0, i = 0;
  unsigned char Buf[25];

  EIMSK = 0;   // int 1 disable
  Kbdata = 0;

// read key bits from the TTP229
  for (i = 0; i < NKEYS; i++) {
//  generate clock
    digitalWrite(SCL, 0);
    digitalWrite(SCL, 1);

//  collect bit and assemble
    Kbdata |= digitalRead(SDO) << i; 

// fill up the top 8 bits if not in use
  if (NKEYS == 8) {
      Kbdata |= 0xFF00;
    }
  }

// if a key is pressed then output some info
  if (Kbdata != 0xffff) {
    Kbdata ^= 0xFFFF; // invert the key data
    len = sprintf(Buf, "%4.4x\r\n", Kbdata);
    digitalWrite(ONBOARDLED, 1);
    Serial.write(Buf, len);
  } else {
    digitalWrite(ONBOARDLED, 0);
  }
  
  EIMSK = 1 << INT1;   // int 1 re-enable
}
