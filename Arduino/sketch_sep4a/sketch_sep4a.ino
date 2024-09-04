#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX
int FSRsensor = A0;
int value = 0;
int initialValue = 0;

// RTC를 위한 변수 선언
unsigned long previousMillis = 0; // 이전 시간 저장
int seconds, minutes, hours, days, months, years;

int daysInMonth(int month, int year) {
    switch (month) {
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
                return 29;
            else
                return 28;
        default:
            return 31;
    }
}

void initializeRTC() {
    // __DATE__와 __TIME__ 매크로를 사용해 초기화
    char date[] = __DATE__;  // "MMM DD YYYY"
    char time[] = __TIME__;  // "HH:MM:SS"
    
    // 날짜 파싱
    char monthStr[4];
    sscanf(date, "%s %d %d", monthStr, &days, &years);
    
    // 시간 파싱
    sscanf(time, "%d:%d:%d", &hours, &minutes, &seconds);

    // 월을 숫자로 변환
    if (strcmp(monthStr, "Jan") == 0) months = 1;
    else if (strcmp(monthStr, "Feb") == 0) months = 2;
    else if (strcmp(monthStr, "Mar") == 0) months = 3;
    else if (strcmp(monthStr, "Apr") == 0) months = 4;
    else if (strcmp(monthStr, "May") == 0) months = 5;
    else if (strcmp(monthStr, "Jun") == 0) months = 6;
    else if (strcmp(monthStr, "Jul") == 0) months = 7;
    else if (strcmp(monthStr, "Aug") == 0) months = 8;
    else if (strcmp(monthStr, "Sep") == 0) months = 9;
    else if (strcmp(monthStr, "Oct") == 0) months = 10;
    else if (strcmp(monthStr, "Nov") == 0) months = 11;
    else if (strcmp(monthStr, "Dec") == 0) months = 12;
}

void updateRTC() {
    unsigned long currentMillis = millis();
    
    if (currentMillis - previousMillis >= 1000) { // 1초가 경과했을 때
        previousMillis = currentMillis;
        seconds++;
        
        if (seconds >= 60) {
            seconds = 0;
            minutes++;
            
            if (minutes >= 60) {
                minutes = 0;
                hours++;
                
                if (hours >= 24) {
                    hours = 0;
                    days++;
                    
                    int maxDays = daysInMonth(months, years);
                    if (days > maxDays) {
                        days = 1;
                        months++;
                        
                        if (months > 12) {
                            months = 1;
                            years++;
                        }
                    }
                }
            }
        }
    }
}

void sendRTC() {
    // 날짜와 시간을 "YYYY-MM-DD HH:MM:SS" 형식으로 블루투스로 전송
    char dateTimeString[20];
    snprintf(dateTimeString, sizeof(dateTimeString), "%04d-%02d-%02d %02d:%02d", years, months, days, hours, minutes);
    mySerial.println(dateTimeString);
    Serial.print("RTC sent: ");
    Serial.println(dateTimeString);
}

void setup() {    
    Serial.begin(9600);
    mySerial.begin(9600);

    // RTC 초기화
    initializeRTC();
    Serial.println("RTC initialized to current compile time.");

    // 초기 센서 값을 읽음
    initialValue = analogRead(FSRsensor);
    Serial.print("Initial Value: ");
    Serial.println(initialValue);
}

void loop() {
    updateRTC();

    value = analogRead(FSRsensor);  // 현재 센서 값을 읽음
    Serial.print("Current Value: ");
    Serial.println(value);

    // 압력 센서의 값이 초기 값보다 150 이상 상승했을 때
    if (value >= initialValue + 150) {
        Serial.println("Value exceeded threshold, sending RTC...");
        sendRTC();
        delay(5000);  // 5초 대기 (중복 전송 방지)
    }

    // 압력 센서의 값이 초기 값보다 200 이상 하락했을 때
    if (value <= initialValue - 200) {
        Serial.println("Value dropped below threshold, sending RTC...");
        sendRTC();
        delay(5000);  // 5초 대기 (중복 전송 방지)
    }

    delay(1000);  // 1초마다 센서 값 확인
}
