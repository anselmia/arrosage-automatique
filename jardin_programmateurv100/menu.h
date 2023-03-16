#ifndef MENU_h
#define MENU_h

#include "myeeprom.h"
#include "clock.h";

class MENU
{
private:
public:
    MENU();
    int rtc[7];
    int rtc_min;
    int rtc_day;
    int actualScreen;
    int actualLine;
    int cursorPos;
    int action;
    int screenValue;
    void initClock();
    void getClock();
    void prinheu();
    void prinheu2();
    void forward();
    void backward();
    void up();
    void down();
    void selectEV(int ev);
    void updateValue(int dir, int value = 0);
    void moveCursor();
    MYEEPROM eeprom = MYEEPROM();
    CLOCK clock = CLOCK();
    int selectedEV;
    int delay;
    int stop;
    int manual;
    int inactive;
};
#endif