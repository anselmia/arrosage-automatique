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
    int action;
    void initClock();
    void getClock();
    void prinheu();
    void prinheu2();
    void forward();
    void backward();
    void up();
    void down();
    void selectEV(int ev);
    void updateValue(int dir);
    void moveCursor();
    MYEEPROM eeprom = MYEEPROM();
    int selectedEV;
};
#endif