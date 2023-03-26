#include "menu.h"

MENU::MENU()
{
    eeprom = MYEEPROM();
    // clock = CLOCK();
    rtc_min = -1;
    rtc_day = 0;
    screenValue = 0;
    actualScreen = 0;
    actualLine = 0;
    cursorPos = 0;
    delay = false;
    stop = false;
    inactive = 0;
    manual = false;
    stop_all = false;
    manual_all = 0;
}

void MENU::initClock(int (&error)[2])
{
    rtc.begin();
    rtc.start();
    sec = 0;
    min = 0;
    hour = 0;
    day = 0;
    month = 0;
    year = 0;
    getClock(error);
}

void MENU::getClock(int (&error)[2])
{
    rtc.get(&sec, &min, &hour, &day, &month, &year);
    // clock.updateTime(&sec, &min, &hour, &day, &month, &year); // to remove when real clock

    if (day == 165)
        error[1] = 1;
    else
        error[1] = 0;
}

void MENU::forward()
{
    switch (actualScreen)
    {
    case 0:
        actualScreen = 1;
        actualLine = 0;
        break;
    case 1:
        switch (actualLine)
        {
        case 0:
            actualScreen = 2;
            actualLine = 1;
            break;
        case 1:
            actualScreen = 8;
            actualLine = 2;
            selectedEV = 1;
            screenValue = 0;
            break;
        case 2:
            actualScreen = 9;
            selectedEV = 1;
            actualLine = 2;
            screenValue = 0;
            break;
        case 3:
            actualScreen = 10;
            selectedEV = 1;
            actualLine = 2;
            screenValue = 0;
            break;
        case 4:
            actualScreen = 11;
            selectedEV = 1;
            actualLine = 2;
            screenValue = 0;
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
            break;
        case 2:
            cursorPos = 0;
            actualScreen = 6;
            actualLine = 1;
            break;
        case 3:
            actualScreen = 7;
            actualLine = 1;
            break;
        }
        break;
    case 6:
        moveCursor();
        break;
    case 8:
        manual = true;
        actualScreen = 2;
        actualLine = 2;
        break;
    case 10:
        delay = true;
        actualScreen = 2;
        actualLine = 2;
        break;
    case 11:
        stop = true;
        actualScreen = 2;
        actualLine = 2;
        break;
    }
}

void MENU::backward()
{
    switch (actualScreen)
    {
    case 1:
        actualScreen = 0;
        actualLine = 0;
        break;
    case 2:
    case 8:
    case 9:
    case 10:
    case 11:
        actualScreen = 1;
        actualLine = 0;
        screenValue = 0;
        break;

    case 4:
    case 6:
    case 7:
        actualScreen = 2;
        actualLine = 1;
        break;
    }
}

void MENU::up()
{
    switch (actualScreen)
    {
    case 1:
        if (actualLine == 0)
            actualLine = 4;
        else
            actualLine--;
        break;
    case 2:
        if (actualLine == 1)
            actualLine = 3;
        else
            actualLine--;
        break;
    case 4:
        if (actualLine == 1)
            actualLine = 4;
        else
            actualLine--;
        break;
    case 6:
        if (actualLine == 1)
            actualLine = 3;
        else
            actualLine = 1;
        cursorPos = 0;
        break;
    case 7:
        if (actualLine == 1)
            actualLine = 7;
        else if (actualLine == 6)
            actualLine = 4;
        else
            actualLine--;
        break;
    }
}

void MENU::down()
{
    switch (actualScreen)
    {
    case 1:
        if (actualLine == 4)
            actualLine = 0;
        else
            actualLine++;
        break;
    case 2:
        if (actualLine == 3)
            actualLine = 1;
        else
            actualLine++;
        break;
    case 4:
        if (actualLine == 4)
            actualLine = 1;
        else
            actualLine++;
        break;
    case 6:
        if (actualLine == 1)
            actualLine = 3;
        else
            actualLine = 1;
        cursorPos = 0;
        break;
    case 7:
        if (actualLine > 6)
            actualLine = 1;
        else if (actualLine == 4)
            actualLine = 6;
        else
            actualLine++;
        break;
    }
}

void MENU::updateValue(int dir, int value)
{
    int mem_value;
    int max_value;
    int mem_adress = -1;
    switch (actualScreen)
    {
    case 0:
        switch (dir)
        {
        case 0:
            stop_all = true;
            break;
        case 1:
            manual_all = 1;
            break;
        }
        break;
    case 4:
        switch (actualLine)
        {
        case 1:
            mem_adress = mem_autostate + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
            max_value = 1;
            break;
        case 2:
            mem_adress = mem_autoTimeOn + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
            max_value = 20;
            break;
        case 3:
            mem_adress = mem_autoFreq + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
            max_value = 14;
            break;
        case 4:
            mem_adress = mem_autoStartHour + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
            max_value = 23;
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
                if (dir == 1)
                {
                    day = day + 1;
                    if (day >= 31)
                        day = 0;
                }
                else
                {
                    day = day + 1;
                    if (day < 1)
                    {
                        day = 31;
                    }
                }

                rtc.stop();
                rtc.set(sec, min, hour, day, month, year);
                rtc.start();
                // clock.days = day; // to remove
                break;
            case 1: // month
                if (dir == 1)
                {
                    month = month + 1;
                    if (month >= 12)
                        month = 0;
                }
                else
                {
                    month = month - 1;
                    if (month < 1)
                        month = 12;
                }
                rtc.stop();
                rtc.set(sec, min, hour, day, month, year);
                rtc.start();
                // clock.months = month; // to remove
                break;
            case 2: // annee
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
                        year = 0;
                }
                rtc.stop();
                rtc.set(sec, min, hour, day, month, year);
                rtc.start();
                // clock.years = year; // to remove
                break;
            }
            break;
        case 3:
            switch (cursorPos)
            {
            case 0:
                if (dir == 1)
                {
                    hour++;
                    if (hour > 23)
                        hour = 0;
                }
                else
                {
                    hour--;
                    if (hour < 0)
                        hour = 23;
                }
                rtc.stop();
                rtc.set(sec, min, hour, day, month, year);
                rtc.start();
                // clock.hrs = hour; // to remove
                break;

            case 1: // minute
                if (dir == 1)
                {
                    min++;
                    if (min > 59)
                        min = 0;
                }
                else
                {
                    min--;
                    if (min < 0)
                        min = 59;
                }
                rtc.stop();
                rtc.set(sec, min, hour, day, month, year);
                rtc.start();
                // clock.mins = min; // to remove
                break;
            case 2: // seconde
                sec = 0;
                rtc.stop();
                rtc.set(sec, min, hour, day, month, year);
                rtc.start();
                // clock.secs = 0; // to remove
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
            break;
        case 2:
            mem_adress = mem_tempSeason;
            mem_value = eeprom.Read(mem_adress);
            max_value = 35;
            break;
        case 3:
            mem_adress = mem_sumerTimeon;
            mem_value = eeprom.Read(mem_adress);
            max_value = 20;
            break;
        case 4:
            mem_adress = mem_sumerFreq;
            mem_value = eeprom.Read(mem_adress);
            max_value = 14;
            break;
        case 6:
            mem_adress = mem_winterTimeon;
            mem_value = eeprom.Read(mem_adress);
            max_value = 20;
            break;
        case 7:
            mem_adress = mem_winterFreq;
            mem_value = eeprom.Read(mem_adress);
            max_value = 14;
            break;
        }
        break;
    case 8:
        screenValue += 5;
        if (screenValue > 20)
            screenValue = 0;
        break;
    case 9:
        mem_adress = mem_state + (10 * selectedEV);
        mem_value = eeprom.Read(mem_adress);
        max_value = 1;
        break;
    case 10:
        screenValue += 1;
        if (screenValue > 14)
            screenValue = 0;
        break;
    case 11:
        if (screenValue == 0)
            screenValue = 1;
        else
            screenValue = 0;
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

void MENU::selectEV(int ev)
{
    switch (actualScreen)
    {
    case 0:
        selectedEV = ev;
        manual = true;
        screenValue = 5;
        break;
    case 4:
    case 8:
    case 9:
    case 10:
    case 11:
        selectedEV = ev;
        break;
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