#ifndef MENU_h
#define MENU_h

#include "m_item.h"
#include "myeeprom.h"

class MENU
{
private:
public:
    MENU();
    int rtc[7];
    int rtc_min;
    int rtc_day;
    int ecran_princ_min;
    int ecran_princ_sec;
    int actualScreen;
    int actualLine;
    int cursorPos;
    M_ITEM items[39];
    void initClock();
    void getClock();
    void prinheu();
    void prinheu2();
    int forward();
    int backward();
    int up();
    int down();
    int updateValue(int dir);
    void moveCursor();
    M_ITEM getItem();
    MYEEPROM eeprom = MYEEPROM();
    int selectedEV;
};
#endif