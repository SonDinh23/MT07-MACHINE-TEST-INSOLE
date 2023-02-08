#include "TestInsole.h"

static const unsigned char PROGMEM logo16_glcd_bmp[] = { 0xff, 0xf0, 0xfd, 0xf0, 0xf8, 0xf0, 0xf8, 0xf0, 0xf8, 0x70, 0x9a, 0x70, 0xdf, 0x30, 0xcf, 0x30,
                                                         0xed, 0x90, 0xe1, 0x90, 0xf1, 0xf0, 0xf3, 0xf0, 0xfb, 0xf0, 0xff, 0xf0 };

static const unsigned char PROGMEM logo[] = { 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xf1, 0xff, 0xff, 0xe1, 0xff, 0xff,
                                              0xe0, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xc0, 0x7f, 0xff, 0x80, 0x7f, 0x87, 0x80, 0x3f, 0xc7, 0xc4,
                                              0x3f, 0xc3, 0xce, 0x1f, 0xe3, 0xee, 0x1f, 0xe1, 0xff, 0x0f, 0xf1, 0xff, 0x0f, 0xf0, 0xff, 0x87,
                                              0xf8, 0x7f, 0x87, 0xf8, 0x77, 0xc3, 0xfc, 0x33, 0xc3, 0xfc, 0x23, 0xe1, 0xfe, 0x03, 0xff, 0xfe,
                                              0x03, 0xff, 0xff, 0x07, 0xff, 0xff, 0x87, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xdf,
                                              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

TestInsole::TestInsole(/* args */)
{
}

void TestInsole::begin() {
    pinMode(CLK, INPUT);
    pinMode(DT, INPUT);
    pinMode(SW, INPUT_PULLUP);

    pinMode(INSOLE_1, INPUT_PULLUP);
    pinMode(INSOLE_2, INPUT_PULLUP);
    myservo.attach(SERVO,1000,2000);
    display.begin(i2c_Address, true);
}

void TestInsole::setup() {
    //uint32_t lastTime = millis();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
        // if(millis() - lastTime > 5000) {
        //     break;
        //     lastTime = millis(); 
        // }
    }
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);
    Serial.println("Connection");
    if(!Firebase.beginStream(firebaseDate, path)) {
        Serial.println("Reason: "+firebaseDate.errorReason());
        Serial.println();
    }
  
    Serial.print("ok ");

    rtc.begin();
    if (!rtc.isrunning()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
}

void TestInsole::screenIntro() {
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.drawRoundRect(0, 0, 128, 64, 8, SH110X_WHITE);
    display.display();
    display.drawBitmap(25, 15, logo, 24, 29, 1);
    display.setTextSize(1);
    display.setCursor(55, 23);
    display.setTextColor(SH110X_WHITE);
    display.print(F("VULCAN"));
    display.setCursor(55, 33);
    display.print(F("AUGMETICS"));
    display.display();
    delay(1);
    lastStateCLK = digitalRead(CLK);
    stateMH = 0;
    //setServoTB();
}

void TestInsole::printTriangle(uint8_t st) {
    if (st == 1) {
        display.fillTriangle(10, 50, 10, 60, 20, 55, SH110X_BLACK);
        display.display();
        display.fillTriangle(10, 30, 10, 40, 20, 35, SH110X_WHITE);
        display.display();
    } else if (st == 2) {
        display.fillTriangle(10, 50, 10, 60, 20, 55, SH110X_WHITE);
        display.display();
        display.fillTriangle(10, 30, 10, 40, 20, 35, SH110X_BLACK);
        display.display();
  }
}

uint8_t TestInsole::updateEncoder() {
    // Read the current state of CLK
    currentStateCLK = digitalRead(CLK);

    // If last and current state of CLK are different, then pulse occurred
    // React to only 1 state change to avoid double count
    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

        // If the DT state is different than the CLK state then
        // the encoder is rotating CCW so decrement
        if (digitalRead(DT) != currentStateCLK) {
            counter++;
            stateTG = 1;
        } else {
        // Encoder is rotating CW so increment
        counter--;
        stateTG = 2;
    }
    
  }

  // Remember last CLK state
  lastStateCLK = currentStateCLK;
  delay(1);
  return stateTG;
}

void TestInsole::printTargetset(int16_t countTargetset) {
    static int16_t lastCountTargetset = -1;
    if (lastCountTargetset != countTargetset) {
        // in lastCount1 mau nen
        display.setCursor(30, 37);
        display.setTextColor(SH110X_BLACK);
        display.print(lastCountTargetset);
        display.display();
        // in count1 mau mong muon
        display.setCursor(30, 37);
        display.setTextColor(SH110X_WHITE);
        display.print(countTargetset);
        display.display();
        lastCountTargetset = countTargetset;
    }
}

void TestInsole::targetCount(uint8_t x, uint8_t y, uint16_t valuelast, uint16_t value) {
    display.setCursor(x, y);
    display.setTextColor(SH110X_BLACK);
    display.print(valuelast);
    display.display();
    // in count1 mau mong muon
    display.setCursor(x, y);
    display.setTextColor(SH110X_WHITE);
    display.print(value);
    display.display();
}

void TestInsole::printCountTarget1(int16_t demTarget) {
    static int16_t lastDemTargetset = -1;

    if (lastDemTargetset != demTarget) {
        // in lastCount1 mau nen
        targetCount(83, 37, lastDemTargetset, demTarget);
        lastDemTargetset = demTarget;
    }
}

void TestInsole::printCountTarget2(int16_t demTarget) {
    static int16_t lastDemTargetset1 = -1;

    if (lastDemTargetset1 != demTarget) {
        // in lastCount1 mau nen
        targetCount(90, 37, lastDemTargetset1, demTarget);
        lastDemTargetset1 = demTarget;
    }
}

uint16_t TestInsole::testInsole_1() {
    if (digitalRead(INSOLE_1) == 0) {
        if (m == 0) {
            countInsole_1++;
            m = 1;
            stateInsoleError = 0;
        }
    } else if (digitalRead(INSOLE_1) == 1) {
        m = 0;
    }
    return countInsole_1;
}

uint16_t TestInsole::testInsole_2() {
    if (digitalRead(INSOLE_2) == 0) {
        if (n == 0) {
            countInsole_2++;
            n = 1;
            stateInsoleError = 0;
        }
    } else if (digitalRead(INSOLE_2) == 1) {
        n = 0;
    }
    return countInsole_2;
}

void TestInsole::printInsole_1(uint16_t countIns1) {
    static int16_t lastCount1 = -1;
    //Serial.println(countIns1);
    if (lastCount1 != countIns1) {
        // in lastCount1 mau nen
        display.setCursor(12, 55);
        display.setTextColor(SH110X_BLACK);
        display.print(lastCount1);
        display.display();
        // in count1 mau mong muon
        display.setCursor(12, 55);
        display.setTextColor(SH110X_WHITE);
        display.print(countIns1);
        display.display();
        lastCount1 = countIns1;
    }
}

void TestInsole::printInsole_2(uint16_t countIns2) {
    static int16_t lastCount2 = -1;

    if (lastCount2 != countIns2) {
    // in lastCount1 mau nen
        display.setCursor(82, 55);
        display.setTextColor(SH110X_BLACK);
        display.print(lastCount2);
        display.display();
    // in count1 mau mong muon
        display.setCursor(82, 55);
        display.setTextColor(SH110X_WHITE);
        display.print(countIns2);
        display.display();
        lastCount2 = countIns2;
    }
}

void TestInsole::printMonth(uint8_t x, uint8_t y, uint16_t lastValue, uint16_t value) {
    display.setCursor(x, y);
    display.setTextColor(SH110X_BLACK);
    display.print(lastValue);
    display.display();
    // in count1 mau mong muon
    display.setCursor(x, y);
    display.setTextColor(SH110X_WHITE);
    display.print(value);
    display.display();
}

void TestInsole::printDay(uint8_t x, uint8_t y, uint16_t lastValue, uint16_t value) {
    display.setCursor(x, y);
    display.setTextColor(SH110X_BLACK);
    display.print(lastValue);
    display.display();
    // in count1 mau mong muon
    display.setCursor(x, y);
    display.setTextColor(SH110X_WHITE);
    display.print(value);
    display.display();
}

void TestInsole::printHour(uint8_t x, uint8_t y, uint16_t lastValue, uint16_t value) {
    display.setCursor(x, y);
    display.setTextColor(SH110X_BLACK);
    display.print(lastValue);
    display.display();
    // in count1 mau mong muon
    display.setCursor(x, y);
    display.setTextColor(SH110X_WHITE);
    display.print(value);
    display.display();
}

void TestInsole::printMinutes(uint8_t x, uint8_t y, uint16_t lastValue, uint16_t value) {
    display.setCursor(x, y);
    display.setTextColor(SH110X_BLACK);
    display.print(lastValue);
    display.display();
    // in count1 mau mong muon
    display.setCursor(x, y);
    display.setTextColor(SH110X_WHITE);
    display.print(value);
    display.display();
}

void TestInsole::printRtcNow(uint8_t month, uint8_t day, uint8_t hour, uint8_t minute) {
    if (month != thang) {
        printMonth(30, 20, thang, month);
        thang = month;
    }

    if (day != ngay) {
        printDay(50, 20, ngay, day);
        ngay = day;
    }

    if (hour != gio) {
        printHour(83, 20, gio, hour);
        gio = hour;
    }

    if (minute != phut) {
        printMinutes(103, 20, phut, minute);
        phut = minute;
    }
}

void TestInsole::printRtcStart(uint8_t month, uint8_t day, uint8_t hour, uint8_t minute) {

    if (month != strThang) {
        printMonth(40, 20, strThang, month);
        strThang = month;
    }

    if (day != strNgay) {
        printMonth(60, 20, strNgay, day);
        strNgay = day;
    }

    if (hour != strGio) {
        printMonth(93, 20, strGio, hour);
        strGio = hour;
    }

    if (minute != strPhut) {
        printMonth(113, 20, strPhut, minute);
        strPhut = minute;
    }
}

void TestInsole::printRunTime( uint8_t hour, uint8_t minute) {

    if (hour != lastRunGio) {
        printMonth(40, 37, lastRunGio, hour);
        lastRunGio = hour;
    }

    if (minute != lastRunPhut) {
        printMonth(65, 37, lastRunPhut, minute);
        lastRunPhut = minute;
    }
}

void TestInsole::setServo() {
    for (int j = ANGLEMIN; j < ANGLEMAX; j++) {
        myservo.write(j);
        printInsole_1(testInsole_1());
        printInsole_2(testInsole_2());
        delay(SPEED);
    }
    for (int r = ANGLEMAX; r > ANGLEMIN; r--) {
        myservo.write(r);
        printInsole_1(testInsole_1());
        printInsole_2(testInsole_2());
        delay(SPEED);
    }
}

void TestInsole::setServoTB() {
    for (int k = ANGLEMIN; k < ANGLETB; k++) {
        myservo.write(k);
        delay(SPEED);
    }
}

void TestInsole::display1() {
    setServoTB();
    //display.drawBitmap(0, 50, logo16_glcd_bmp, 12, 14, 1);
    Firebase.setString(firebaseDate, path + "/btnStart1", 0);
    Firebase.setString(firebaseDate, path + "/btnPause1", 0);
    Firebase.setString(firebaseDate, path + "/btnStop1", 0);

    Firebase.setString(firebaseDate, path + "/btnStart2", 0);
    Firebase.setString(firebaseDate, path + "/btnPause2", 0);
    Firebase.setString(firebaseDate, path + "/btnStop2", 0);

    Firebase.setInt(firebaseDate, path + "/countdata1", 0);
    Firebase.setInt(firebaseDate, path + "/countdata2", 0);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(5, 0);
    display.print("InsoleTest");
    display.drawRect(5, 20, 120, 1, SH110X_WHITE);
    display.setTextSize(1);
    display.setCursor(45, 30);
    display.println("Test 1");
    display.setCursor(45, 52);
    display.println("Test 2");
    display.display();
    uint32_t lastTime = millis();
    while (true) {
        int8_t tamgiac = updateEncoder();
        printTriangle(tamgiac);
        if (digitalRead(SW) == 0) {
            if(millis() - lastTime > 80) {
                stateMH = tamgiac;
                break;
            }
        }
    }
}

void TestInsole::display2() {
    
    display.clearDisplay();
    display.setCursor(4, 0);
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.print("VULCAN AUGEMETICS");
    display.drawRect(4, 10, 100, 1, SH110X_WHITE);
    display.display();

    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 20);
    display.print("Now:");

    display.setCursor(43, 20);
    display.print("/");
    display.setCursor(68, 20);
    display.print("||");
    display.setCursor(96, 20);
    display.print(":");

    display.setCursor(75, 37);
    display.print("/");

    display.setTextSize(1);
    display.setCursor(0, 37);
    display.println("TSet:");
    display.setCursor(0, 55);
    display.println("L:");
    display.setCursor(70, 55);
    display.println("R:");
    display.display();
    
    counter = 0;
    uint16_t count1 = 0;
    uint32_t lastTime = millis();
    uint32_t lastTimeA = millis();
    uint32_t lastTimeB = millis();
    uint32_t lastTimeC = millis();
    uint32_t lastTimeD = millis();
    uint32_t lastTimeE = millis();
    uint32_t lastTimeF = millis();
    uint32_t lastTimeG = millis();
    uint32_t lastTimeH = millis();
    uint32_t lastTimeI = millis();
  
    uint8_t stateStartTest1 = 0;
    uint8_t statePauseTest1 = 0;
    bool statePauseBool = false;
    
    DateTime now = rtc.now();

    thang = 0;
    ngay = 0;
    gio = 0;
    phut = 0;
    
    countInsole_1 = 0;
    countInsole_2 = 0;
    printInsole_1(countInsole_1);
    printInsole_2(countInsole_2);
    
    String x;
    String y;
    String z;

    int8_t e;
    int8_t u;
    int8_t t;
    while (true) {

        now = rtc.now();
        printRtcNow(now.month(), now.day(), now.hour(), now.minute());
        
        updateEncoder();

        if (counter < 0) counter = 0;
        count1 = counter * 100;

        printTargetset(count1);

        if(millis() - lastTime > 3000) {
            Firebase.setInt(firebaseDate, path + "/data1", count1);
            lastTime = millis();
        }

        if(millis() - lastTimeA > 1500) {
            if(Firebase.getString(firebaseDate, path + "/btnStart1")) x = firebaseDate.stringData();
            e = x.toInt();
            //Serial.println(e);
            lastTimeA = millis();
        }
        uint16_t data = count1;

        if(e == 1) {
            stateStartTest1 = 2;
            countInsole_1 = 0;
            countInsole_2 = 0;
        }

        while (digitalRead(SW) == 0) {
            if(millis() - lastTimeF > 80) {
                stateStartTest1++;
                Serial.println(stateStartTest1);
                lastTimeF = millis();
            }
            
            countInsole_1 = 0;
            countInsole_2 = 0;
        }

        if ((stateStartTest1 >= 1) && (stateStartTest1 < 5)) {
            //Serial.println("alo");
            Firebase.setString(firebaseDate, path + "/btnStart1", 0);
            stateStartTest1 = 0;
        
            uint16_t dem1 = 0;
            for (int i = 0; i < data; i++) {
                now = rtc.now();
                printRtcNow(now.month(), now.day(), now.hour(), now.minute());

                setServo();
                // Serial.println(testInsole_1());
                // Serial.println(testInsole_2());
                if (millis() - lastTimeB > 500) {
                    Firebase.setInt(firebaseDate, path + "/datacount1R", testInsole_1());
                    Firebase.setInt(firebaseDate, path + "/datacount1L", testInsole_2());
                    lastTimeB = millis();
                }

                dem1++;
                if(millis() - lastTimeC > 2000) {
                    float ml = (float) dem1/data * 100.0;
                    int ap = (int)ml;
                    Firebase.setInt(firebaseDate, path + "/countdata1", dem1);
                    Firebase.setInt(firebaseDate, path + "/data", ml);
                    lastTimeC = millis();
                }
                printCountTarget1(dem1);
        
                if ((digitalRead(INSOLE_1) == 1) && (digitalRead(INSOLE_2) == 1)) {
                    if (millis() - lastTimeG > 400) {
                        stateInsoleError++;
                        lastTimeG = millis();
                    }
                    if (stateInsoleError > 5) {
                        stateInsoleError = 5;
                        break;
                    }
                }
                if (stateInsoleError == 5) {
                    statePauseBool = true;

                    setServoTB();

                    while (statePauseBool == true) {
                        display.drawBitmap(115, 0, logo16_glcd_bmp, 12, 14, SH110X_WHITE);
                        display.display();

                        if(Firebase.getString(firebaseDate, path + "/btnPause1")) y = firebaseDate.stringData();
                        u = y.toInt();
                        Serial.println(u);
                        if(u == 1) {
                            statePauseTest1 = 0;
                            Firebase.setInt(firebaseDate, path + "/btnPause1", 0);
                            break;
                        }

                        if(Firebase.getString(firebaseDate, path + "/btnStop1")) z = firebaseDate.stringData();
                        t = z.toInt();
                        Serial.println(t);
                        if(t == 1) {
                            statePauseTest1 = 0;
                            stateStartTest1 = 7;
                            Firebase.setInt(firebaseDate, path + "/btnStop1", 0);
                            break;
                        }

                        while (digitalRead(SW) == 0) {
                            if (millis() - lastTimeH > 400) {
                                statePauseBool = false;
                                stateInsoleError = 0;
                                stateStartTest1++;
                                lastTimeH = millis();
                            }
                        }
                    }
                    display.drawBitmap(115, 0, logo16_glcd_bmp, 12, 14, 0);
                    display.display();
                }

                if(millis() - lastTimeE > 1000) {
                    if(Firebase.getString(firebaseDate, path + "/btnStop1")) z = firebaseDate.stringData();
                    t = z.toInt();
                    //Serial.println(t);
                    if(t == 1) {
                        stateStartTest1 = 7;
                        Firebase.setString(firebaseDate, path + "/btnStop1", 0);
                        break;
                    }
                    lastTimeE = millis();
                }

                //Serial.println("chao");
                if(millis() - lastTimeD > 1000) {
                    if(Firebase.getString(firebaseDate, path + "/btnPause1")) y = firebaseDate.stringData();
                    u = y.toInt();
                    //Serial.println(u);
                    lastTimeD = millis();
                }
                if(u == 1) {
                    statePauseTest1 = 1;
                }

        
                while (digitalRead(SW) == 0) {
                    statePauseTest1 = 1;
                }

                if (statePauseTest1 == 1) {
                    Firebase.setString(firebaseDate, path + "/btnPause1", 0);
                    statePauseBool = true;

                    setServoTB();

                    while (statePauseBool == true) {
                        display.drawBitmap(115, 0, logo16_glcd_bmp, 12, 14, SH110X_WHITE);
                        display.display();

                        now = rtc.now();
                        printRtcNow(now.month(), now.day(), now.hour(), now.minute());

                        if(Firebase.getString(firebaseDate, path + "/btnPause1")) y = firebaseDate.stringData();
                        u = y.toInt();
                        //Serial.println(u);
                        if(u == 1) {
                            statePauseTest1 = 0;
                            Firebase.setString(firebaseDate, path + "/btnPause1", 0);
                            break;
                        }

                        if(Firebase.getString(firebaseDate, path + "/btnStop1")) z = firebaseDate.stringData();
                        t = z.toInt();
                        //Serial.println(t);
                        if(t == 1) {
                            statePauseTest1 = 0;
                            stateStartTest1 = 7;
                            Firebase.setString(firebaseDate, path + "/btnStop1", 0);
                            break;
                        }

                        while (digitalRead(SW) == 0) {
                            if (millis() - lastTimeI > 300) {
                                statePauseBool = false;
                                statePauseTest1 = 0;
                                stateStartTest1++;
                                Serial.println(stateStartTest1);
                                lastTimeI = millis();
                            }
                        }
                    }
                    display.drawBitmap(115, 0, logo16_glcd_bmp, 12, 14, SH110X_BLACK);
                    display.display();
                }
                if (stateStartTest1 >= 5) {
                    break;
                }
            }
            setServoTB();
        }
        if (stateStartTest1 >= 5) {
            stateMH = 0;
            break;
        }
    }
}

void TestInsole::display3() {
    display.clearDisplay();

    display.setCursor(4, 0);
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.print("VULCAN AUGEMETICS");
    display.drawRect(4, 10, 100, 1, SH110X_WHITE);
    display.display();

    display.setCursor(53, 20);
    display.print("/");
    display.setCursor(76, 20);
    display.print("||");
    display.setCursor(105, 20);
    display.print(":");

    display.setCursor(53, 37);
    display.print(":");
    display.setCursor(76, 37);
    display.print("||");

    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 20);
    display.print("Str.T:");
    display.setTextSize(1);
    display.setCursor(0, 37);
    display.println("Run.T:");
    display.setCursor(0, 55);
    display.println("L:");
    display.setCursor(70, 55);
    display.println("R:");
    display.display();

    uint32_t lastTime = millis();
    uint32_t lastTimeA = millis();
    uint32_t lastTimeB = millis();
    uint32_t lastTimeC = millis();
    uint32_t lastTimeD = millis();
    uint32_t lastTimeE = millis();
    uint32_t lastTimeF = millis();
    uint32_t lastTimeG = millis();
    uint32_t lastTimeH = millis();
    uint32_t lastTimeI = millis();


    DateTime now = rtc.now();

    uint8_t stateStartTest2 = 0;
    uint8_t statePauseTest2 = 0;
    bool statePauseBool = false;

    lastRunGio = -1;
    lastRunPhut = -1;

    runGio = 0;
    runPhut = 0;

    strThang = -1;
    strNgay = -1;
    strGio = -1;
    strPhut = -1;

    thangStr = 0;
    ngayStr = 0;
    gioStr = 0;
    phutStr = 0;

    printRtcStart(thangStr, ngayStr, gioStr, phutStr);
    printRunTime(runGio, runPhut);

    uint16_t tempTime = 0;
    uint16_t tempTimeStart = 0;
    uint16_t tempTimeNow = 0;
    uint16_t tempTimePause = 0;

    uint16_t minutesPause = 0;
    uint16_t hourPause = 0;
    uint16_t dayPause = 0;

    uint16_t tempRun = 0;
    uint16_t ul = 0;

    uint16_t nowDay = 0;
    uint16_t nowHour = 0;
    uint16_t nowMinute = 0;

    uint16_t tempPause = 0;

    uint16_t tempPause2 = 0;

    String x;
    String y;
    String z;

    int8_t w;
    int8_t e;
    int8_t h;

    while (true) {

        now = rtc.now();

        while (digitalRead(SW) == 0) {
            if(millis() - lastTime > 200) {
                stateStartTest2++;
                Serial.println(stateStartTest2);
                lastTime = millis();
            }

            now = rtc.now();

            thangStr = now.month();
            ngayStr = now.day();
            gioStr = now.hour();
            phutStr = now.minute();

            printRtcStart(thangStr, ngayStr, gioStr, phutStr);
            tempTimeStart = (((ngayStr*24)*60) + (gioStr*60) + phutStr);
            countInsole_1 = 0;
            countInsole_2 = 0;
            //Serial.println("In thoi gian start:");
            //Serial.println(tempTimeStart);
        }

        if(millis() - lastTimeA > 1000) {
            if(Firebase.getString(firebaseDate, path + "/btnStart2")) x = firebaseDate.stringData();
            w = x.toInt();
            //Serial.println(w);
            lastTimeA = millis();
        }
        if(w == 1) {
            stateStartTest2 = 2;

            now = rtc.now();

            thangStr = now.month();
            ngayStr = now.day();
            gioStr = now.hour();
            phutStr = now.minute();

            printRtcStart(thangStr, ngayStr, gioStr, phutStr);
            tempTimeStart = (((ngayStr*24)*60) + (gioStr*60) + phutStr);

            countInsole_1 = 0;
            countInsole_2 = 0;
        }

        if ((stateStartTest2 >= 1) && (stateStartTest2 < 5)) {

            Firebase.setString(firebaseDate, path + "/btnStart2", 0);
            stateStartTest2 = 0;
            uint16_t dem2 = 0;

            for (;;) {
                //if(millis() - lastTime > 50) {
                    now = rtc.now();
                    nowDay = now.day();
                    nowHour = now.hour();
                    nowMinute = now.minute();

                    tempTimeNow = (((nowDay*24)*60) + (nowHour*60) + nowMinute);

                    if(tempTimePause != 0) {
                        // tempTimeStart = tempTimeStart - tempTimePause;
                        tempPause = tempTimeNow - tempTimePause;
                        tempTimePause = 0;
                        tempPause2 = tempPause2 + tempPause; 
                    }
                    //Serial.println(tempPause2);
                    tempTime = tempTimeNow - tempTimeStart;
                    tempRun = tempTime - tempPause2;
                    // tempPause = 0;
                    //  Serial.println("In thoi gian tru:");
                    //  Serial.println(tempRun);
                    while(tempRun >= 60) {
                        tempRun = tempRun - 60;
                        ul++;
                    }
                    //Serial.println(tempRun);
                    printRunTime(ul, tempRun);
                    //lastTime = millis();
                //}
                if(millis() - lastTimeI > 500) {
                    Firebase.setInt(firebaseDate, path + "/runtimeH", ul);
                    Firebase.setInt(firebaseDate, path + "/runtimeM", tempRun);
                    lastTimeI = millis();
                }
        

                setServo();

                if(millis() - lastTimeB > 1000) {
                    Firebase.setInt(firebaseDate, path + "/datacount2R", testInsole_1());
                    Firebase.setInt(firebaseDate, path + "/datacount2L", testInsole_2());
                    lastTimeB = millis();
                }

                dem2++;
                if(millis() - lastTimeC > 2000) {
                    Firebase.setInt(firebaseDate, path + "/countdata2", dem2);
                    lastTimeC = millis();
                }
                printCountTarget2(dem2);

                if ((digitalRead(INSOLE_1) == 1) && (digitalRead(INSOLE_2) == 1)) {
                  if(millis() - lastTimeG > 400) {
                    stateInsoleError++;
                    //Serial.println(stateInsoleError);
                    lastTimeG = millis();
                  }
                  if(stateInsoleError > 5) {
                    stateInsoleError = 5;
                    Serial.println("out");
                    break;
                  }
                }
                if (stateInsoleError == 5) {
                  statePauseBool = true;

                  setServoTB();

                  while (statePauseBool == true) {
                    display.drawBitmap(115, 0, logo16_glcd_bmp, 12, 14, SH110X_WHITE);
                    display.display();

                    if(Firebase.getString(firebaseDate, path + "/btnPause2")) y = firebaseDate.stringData();
                    e = y.toInt();
                    //Serial.println(e);
                    if(e == 1) {
                      statePauseTest2 = 0;
                      Firebase.setString(firebaseDate, path + "/btnPause2", 0);
                      break;
                    }

                    if(Firebase.getString(firebaseDate, path + "/btnStop2")) z = firebaseDate.stringData();
                    h = z.toInt();
                    //Serial.println(h);
                    if(h == 1) {
                      statePauseTest2 = 0;
                      stateStartTest2 = 7;
                      Firebase.setString(firebaseDate, path + "/btnStop2", 0);
                      break;
                    }

                    while (digitalRead(SW) == 0) {
                      if(millis() - lastTimeH > 400) {
                        statePauseBool = false;
                        stateInsoleError = 0;
                        stateStartTest2++;
                        lastTimeH = millis();
                      }
                    }
                  }
                  display.drawBitmap(115, 0, logo16_glcd_bmp, 12, 14, 0);
                  display.display();
                }

                //Serial.println("chao");
                if(millis() - lastTimeD > 500) {
                    if(Firebase.getString(firebaseDate, path + "/btnPause2")) y = firebaseDate.stringData();
                    e = y.toInt();
                    //Serial.println(e);
                    lastTimeD = millis();
                }
                if(e == 1) {
                    statePauseTest2 = 1;
                }

                if(millis() - lastTimeE > 500) {
                    if(Firebase.getString(firebaseDate, path + "/btnStop2")) z = firebaseDate.stringData();
                    h = z.toInt();
                    //Serial.println(h);
                    if(h == 1) {
                        stateStartTest2 = 7;
                        Firebase.setString(firebaseDate, path + "/btnStop2", 0);
                        break;
                    }
                    lastTimeE = millis();
                }        

                while (digitalRead(SW) == 0) {
                    statePauseTest2 = 1;
                }

                if (statePauseTest2 == 1) {
                    Firebase.setString(firebaseDate, path + "/btnPause2", 0);
                    statePauseBool = true;

                    setServoTB();

                    now = rtc.now();
                    minutesPause = now.minute();
                    hourPause = now.hour();
                    dayPause = now.day();

                    tempTimePause = (((dayPause*24)*60) + (hourPause*60) + minutesPause);
                    //Serial.println("In thoi gian pause:");
                    //Serial.println(tempTimeStart);

                    while (statePauseBool == true) {
                        display.drawBitmap(115, 0, logo16_glcd_bmp, 12, 14, SH110X_WHITE);
                        display.display();

                        if(Firebase.getString(firebaseDate, path + "/btnPause2")) y = firebaseDate.stringData();
                        e = y.toInt();
                        //Serial.println(e);
                        if(e == 1) {
                            statePauseTest2 = 0;
                            Firebase.setString(firebaseDate, path + "/btnPause2", 0);
                            break;
                        }

                        if(Firebase.getString(firebaseDate, path + "/btnStop2")) z = firebaseDate.stringData();
                        h = z.toInt();
                        //Serial.println(h);
                        if(h == 1) {
                            statePauseTest2 = 0;
                            stateStartTest2 = 7;
                            Firebase.setString(firebaseDate, path + "/btnStop2", 0);
                            break;
                        }

                        while (digitalRead(SW) == 0) {
                            if(millis() - lastTimeF > 300) {
                                statePauseBool = false;
                                statePauseTest2 = 0;
                                stateStartTest2++;
                                Serial.println(stateStartTest2);
                                lastTimeF = millis();
                            }
                        }
                    }
                    display.drawBitmap(115, 0, logo16_glcd_bmp, 12, 14, 0);
                    display.display();
          
                }
                if (stateStartTest2 > 5) {
                    break;
                }
            }
            setServoTB();
        } else if (stateStartTest2 > 5) {
            stateMH = 0;
            break;
        }
    }
}