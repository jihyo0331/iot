#include <ThreeWire.h>  
#include <RtcDS1302.h>

#define TRIG 9 // TRIG 핀 설정 (초음파 보내는 핀)
#define ECHO 8 // ECHO 핀 설정 (초음파 받는 핀)

ThreeWire myWire(5,4,6); // IO, SCLK, CE  디지털 연결 번호
RtcDS1302<ThreeWire> Rtc(myWire);

long lastDistance = 0; // 이전 거리 값을 저장할 변수

void setup () {    
    Serial.begin(9600);
    Rtc.Begin();

    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    // 현재 시간을 받아와 출력
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
    Serial.println();
}

void loop () {
    // 초음파 센서로부터 거리 측정
    long distance = getDistance();
    
    // 이전 거리와 현재 거리 비교 (10cm 이상 차이가 있을 때만)
    if (abs(distance - lastDistance) >= 10) {
        RtcDateTime now = Rtc.GetDateTime();
        printDateTime(now);
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
