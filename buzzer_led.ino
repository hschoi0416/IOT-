#include <SoftwareSerial.h>
#include "SNIPE.h"

#define TXpin 11
#define RXpin 10
#define buzzerPin 6
#define ledPin 7
#define ATSerial Serial

String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00"; 

SoftwareSerial DebugSerial(RXpin, TXpin);
SNIPE SNIPE(ATSerial);

void setup() {
  ATSerial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  while (ATSerial.read() >= 0) {}
  while (!ATSerial);

  DebugSerial.begin(115200);

  /* SNIPE LoRa Initialization */
  if (!SNIPE.lora_init()) {
    DebugSerial.println("SNIPE LoRa Initialization Fail!");
    while (1);
  }

  /* SNIPE LoRa Set Appkey */
  if (!SNIPE.lora_setAppKey(lora_app_key)) {
    DebugSerial.println("SNIPE LoRa app key value has not been changed");
  }
  
  /* SNIPE LoRa Set Frequency */
  if (!SNIPE.lora_setFreq(LORA_CH_1)) {
    DebugSerial.println("SNIPE LoRa Frequency value has not been changed");
  }

  /* SNIPE LoRa Set Spreading Factor */
  if (!SNIPE.lora_setSf(LORA_SF_7)) {
    DebugSerial.println("SNIPE LoRa Sf value has not been changed");
  }

  /* SNIPE LoRa Set Rx Timeout 
   * If you select LORA_SF_12, 
   * RX Timout use a value greater than 5000  
  */
  if (!SNIPE.lora_setRxtout(5000)) {
    DebugSerial.println("SNIPE LoRa Rx Timout value has not been changed");
  }  

  DebugSerial.println("start");

}

void loop() {
  String value = SNIPE.lora_recv();
  int i_distance = value.toInt();
  DebugSerial.println(i_distance);

  // 초음파 값에 따라 부저와 LED를 작동시킴
  if (i_distance < 10) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
    SNIPE.lora_send("h");
    DebugSerial.println("high");
  } 

  else {
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
    SNIPE.lora_send("l");
    DebugSerial.println("low");
  }

  delay(100);
}
