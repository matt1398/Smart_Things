#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "customChar.h"

#define LCD_ADDRESS 0x27
#define STASSID "beomsuk" //와이파이 검색했을때 뜨는 이름
#define STAPSK  "11111111" //패스워드

const char* ssid = STASSID;
const char* password = STAPSK;

//http://www.kma.go.kr/wid/queryDFSRSS.jsp?zone=1159068000
const char* host = "www.kma.go.kr";
const int httpsPort = 80;
 
LiquidCrystal_I2C lcd(LCD_ADDRESS, 2, 1, 0, 4, 5, 6, 7);
String url = "/wid/queryDFSRSS.jsp?zone=1159068000";

const int num = 4;

void setup() {
/*  Serial.begin(9600);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);*/

  lcd.begin(16, 2);
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
//    Serial.print(".");
  }
/*Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("connecting to ");
  Serial.println(host);
  Serial.print("requesting URL: ");
  Serial.println(url);*/

  lcd.setCursor(0,1);
  lcd.print("WiFi connected");
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("IP address");
  
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());

  LCD_Custom();
  delay(2000);
}

void loop() { 

  String str[num];
  char* strKey[num] = {"hour","temp","pop","pty"};
  bool check[num] = {false, };
  String strTmp;

  WiFiClient client;
  
  if (!client.connect(host, httpsPort)) 
  {
    //Serial.println("connection failed");
    lcd.print("connection Failed");
    return;
  }
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
 
  unsigned long t = millis(); //생존시간
  while(1){
    if(client.available()) break;
    if(millis() - t > 10000) break;
  }
 
  while(client.available())
  {
    String data = client.readStringUntil('\n'); //readline();
//  Serial.println(data);
    delay(1);

    for (int i=0 ; i<num ; i++) {
      strTmp = dataParsing(data, strKey[i], &check[i]);
      if(strTmp!= "e") str[i] = strTmp;
    }
  }
/*
  Serial.print("Time : "); Serial.print(strHOUR);
  Serial.print(" , Temp : "); Serial.print(strTEMP);
  Serial.print(" , POP : "); Serial.println(strPOP); 
*/
  lcdDisplay(str);
 // initParam(str, check);
  delay(10000);
}

void lcdDisplay(String *strData) {
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("At "); lcd.print(strData[0]); lcd.print(" o' clock"); //hour
  lcd.setCursor(0,1);
  lcd.print(strData[1]); lcd.print("C"); //Temperature
  lcd.setCursor(7,1);
  lcd.print(strData[2]); lcd.print("%");
  lcd.setCursor(13,1);
  if(strData[3] == "0") lcd.write(0);// clear
  else if (strData[3] == "1" || strData[3] == "2"|| strData[3] == "5"|| strData[3] == "6") lcd.write(2); // rain
  else if (strData[3] == "3" || strData[3] == "7") lcd.write(3);
  else if (strData[3] == "4") lcd.write(4);
  else if (strData[3] == "5") lcd.write(5);
}

void LCD_Custom(void)
{
  lcd.createChar(0, CC1);
  lcd.createChar(1, CC2);
  lcd.createChar(2, CC3);
  lcd.createChar(3, CC4);
  lcd.createChar(4, CC5);
  lcd.createChar(5, CC6);
  lcd.createChar(6, CC7);
  lcd.createChar(7, CC8);
}

String dataParsing(String DATA, char* str, bool *check)
{ // HOUR, TEMP, POP, PTY
  int s = -1;
  int e = -1;
  
  if (str=="hour"){
    String temp = "<hour>";
    s = DATA.indexOf(temp) + temp.length();
    e = DATA.indexOf("</hour>");
  }
  else if (str=="temp"){
    String temp = "<temp>";
    s = DATA.indexOf(temp) + temp.length();
    e = DATA.indexOf("</temp>");
  }
  else if (str=="pop"){
    String temp = "<pop>";
    s = DATA.indexOf(temp) + temp.length();
    e = DATA.indexOf("</pop>");
  }
  else if (str=="pty"){
    String temp = "<pty>";
    s = DATA.indexOf(temp) + temp.length();
    e = DATA.indexOf("</pty>");
  }
  
  if( (s != -1) && (e != -1) && !(*check) ) {
    (*check) = true;
    return DATA.substring(s,e);
  }
  return "e"; 
}  