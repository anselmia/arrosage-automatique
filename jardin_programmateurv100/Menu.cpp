#include "menu.h"
#include "m_item.h"
#include "myeeprom.h"
#include <DS1307.h>

MENU::MENU()
{
    MYEEPROM eeprom = MYEEPROM();
    int rtc[7];
    int rtc_min = 0;
    int rtc_day = 0;
    int ecran_princ_min = 0;
    int ecran_princ_sec = 0;
    actualScreen = 0;
    actualLine = 0;
    cursorPos = 0;
    selectedEV = 0;
    action = 1;
}

void MENU::initClock()
{
    getClock();
    RTC.SetOutput(DS1307_SQW32KHZ);
}

void MENU::getClock()
{
    RTC.get(rtc, true);
}

void MENU::prinheu()
{
    ecran_princ_min = rtc[1] + 2;
    if (ecran_princ_min >= 60)
    {
        ecran_princ_min = 0;
    }
    ecran_princ_sec = rtc[0];
}

void MENU::prinheu2()
{
    prinheu();
}

void MENU::forward()
{
    action = -1;
    switch (actualScreen)
    {
    case 0:
        previousScreen = actualScreen;
        actualScreen = 1;
        actualLine = 0;
        action = 1;
        break;
    case 1:
        previousScreen = actualScreen;
        previousLine = actualLine;
        switch (actualLine)
        {
        case 0:
            actualScreen = 2;
            actualLine = 1;
            action = 1;
            break;
        case 1:
            actualScreen = 8;
            actualLine = 2;
            selectedEV = 1;
            action = 1;
            break;
        case 2:
            actualScreen = 9;
            selectedEV = 1;
            actualLine = 2;
            action = 1;
            break;
        case 3:
            actualScreen = 10;
            selectedEV = 1;
            actualLine = 2;
            action = 1;
            break;
        case 4:
            actualScreen = 11;
            selectedEV = 1;
            actualLine = 2;
            action = 1;
            break;
        }
        break;
    case 2:
        previousScreen = actualScreen;
        switch (actualLine)
        {
        case 1:
            actualScreen = 4;
            actualLine = 1;
            action = 1;
            break;
        case 2:
            actualScreen = 5;
            actualLine = 1;
            action = 1;
            break;
        case 3:
            cursorPos = 0;
            actualScreen = 6;
            actualLine = 1;
            action = 1;
            break;
        case 4:
            actualScreen = 7;
            actualLine = 1;
            action = 1;
            break;
        }
        break;
    case 6:
        moveCursor();
        action = 0;
        break;
    }
}

void MENU::backward()
{
    action = -1;
    switch (actualScreen)
    {
    case 1:
        actualScreen = 0;
        actualLine = 0;
        action = 1;
        break;
    case 2:
    case 8:
    case 9:
    case 10:
    case 11:
        actualScreen = 1;
        actualLine = 1;
        action = 1;
        break;

    case 4:
    case 5:
    case 6:
    case 7:
        actualScreen = 2;
        actualLine = 1;
        action = 1;
        break;
    }
}

void MENU::up()
{
    action = -1;
    switch (actualScreen)
    {
    case 1:
        action = 0;
        if (actualLine == 0)
            actualLine = 4;
        else
            actualLine--;
        break;
    case 2:
    case 4:
    case 5:
        action = 0;
        if (actualLine == 1)
            actualLine = 4;
        else
            actualLine--;
        break;
    case 6:
        action = 0;
        if (actualLine == 1)
            actualLine = 3;
        else
            actualLine = 1;
        cursorPos = 0;
        break;
    case 7:
        if (actualLine == 1)
        {
            actualLine = 7;
            action = 1;
        }
        else if (actualLine == 6)
        {
            actualLine--;
            action = 1;
        }
        else
        {
            actualLine--;
            action = 0;
        }
        break;
    }
}

void MENU::down()
{
    action = -1;
    switch (actualScreen)
    {
    case 1:
        action = 0;
        if (actualLine == 4)
            actualLine = 0;
        else
            actualLine++;
        break;
    case 2:
    case 4:
    case 5:
        action = 0;
        if (actualLine == 4)
            actualLine = 1;
        else
            actualLine++;
        break;
    case 6:
        action = 0;
        if (actualLine == 1)
            actualLine = 3;
        else
            actualLine = 1;
        cursorPos = 0;
        break;
    case 7:
        if (actualLine == 7)
        {
            actualLine = 1;
            action = 1;
        }
        else if (actualLine == 4)
        {
            actualLine++;
            action = 1;
        }
        else
        {
            actualLine++;
            action = 0;
        }
        break;
    }
}

void MENU::selectEV(int ev)
{
    selectedEV = ev;
    switch (actualScreen)
    {
    case 4:
    case 8:
    case 9:
    case 10:
    case 11:
        action = 1;
        break;
    }
}

void MENU::updateValue(int dir)
{
    action = -1;
    int mem_value;
    int max_value;
    int mem_adress = -1;
    switch (actualScreen)
    {
    case 4:
        switch (actualLine)
        {
        case 1:
            mem_adress = 12 + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
            max_value = 1;
            break;
        case 2:
            mem_adress = 11 + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
            max_value = 14;
            break;
        case 3:
            mem_adress = 10 + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
            max_value = 20;
            break;
        case 4:
            mem_adress = 13 + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
            max_value = 23;
            break;
        }
        break;
    case 5:
        int rtc[7];
        RTC.get(rtc, true);
        switch (actualLine)
        {
        case 1:
            switch (cursorPos)
            {
            case 0:
                int day;
                day = rtc[4];
                if (dir == 1)
                {
                    day = day + 1;
                    if (day >= 31)
                    {
                        day = 0;
                    }
                }
                else
                {
                    day = day + 1;
                    if (day < 1)
                    {
                        day = 31;
                    }
                }

                RTC.stop();
                RTC.set(DS1307_DATE, day);
                RTC.start();
                break;
            case 1: // month
                int month;
                month = rtc[5];
                if (dir == 1)
                {
                    month = month + 1;
                    if (month >= 12)
                    {
                        month = 0;
                    }
                }
                else
                {
                    month = month - 1;
                    if (month < 1)
                    {
                        month = 12;
                    }
                }
                RTC.stop();
                RTC.set(DS1307_MTH, month);
                RTC.start();
                break;
            case 2: // annee
                int year;
                year = rtc[6];
                if (dir == 1)
                {
                    year = year - 2000;
                    year = year + 1;
                }
                else
                {
                    year = year - 2000;
                    year = year - 1;
                    if (year < 0)
                    {
                        year = 0;
                    }
                }
                RTC.stop();
                RTC.set(DS1307_YR, year);
                RTC.start();
                break;
            }
            break;
        case 3:
            switch (cursorPos)
            {
            case 0:
                int hour;
                hour = rtc[2];
                if (dir == 1)
                {
                    hour++;
                    if (hour > 23)
                    {
                        hour = 0;
                    }
                }
                else
                {
                    hour--;
                    if (hour < 0)
                    {
                        hour = 23;
                    }
                }
                RTC.stop();
                RTC.set(DS1307_HR, hour);
                RTC.start();
                break;

            case 1: // minute
                int minute;
                minute = rtc[1];
                if (dir == 0)
                {
                    minute++;
                    if (minute > 59)
                    {
                        minute = 0;
                    }
                }
                else
                {
                    minute--;
                    if (minute < 0)
                    {
                        minute = 59;
                    }
                }
                RTC.stop();
                RTC.set(DS1307_MIN, minute);
                RTC.start();
                break;
            case 2: // seconde
                RTC.stop();
                RTC.set(DS1307_SEC, 0);
                RTC.start();
                break;
            }
        }
        break;
    case 7:
        switch (actualLine)
        {
        case 1:
            mem_adress = 160;
            mem_value = eeprom.Read(mem_adress);
            max_value = 1;
            break;
        case 2:
            mem_adress = 161;
            mem_value = eeprom.Read(mem_adress);
            max_value = 35;
            break;
        case 3:
            mem_adress = 162;
            mem_value = eeprom.Read(mem_adress);
            max_value = 20;
            break;
        case 4:
            mem_adress = 163;
            mem_value = eeprom.Read(mem_adress);
            max_value = 14;
            break;
        case 6:
            mem_adress = 164;
            mem_value = eeprom.Read(mem_adress);
            max_value = 20;
            break;
        case 7:
            mem_adress = 165;
            mem_value = eeprom.Read(mem_adress);
            max_value = 14;
            break;
        }
        break;
    case 9:
        mem_adress = 17 + (10 * selectedEV);
        mem_value = eeprom.Read(mem_adress);
        max_value = 1;
        break;
    }

    if (dir == 1)
    {
        mem_value++;
        if (mem_value > max_value)
        {
            mem_value = 0;
        }
        eeprom.write(mem_adress, mem_value);
    }
    if (dir == 0 and actualScreen != 5)
    {
        mem_value--;
        if (mem_value < 0)
        {
            mem_value = max_value;
        }
        eeprom.write(mem_adress, mem_value);
    }
}

void MENU::moveCursor()
{
    cursorPos++;
    if (cursorPos == 3)
    {
        cursorPos = 0;
    }
}