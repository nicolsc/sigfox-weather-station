#include <LPS25H.h>
#include <HTS221.h>
#include <Wire.h>
#include <Arduino.h>

#define INTERVAL 600000
#define DEBUG 0

unsigned long previousSendTime = 0;

struct data {
  int humidity;
  float temperature;
  int pressure;
};


void setup() {

  // Init UART devices
  if (DEBUG) {
    SerialUSB.begin(115200);
  }
  smeHumidity.begin();
  smePressure.begin();
  
  SigFox.begin(19200);

  if (DEBUG) {
    // Wait for the user to connect to serial console
    while (!SerialUSB) {}
  }

  // Switch SigFox modem to command mode
  sigfoxCommandMode();
}

void loop() {
  data frame;
  frame.humidity = smeHumidity.readHumidity();  
  frame.temperature = (smeHumidity.readTemperature() + smePressure.readTemperature())/2.0;
  frame.pressure = smePressure.readPressure();

  if (DEBUG) {
    SerialUSB.print("Temp ");
    SerialUSB.println(frame.temperature, 6);
    SerialUSB.print("\tHumidity ");
    SerialUSB.println(frame.humidity);
    SerialUSB.print("\tPressure ");
    SerialUSB.println(frame.pressure);
  }

  // Send data
  if (DEBUG) {
    SerialUSB.println("Sending data");
  }
  bool answer = sigfoxSend(&frame, sizeof(data));

  // Light LED depending on modem answer
  if (answer) {
    ledGreenLight(HIGH);
    ledRedLight(LOW);
  } else {
    ledGreenLight(LOW);
    ledRedLight(HIGH);
  }
  delay(1000);
  ledGreenLight(LOW);
  ledRedLight(LOW);
  
  delay(INTERVAL);
}

// Switch SigFox modem to command mode
void sigfoxCommandMode() {
  SigFox.print("+++");

  // Waiting for modem answer
  while (!SigFox.available()) {
    delay(100);
  }
  while (SigFox.available()) {
    char answer = (char)SigFox.read();
    if (DEBUG) {
      SerialUSB.print(answer);
    }
  }

  if (DEBUG) {
    SerialUSB.println("");
  }
  delay(10000);
}
boolean canSend(long prev){
  if (prev==0){
    return true;
  }
  unsigned long diff = millis() - prev;
  return diff >= 600000;
}
/* Send any datatype through AT commands. */
bool sigfoxSend(const void* data, uint8_t len) {
  uint8_t* bytes = (uint8_t*)data;

  if (DEBUG) {
    SerialUSB.println("Issuing AT command");
  }
  
  SigFox.print('A');
  SigFox.print('T');
  SigFox.print('$');
  SigFox.print('S');
  SigFox.print('F');
  SigFox.print('=');
  //0-1 == 255 --> (0-1) > len
  for(uint8_t i = len-1; i < len; --i) {
    if (bytes[i] < 16) {SigFox.print("0");}
    SigFox.print(bytes[i], HEX);
  }
  SigFox.print('\r');

  if (DEBUG) {
    SerialUSB.print('A');
    SerialUSB.print('T');
    SerialUSB.print('$');
    SerialUSB.print('S');
    SerialUSB.print('F');
    SerialUSB.print('=');
    for(uint8_t i = len-1; i < len; --i) {
      if (bytes[i] < 16) {SerialUSB.print("0");}
      SerialUSB.print(bytes[i], HEX);
    }
    SerialUSB.print('\r');
    SerialUSB.println("");
  }

  bool error = false;
  // Waiting for modem answer
  while (!SigFox.available()) {
    delay(100);
  }
  bool firstChar = true;
  while (SigFox.available()) {
    char answer = (char)SigFox.read();
    if (DEBUG) {
      SerialUSB.print(answer);
    }
    if (firstChar) {
      firstChar = false;
      if (answer == 'O') { // "OK" message
        error = false; 
      } else { // "ERROR" message 
        error = true;
      }
    }
  }
  if (DEBUG) {
    SerialUSB.println("");
  }

  return !error;
}




