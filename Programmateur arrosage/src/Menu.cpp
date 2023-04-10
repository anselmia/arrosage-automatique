#include "menu.h"

MENU::MENU()
{
    screenValue = 0;
    actualScreen = 0;
    actualLine = 0;
    cursorPos = 0;
    delay = false;
    stop = false;
    manual = false;
    stop_all = false;
    manual_all = 0;
}

void MENU::initClock(bool (&error)[2])
{
    rtc.begin();
    rtc.start();

    getClock(error);
}

void MENU::getClock(bool (&error)[2])
{
    rtc.get(&rtc_sec, &rtc_min, &rtc_hour, &rtc_day, &rtc_month, &rtc_year);

    if (rtc_day == 165)
        error[1] = true;
    else
        error[1] = false;
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
            cursorPos = 0;
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
        case 4:
            actualScreen = 5;
            break;
        }
        break;
    case 4:
        if (actualLine == 4)
            moveCursor(2);
        break;
    case 6:
        moveCursor(3);
        break;
    case 8:
        manual = true;
        actualScreen = 0;
        actualLine = 0;
        break;
    case 10:
        delay = true;
        actualScreen = 0;
        actualLine = 0;
        break;
    case 11:
        stop = true;
        actualScreen = 0;
        actualLine = 0;
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
    case 5:
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

void MENU::updateValue(MYEEPROM eeprom, byte dir, byte value)
{
    byte mem_value;
    byte max_value;
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
            switch (cursorPos)
            {
            case 0:
                mem_adress = mem_autoStartHour + (10 * selectedEV);
                mem_value = eeprom.Read(mem_adress);
                max_value = 23;
                break;
            case 1:
                mem_adress = mem_autoStartMin + (10 * selectedEV);
                mem_value = eeprom.Read(mem_adress);
                max_value = 59;
                break;
            }
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
                    rtc_day = rtc_day + 1;
                    if (rtc_day >= 31)
                        rtc_day = 0;
                }
                else
                {
                    rtc_day = rtc_day - 1;
                    if (rtc_day < 1)
                    {
                        rtc_day = 31;
                    }
                }

                rtc.stop();
                rtc.set(rtc_sec, rtc_min, rtc_hour, rtc_day, rtc_month, rtc_year);
                rtc.start();
                break;
            case 1: // month
                if (dir == 1)
                {
                    rtc_month = rtc_month + 1;
                    if (rtc_month >= 12)
                        rtc_month = 0;
                }
                else
                {
                    rtc_month = rtc_month - 1;
                    if (rtc_month < 1)
                        rtc_month = 12;
                }
                rtc.stop();
                rtc.set(rtc_sec, rtc_min, rtc_hour, rtc_day, rtc_month, rtc_year);
                rtc.start();
                break;
            case 2: // year
                if (dir == 1)
                {
                    rtc_year = rtc_year - 2000;
                    rtc_year = rtc_year + 1;
                }
                else
                {
                    rtc_year = rtc_year - 2000;
                    rtc_year = rtc_year - 1;
                    if (rtc_year < 0)
                        rtc_year = 0;
                }
                rtc.stop();
                rtc.set(rtc_sec, rtc_min, rtc_hour, rtc_day, rtc_month, rtc_year);
                rtc.start();
                break;
            }
            break;
        case 3:
            switch (cursorPos)
            {
            case 0:
                if (dir == 1)
                {
                    rtc_hour++;
                    if (rtc_hour > 23)
                        rtc_hour = 0;
                }
                else
                {
                    rtc_hour--;
                    if (rtc_hour < 0)
                        rtc_hour = 23;
                }
                rtc.stop();
                rtc.set(rtc_sec, rtc_min, rtc_hour, rtc_day, rtc_month, rtc_year);
                rtc.start();
                break;

            case 1: // minute
                if (dir == 1)
                {
                    rtc_min++;
                    if (rtc_min > 59)
                        rtc_min = 0;
                }
                else
                {
                    rtc_min--;
                    if (rtc_min < 0)
                        rtc_min = 59;
                }
                rtc.stop();
                rtc.set(rtc_sec, rtc_min, rtc_hour, rtc_day, rtc_month, rtc_year);
                rtc.start();
                break;
            case 2: // seconde
                rtc_sec = 0;
                rtc.stop();
                rtc.set(rtc_sec, rtc_min, rtc_hour, rtc_day, rtc_month, rtc_year);
                rtc.start();
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

void MENU::selectEV(byte ev)
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

void MENU::moveCursor(byte maxPos)
{
    cursorPos++;
    if (cursorPos == maxPos)
    {
        cursorPos = 0;
    }
}