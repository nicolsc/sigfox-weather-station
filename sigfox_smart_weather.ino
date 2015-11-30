#include <LPS25H.h>
#include <HTS221.h>
#include <Wire.h>
#include <Arduino.h>

#define SIGFOX_FRAME_LENGTH 12
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

  initSigfox();
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
 
  bool answer = sendSigfox(&frame, sizeof(data));

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

void initSigfox(){
  SigFox.print("+++");
  while (!SigFox.available()){
    delay(100);
  }
  while (SigFox.available()){
    if (DEBUG){
      SerialUSB.print(SigFox.read());
    }
  }
  SerialUSB.println("\n ** Setup OK **");
}
String getSigfoxFrame(const void* data, uint8_t len){
  String frame = "";
  uint8_t* bytes = (uint8_t*)data;
  
  if (len < SIGFOX_FRAME_LENGTH){
    //fill with zeros
    uint8_t i = SIGFOX_FRAME_LENGTH;
    while (i-- > len){
      frame += "00";
    }
  }

  //0-1 == 255 --> (0-1) > len
  for(uint8_t i = len-1; i < len; --i) {
    if (bytes[i] < 16) {frame+="0";}
    frame += String(bytes[i], HEX);
  }
  
  return frame;
}
bool sendSigfox(const void* data, uint8_t len){
  String frame = getSigfoxFrame(&data, len);
  String status = "";
  char output;
  if (DEBUG){
    SerialUSB.print("AT$SF=");
    SerialUSB.println(frame);
  }
  SigFox.print("AT$SF=");
  SigFox.print(frame);
  SigFox.print("\r");
  while (!SigFox.available());
  
  while(SigFox.available()){
    output = (char)SigFox.read();
    status += output;
    delay(10);
  }
  if (DEBUG){
    SerialUSB.print("Status \t");
    SerialUSB.println(status);
  }
  if (status == "OK\r"){
    //Success :)
    return true;
  }
  else{
    return false;
  }
}
