#include "menu.h"
#include "myeeprom.h"
#include <DS1307.h>
#include "clock.h"

MENU::MENU()
{
    MYEEPROM eeprom = MYEEPROM();
    CLOCK clock = CLOCK();

    rtc_min = -1;
    rtc_day = 0;
    screenValue = 0;
    actualScreen = 0;
    actualLine = 0;
    cursorPos = 0;
    selectedEV = 1;
    redraw = 1;
    delay = 0;
    stop = 0;
    inactive = 0;
    manual = 0;
}

void MENU::initClock(int (&module_state)[2])
{
    getClock(module_state);
    RTC.SetOutput(DS1307_SQW32KHZ);
}

void MENU::getClock(int (&module_state)[2])
{
    RTC.get(rtc, true);
    clock.updateTime(rtc); // to remove when real clock

    if (rtc[0] == 0 && rtc[1] == 0 && rtc[2] == 0 && rtc[3] == 0 && rtc[4] == 0 && rtc[5] == 0 && rtc[6] == 0)
        module_state[0] = 1;
    else
        module_state[0] = 0;
}

void MENU::forward()
{
    redraw = -1;
    screenValue = 0;
    switch (actualScreen)
    {
    case 0:
        actualScreen = 1;
        actualLine = 0;
        redraw = 1;
        break;
    case 1:
        switch (actualLine)
        {
        case 0:
            actualScreen = 2;
            actualLine = 1;
            redraw = 1;
            break;
        case 1:
            actualScreen = 8;
            actualLine = 2;
            selectedEV = 1;
            redraw = 1;
            break;
        case 2:
            actualScreen = 9;
            selectedEV = 1;
            actualLine = 2;
            redraw = 1;
            break;
        case 3:
            actualScreen = 10;
            selectedEV = 1;
            actualLine = 2;
            redraw = 1;
            break;
        case 4:
            actualScreen = 11;
            selectedEV = 1;
            actualLine = 2;
            redraw = 1;
            break;
        }
        break;
    case 2:
        switch (actualLine)
        {
        case 1:
            actualScreen = 4;
            selectedEV = 1;
            actualLine = 1;
            redraw = 1;
            break;
        case 2:
            cursorPos = 0;
            actualScreen = 6;
            actualLine = 1;
            redraw = 1;
            break;
        case 3:
            actualScreen = 7;
            actualLine = 1;
            redraw = 1;
            break;
        }
        break;
    case 6:
        moveCursor();
        redraw = 0;
        break;
    case 8:
        manual = 1;
        break;
    case 10:
        delay = 1;
        break;
    case 11:
        stop = 1;
        break;
    }
}

void MENU::backward()
{
    redraw = -1;
    switch (actualScreen)
    {
    case 1:
        actualScreen = 0;
        actualLine = 0;
        redraw = 1;
        break;
    case 2:
    case 8:
    case 9:
    case 10:
    case 11:
        selectedEV = 0;
        actualScreen = 1;
        actualLine = 0;
        redraw = 1;
        screenValue = 0;
        break;

    case 4:
    case 6:
    case 7:
        selectedEV = 0;
        actualScreen = 2;
        actualLine = 1;
        redraw = 1;
        break;
    }
}

void MENU::up()
{
    redraw = -1;
    switch (actualScreen)
    {
    case 1:
        redraw = 0;
        if (actualLine == 0)
            actualLine = 4;
        else
            actualLine--;
        break;
    case 2:
        redraw = 0;
        if (actualLine == 1)
            actualLine = 3;
        else
            actualLine--;
        break;
    case 4:
        redraw = 0;
        if (actualLine == 1)
            actualLine = 4;
        else
            actualLine--;
        break;
    case 6:
        redraw = 0;
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
            redraw = 1;
        }
        else if (actualLine == 6)
        {
            actualLine = 4;
            redraw = 1;
        }
        else
        {
            actualLine--;
            redraw = 0;
        }
        break;
    }
}

void MENU::down()
{
    redraw = -1;
    switch (actualScreen)
    {
    case 1:
        redraw = 0;
        if (actualLine == 4)
            actualLine = 0;
        else
            actualLine++;
        break;
    case 2:
        redraw = 0;
        if (actualLine == 3)
            actualLine = 1;
        else
            actualLine++;
        break;
    case 4:
        redraw = 0;
        if (actualLine == 4)
            actualLine = 1;
        else
            actualLine++;
        break;
    case 6:
        redraw = 0;
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
            redraw = 1;
        }
        else if (actualLine == 4)
        {
            actualLine = 6;
            redraw = 1;
        }
        else
        {
            actualLine++;
            redraw = 0;
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
    case 10:
    case 11:
        if (eeprom.Read(mem_state + (10 * ev)) == 1)
        {
            actualLine = 1;
            redraw = 1;
        }

        break;
    case 9:
        redraw = 1;
        break;
    }
}

void MENU::updateValue(int dir, int value = 0)
{
    redraw = -1;
    int mem_value;
    int max_value;
    int mem_adress = -1;
    switch (actualScreen)
    {
    case 4:
        switch (actualLine)
        {
        case 1:
            mem_adress = mem_autostate + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
            max_value = 1;
            redraw = 1;
            break;
        case 2:
            mem_adress = mem_autoTimeOn + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
            max_value = 14;
            redraw = 1;
            break;
        case 3:
            mem_adress = mem_autoFreq + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
            max_value = 20;
            redraw = 1;
            break;
        case 4:
            mem_adress = mem_autoStartHour + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
            max_value = 23;
            redraw = 1;
            break;
        }
        break;
    case 6:
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

                // RTC.stop();
                // RTC.set(DS1307_DATE, day);
                // RTC.start();
                clock.days = day; // to remove
                redraw = 1;
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
                // RTC.stop();
                // RTC.set(DS1307_MTH, month);
                // RTC.start();
                clock.months = month; // to remove
                redraw = 1;
                break;
            case 2: // annee
                int year;
                year = rtc[6];
                if (dir == 1)
                {
                    // year = year - 2000; to uncomment
                    year = year + 1;
                }
                else
                {
                    // year = year - 2000; to uncomment
                    year = year - 1;
                    if (year < 0)
                    {
                        year = 0;
                    }
                }
                // RTC.stop();
                // RTC.set(DS1307_YR, year);
                // RTC.start();
                clock.years = year; // to remove
                redraw = 1;
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
                // RTC.stop();
                // RTC.set(DS1307_HR, hour);
                // RTC.start();
                clock.hrs = hour; // to remove
                redraw = 1;
                break;

            case 1: // minute
                int minute;
                minute = rtc[1];
                if (dir == 1)
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
                // RTC.stop();
                // RTC.set(DS1307_MIN, minute);
                // RTC.start();
                clock.mins = minute; // to remove
                redraw = 1;
                break;
            case 2: // seconde
                // RTC.stop();
                // RTC.set(DS1307_SEC, 0);
                // RTC.start();
                clock.secs = 0; // to remove
                redraw = 1;
                break;
            }
        }
        break;
    case 7:
        switch (actualLine)
        {
        case 1:
            mem_adress = mem_autoSeason;
            mem_value = eeprom.Read(mem_adress);
            max_value = 1;
            redraw = 1;
            break;
        case 2:
            mem_adress = mem_tempSeason;
            mem_value = eeprom.Read(mem_adress);
            max_value = 35;
            redraw = 1;
            break;
        case 3:
            mem_adress = mem_sumerTimeon;
            mem_value = eeprom.Read(mem_adress);
            max_value = 20;
            redraw = 1;
            break;
        case 4:
            mem_adress = mem_sumerFreq;
            mem_value = eeprom.Read(mem_adress);
            max_value = 14;
            redraw = 1;
            break;
        case 6:
            mem_adress = mem_winterTimeon;
            mem_value = eeprom.Read(mem_adress);
            max_value = 20;
            redraw = 1;
            break;
        case 7:
            mem_adress = mem_winterFreq;
            mem_value = eeprom.Read(mem_adress);
            max_value = 14;
            redraw = 1;
            break;
        }
        break;
    case 8:
        screenValue += 5;
        if (screenValue > 20)
            screenValue = 0;
        redraw = 1;
        break;
    case 9:
        mem_adress = mem_state + (10 * selectedEV);
        mem_value = eeprom.Read(mem_adress);
        max_value = 1;
        redraw = 1;
        break;
    case 10:
        screenValue += 1;
        if (screenValue > 14)
            screenValue = 0;
        redraw = 1;
        break;
    case 11:
        if (screenValue == 0)
            screenValue = 1;
        else
            screenValue = 0;
        redraw = 1;
        break;
    }

    if (mem_adress != -1)
    {
        if (dir == 1)
        {
            mem_value++;
            if (mem_value > max_value)
            {
                mem_value = 0;
            }
            eeprom.write(mem_adress, mem_value);
        }
        if (dir == 0)
        {
            mem_value--;
            if (mem_value < 0)
            {
                mem_value = max_value;
            }
            eeprom.write(mem_adress, mem_value);
        }
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