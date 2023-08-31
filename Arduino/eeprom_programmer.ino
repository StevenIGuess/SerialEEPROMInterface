#define SHIFT_OUT 2
#define SHIFT_CLK 4
#define EEPROM_CE 3
#define EEPROM_WE 13
#define EEPROM_D0 5
#define EEPROM_D7 12
#define CHAR_SPACE " "
#include "cli.h"
char CLI_DELIM_SPACE[] = CHAR_SPACE;
char CLI_DELIM_EOL[] = "\r\n";
char CLI_DELIM_SPACE_EOL[] = " \r\n";
int LEDS[] = {A0, A1, A2, A3, A4, A5, A6, A7};


void clearOut() {
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(A5, LOW);
  digitalWrite(A6, LOW);
}

void setAddress(int address, bool outEnable) {

  digitalWrite(EEPROM_CE, HIGH);
  delayMicroseconds(1);
  shiftOut(SHIFT_OUT, SHIFT_CLK, MSBFIRST, ((address >> 8) | (outEnable ? 0x00 : 0x80)));
  shiftOut(SHIFT_OUT, SHIFT_CLK, MSBFIRST, address);
  digitalWrite(EEPROM_CE, LOW);

}

byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin +=1) {
    pinMode(pin, INPUT);
  }

  setAddress(address, true);
  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
    digitalWrite(LEDS[pin-5], digitalRead(pin));
  }
  return data;
}

void dump(int len) {

   for (int base = 0; base <= len; base +=16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset +=1) {
      data[offset] = readEEPROM(offset + base);
    }
    char buf[80];

    sprintf(buf, "%03x: %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x", base, data[0],
    data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
    Serial.print("!");
  }

}

void writeEEPROM(int address, byte data) {

  setAddress(address, false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    digitalWrite(LEDS[pin - 5], data & 1);
    data = data >> 1;
  }
  
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin +=1) {
    pinMode(pin, OUTPUT);
  }

  digitalWrite(EEPROM_WE, LOW);
  delayMicroseconds(1);
  digitalWrite(EEPROM_WE, HIGH);
  delay(10); 
}

void cliDUMP(char *cmd, char* buffer) {
  char* num = strtok(buffer, CLI_DELIM_SPACE_EOL);
  if(num)
  {
    dump(atoi(num));
    Serial.print("EOFL!");
    clearOut();
    Serial.end();
  }
  
}

void cliWRITE(char *cmd, char* buffer) {
  char* in = strtok(buffer, CLI_DELIM_SPACE_EOL);
  int addr, val;
  bool err = false;
  if(in)
  {
    addr = atoi(in);
  }else {
    err = true;
  }
  in = strtok(NULL, CLI_DELIM_SPACE_EOL);
  if(in)
  {
    val = atoi(in);
  } else {
    err = true;
  }

  if(err) {
    Serial.print("ERR");
  } else {
    writeEEPROM(addr, val);
    Serial.print("SUC");
  }
}


MyCli cli;

void setup() {
  // put your setup code here, to run once:
  pinMode(SHIFT_OUT, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(EEPROM_CE, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(A6, OUTPUT);

  digitalWrite(EEPROM_WE, HIGH);
  pinMode(EEPROM_WE, OUTPUT);
  
  digitalWrite(EEPROM_CE, LOW);

  clearOut();
  Serial.begin(57600);
  digitalWrite(A0, HIGH);



  

  cli = MyCli();
  cli.addCmd("dump", &cliDUMP);
  cli.addCmd("write", &cliWRITE);
}

void loop() {
  
  cli.think();


}
