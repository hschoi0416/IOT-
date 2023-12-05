#include <SoftwareSerial.h>
#include "SNIPE.h"
#include <ESP8266WiFi.h>

#define TXpin 13
#define RXpin 15
#define ATSerial Serial

String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00"; 

SoftwareSerial DebugSerial(RXpin, TXpin);
SNIPE SNIPE(ATSerial);

// WiFi 정보
char ssid[] = "";
char pass[] = "";

WiFiServer server(80);
void setup() {
  ATSerial.begin(115200);
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

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    DebugSerial.println("Connecting to WiFi...");
  }
  
  DebugSerial.println("WiFi connected");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    Serial.println(client);
    Serial.println("avail failed");
    delay(300);
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  String receivedData = SNIPE.lora_recv();
  while (1){
    if (receivedData == "h" || receivedData == "l"){
      break;
    }
    receivedData = SNIPE.lora_recv();
  }
  /* if (receivedData == "high") {
    DebugSerial.print("led on");
  }
  else if(receivedData == "low") {
    DebugSerial.print ("led off");
  } */
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n"));
  if (receivedData == "h") {
    client.print(F("<b>Bus arrived!!!</b>"));
  }
  else if(receivedData == "l") {
    client.print(F("<b>Bus leave!!</b>"));
  }
  client.print(F("<br><br> <a href='https://www.youtube.com/watch?v=RW-7L_KMayo'>Bus staion CCTV link</a>"));
  //client.print(WiFi.localIP());
  client.print(F("</html>"));
  //delay(3000);
}

