//U-Keeper Door

#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include <SoftwareSerial.h>

#define TRIG 9 // TRIG 핀 설정 (초음파 보내는 핀)
#define ECHO 8 // ECHO 핀 설정 (초음파 받는 핀)

ThreeWire myWire(5,4,6); // IO, SCLK, CE  디지털 연결 번호
RtcDS1302<ThreeWire> Rtc(myWire);

long lastDistance = 0; // 이전 거리 값을 저장할 변수
SoftwareSerial mySerial(2, 3); // RX, TX

void sendCommand(const char * command){
  Serial.print("Command send :");
  Serial.println(command);
  mySerial.println(command);
  //wait some time
  delay(100);
  
  char reply[100];
  int i = 0;
  while (mySerial.available()) {
    reply[i] = mySerial.read();
    i += 1;
  }
  //end the string
  reply[i] = '\0';
  Serial.print(reply);
  Serial.println("Reply end");
}

void setup () {    
    Serial.begin(9600);
    Rtc.Begin();
    while (!Serial);
    mySerial.begin(9600);
    sendCommand ( "AT" );
    sendCommand ("AT+NAMEU-Keeper_D");
    sendCommand("AT");
    sendCommand("AT+ROLE0");
    sendCommand("AT+UUID0xFFE0");
    sendCommand("AT+CHAR0xFFE1");

    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    // RTC에 현재 시간을 설정합니다. 첫 설정 이후에는 주석처리해도 됩니다.
    // 현재 시간을 처음 설정할 때 아래 코드의 주석을 해제하세요.
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    Rtc.SetDateTime(compiled);  // RTC에 현재 시간 설정
    
    // 현재 시간을 받아와 출력
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
    Serial.println();
}


void loop () {

  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }

  if (Serial.available()) {
    mySerial.write(Serial.read());
  }

    // 초음파 센서로부터 거리 측정
    long distance = getDistance();
    
    // 이전 거리와 현재 거리 비교 (10cm 이상 차이가 있을 때만)
    if (abs(distance - lastDistance) >= 10) {
        RtcDateTime now = Rtc.GetDateTime();
        printDateTime(now);

        // 시간을 블루투스로 전송
        sendTime(now);

        // 이전 거리 값을 현재 값으로 업데이트
        lastDistance = distance;
    }
    
    delay(1000);  // 1초마다 업데이트
}

long getDistance() {
    digitalWrite(TRIG, LOW);
    delay(100);
    digitalWrite(TRIG, HIGH);
    delay(100);
    digitalWrite(TRIG, LOW);

    long duration = pulseIn(ECHO, HIGH);
    long distance = duration * 0.034 / 2; // 거리 계산 (cm)
    return distance;
}

void sendTime(const RtcDateTime& dt) {
    char message[30];
    snprintf_P(message, 
               sizeof(message),
               PSTR("%04u-%02u-%02u %02u:%02u\n"),
               dt.Year(),
               dt.Month(),
               dt.Day(),
               dt.Hour(),
               dt.Minute());
    
    // 블루투스로 전송
    mySerial.print(message);
    // 시리얼 모니터로도 전송
    Serial.print("Sent: ");
    Serial.print(message);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt){
    char datestring[20];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%04u-%02u-%02u %02u:%02u\n"),
            dt.Year(),
            dt.Month(),
            dt.Day(),
            dt.Hour(),
            dt.Minute());
    Serial.print(datestring);
}
