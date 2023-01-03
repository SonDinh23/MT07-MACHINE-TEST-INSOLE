#ifndef TEST_INSOLE_H
#define TEST_INSOLE_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <ESP32Servo.h>
#include "RTClib.h"

#define i2c_Address 0x3c

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     //   QT-PY / XIAO

#define ANGLEMAX 65
#define ANGLEMIN 15
#define ANGLETB 35
#define SPEED 8

#define CLK 27
#define DT 25
#define SW 26

#define INSOLE_1 32
#define INSOLE_2 33

#define SERVO 4

class TestInsole
{
public:
    uint8_t stateMH = 0;
    TestInsole(/* args */);
    void begin();
    void setup();
    void screenIntro();

    void printTriangle(uint8_t st);
    uint8_t updateEncoder();
    void printTargetset(int16_t countTargetset);
    void targetCount (uint8_t x, uint8_t y, uint16_t valuelast, uint16_t value);
    void printCountTarget1(int16_t demTarget);
    void printCountTarget2(int16_t demTarget);
    uint16_t testInsole_1();
    uint16_t testInsole_2();
    void printInsole_1(uint16_t countIns1);
    void printInsole_2(uint16_t countIns2);
    void printMonth(uint8_t x, uint8_t y, uint16_t lastValue, uint16_t value);
    void printDay(uint8_t x, uint8_t y, uint16_t lastValue, uint16_t value);
    void printHour(uint8_t x, uint8_t y, uint16_t lastValue, uint16_t value);
    void printMinutes(uint8_t x, uint8_t y, uint16_t lastValue, uint16_t value);
    void printRtcNow(uint8_t month, uint8_t day, uint8_t hour, uint8_t minute);
    void printRtcStart(uint8_t month, uint8_t day, uint8_t hour, uint8_t minute);
    void printRunTime( uint8_t hour, uint8_t minute);
    void setServo();
    void setServoTB();

    void display1();
    void display2();
    void display3();
    //~TestInsole();
private:
    RTC_DS1307 rtc;

    Servo myservo;

    Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    // Rotary Encoder Inputs
    
    uint8_t stateTG = 1;

    int counter = 0;
    int currentStateCLK;
    int lastStateCLK;
    String currentDir = "";
    unsigned long lastButtonPress = 0;

    uint16_t countInsole_1 = 0;
    uint16_t countInsole_2 = 0;

    uint8_t m = 0;
    uint8_t n = 0;


    uint8_t stateInsoleError = 0;

    uint8_t thangStr = 0;
    uint16_t ngayStr = 0;
    uint16_t gioStr = 0;
    uint16_t phutStr = 0;

    uint8_t runGio = 0;
    uint8_t runPhut = 0;

    uint8_t thang = 0;
    uint8_t ngay = 0;
    uint8_t gio = 0;
    uint8_t phut = 0;
};

#endif