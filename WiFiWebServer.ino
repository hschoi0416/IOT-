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
char ssid[] = "KY";
char pass[] = "12345678";

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
  WiFiClient client = server.available();
  if (!client) {
    Serial.println(client);
    Serial.println("avail failed");
    delay(300);
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000);

  // 클라이언트로부터 HTTP 요청을 받아오는 부분
  String req = client.readStringUntil('\r');
  Serial.println(F("요청: "));
  Serial.println(req);

  // LoRa 모듈에서 데이터를 수신하는 부분
  String receivedData = SNIPE.lora_recv();
  while (1){
    // LoRa 모듈에서 수신한 데이터가 "h" 또는 "l"이면 루프를 탈출하고,
    // 그렇지 않으면 계속해서 LoRa 모듈에서 데이터를 받아옴
    if (receivedData == "h" || receivedData == "l"){
      break;
    }
    receivedData = SNIPE.lora_recv();
  }

  // 클라이언트로부터 수신된 데이터는 사용하지 않으므로, 버퍼를 비워줌
  while (client.available()) {
  // 바이트별로 읽는 것은 효율적이지 않으므로, 버퍼 비우기
  client.read();
  }

  // HTTP 응답 헤더 전송 
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n"));
  // LoRa 모듈에서 수신한 데이터에 따라 다른 메시지 전송
  if (receivedData == "h") {
    // 만약 LoRa 모듈에서 수신한 데이터가 "h"라면, "Bus arrived!!!" 메시지를 클라이언트에 전송
    client.print(F("<b>Bus arrived!!!</b>"));
  }
  else if(receivedData == "l") {
    // 만약 LoRa 모듈에서 수신한 데이터가 "l"이라면, "Bus leave!!" 메시지를 클라이언트에 전송
    client.print(F("<b>Bus leave!!</b>"));
  }
  // 유튜브 스트리밍 링크 제공
  client.print(F("<br><br> <a href='https://www.youtube.com/watch?v=RW-7L_KMayo'>Bus staion CCTV link</a>"));
  client.print(F("</html>"));
}
