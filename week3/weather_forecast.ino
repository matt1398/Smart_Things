#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "customChar.h"

#define LCD_ADDRESS 0x27
#define STASSID "olleh_WiFi_04AF" //와이파이 검색했을때 뜨는 이름
#define STAPSK  "0000002249" //패스워드

const char* ssid = STASSID;
const char* password = STAPSK;

//http://www.kma.go.kr/wid/queryDFSRSS.jsp?zone=1159068000
const char* host = "www.kma.go.kr";
const int httpsPort = 80;
 
LiquidCrystal_I2C lcd(LCD_ADDRESS, 2, 1, 0, 4, 5, 6, 7);
String url = "/wid/queryDFSRSS.jsp?zone=1159068000";

const int num = 4;

void setup() {
  lcd.begin(16, 2); // lcd 설정
  lcd.setBacklightPin(3, POSITIVE); // lcd backlight on
  lcd.setBacklight(HIGH); // lcd backlight on


  WiFi.mode(WIFI_STA); // wifi 세팅
  WiFi.begin(ssid, password); // wifi 세팅
  
  while (WiFi.status() != WL_CONNECTED) { // wifi 연결 안된동안 무한루프
    delay(500); // 0.5초마다 체크
    lcd.print("."); // ....출력
  }
  //wifi 연결되면 이리로 넘어옴
  lcd.setCursor(0,1); // lcd 커서 이동(둘째줄)
  lcd.print("WiFi connected"); // ......(다음줄) WiFi connected라고 출력
  
  lcd.clear(); // lcd 비움
  lcd.setCursor(0,0); // lcd 커서 첫째줄 첫칸으로 이동
  lcd.print("IP address"); // IP address 첫줄에 출력
  
  lcd.setCursor(0,1); // lcd 커서 두번째줄 첫칸으로 이동
  lcd.print(WiFi.localIP()); // 연결된 WiFi IP address 출력

  LCD_Custom(); // 특수문자(날씨 이모티콘) 세팅
  delay(2000); // 2초 딜레이
}

void loop() { 

  String str[num]; // 뽑을 데이터 길이만큼의 string 선언
  char* strKey[num] = {"hour","temp","pop","pty"}; // hour, temp, pop, pty의 html 파싱한다는거 알려줌
  bool check[num] = {false, }; // 파싱 결과를 저장할 배열
  String strTmp; // 데이터 파싱 후 거쳐갈 temporary string

  WiFiClient client; // WiFiClient 선언
  
  if (!client.connect(host, httpsPort)) { // 기상청 연결이 안됐다면
    lcd.print("connection Failed"); // connection failed 출력
    return; // 다시 loop 처음부터
  }
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n"); // 기상청 홈페이지 서버에 request 보냄
 
  unsigned long t = millis(); // 현재 시간 저장
  while(1){ // 무한루프(아래 두가지 조건이 만족 시 탈출)
    if(client.available()) break; // client에서 데이터가 답변이 들어왔으면
    if(millis() - t > 10000) break; // or 연결 안되고 10초가 넘어갔으면
  }
 
  while(client.available()) { // clinet에서 데이터 답변이 들어왔으면
    String data = client.readStringUntil('\n'); // 답변 전체를 데이터로 저장
    delay(1); // 아주잠깐 딜레이

    for (int i=0 ; i< num; i++) { // 뽑을 데이터 만큼 반복(여기선 4개)
      strTmp = dataParsing(data, strKey[i], &check[i]); // dataParsing을 통해 데이터 받아옴
      if(strTmp!= "e") str[i] = strTmp; // 데이터 파싱 실패("e" return)하지 않는다면 strTmp에 저장된 데이터를 str에 저장
    }
  }
  lcdDisplay(str); // 데이터 LCD에 정해진 형식으로 출력
  delay(10000); // 10초마다 갱신
}

void lcdDisplay(String *strData) { // lcdDisplay함수 -> 파싱을 통해 생성된 str[num]을 받아와서 lcd에 출력하는 함수
  lcd.clear(); // lcd 초기화
  lcd.setCursor(1,0); // lcd 첫째줄 둘째칸으로 커서 이동
  lcd.print("At "); lcd.print(strData[0]); lcd.print(" o' clock"); // At (hour) o' clock 형식으로 시간 출력
  lcd.setCursor(0,1); // lcd 둘째줄 첫째칸으로 커서 이동
  lcd.print(strData[1]); lcd.print("C"); // (temp) C 형식으로 온도 출력
  lcd.setCursor(7,1); // lcd 둘째줄 여덟번째칸으로 커서 이동
  lcd.print(strData[2]); lcd.print("%"); // (pop) % 형식으로 습도 출력
  lcd.setCursor(13,1); // lcd 둘째줄 열네번째칸으로 커서 이동
  if(strData[3] == "0") lcd.write(0); // 선언한 특수문자 첫번째꺼 출력(맑음)
  else if (strData[3] == "1" || strData[3] == "2"|| strData[3] == "5"|| strData[3] == "6") lcd.write(2); // 선언한 특수문자 세번째꺼 출력(비)
  else if (strData[3] == "3" || strData[3] == "7") lcd.write(3); // 선언한 특수문자 네번째꺼 출력
  else if (strData[3] == "4") lcd.write(4); // 선언한 특수문자 다섯번째꺼 출력
  else if (strData[3] == "5") lcd.write(5); // 선언한 특수문자 여섯번째꺼 출력
}

void LCD_Custom(void)
{
  lcd.createChar(0, CC1); // CC1을 lcd.write(0)으로 했을때 출력되는 특수문자로 저장
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
  int s = -1; // 요구된 데이터(str)의 처음 index
  int e = -1; // 요구된 데이터(str)의 마지막 index -> 웹으로부터 받아온 data의 s ~ e를 요구한 데이터의 결과값으로 return
  
  if (str=="hour"){ // 요구된 데이터(str) == hour일때
    String temp = "<hour>";
    s = DATA.indexOf(temp) + temp.length(); // <hour>시간</hour>에서 <hour>(여기) 포인트를 s로 저장
    e = DATA.indexOf("</hour>"); // <hour>시간</hour>에서 (여기)</hour> 포인트를 e로 저장
  }
  else if (str=="temp"){ // 위와 동일
    String temp = "<temp>";
    s = DATA.indexOf(temp) + temp.length();
    e = DATA.indexOf("</temp>");
  }
  else if (str=="pop"){ // 위와 동일
    String temp = "<pop>";
    s = DATA.indexOf(temp) + temp.length();
    e = DATA.indexOf("</pop>");
  }
  else if (str=="pty"){ // 위와 동일
    String temp = "<pty>";
    s = DATA.indexOf(temp) + temp.length();
    e = DATA.indexOf("</pty>");
  }
  
  if( (s != -1) && (e != -1) && !(*check) ) { // s, e가 선언되었고, check(요구된 데이터가 이미 찾아졌는지 여부)
    (*check) = true; // 요구된 데이터가 찾아졌다고 저장
    return DATA.substring(s,e); // s ~ e까지의 데이터를 잘라서 요구된 데이터의 결과값으로 리턴(찾아졌으면 여기서 함수 끝)
  }
  return "e"; // 못찾아졌을때 "e"로 리턴
}  
