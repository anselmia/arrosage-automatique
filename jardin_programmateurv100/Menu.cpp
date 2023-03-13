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
    action = -1;
    items[0] = M_ITEM(0, -1, 1, 0, -1);
    items[1] = M_ITEM(0, -1, 2, 1, -1);
    items[2] = M_ITEM(0, -1, 3, 2, -1);
    items[3] = M_ITEM(0, -1, 4, 3, -1);
    items[4] = M_ITEM(0, -1, 6, 4, -1);
    items[5] = M_ITEM(1, 0, 7, 1, -1);
    items[6] = M_ITEM(1, 0, 8, 2, -1);
    items[7] = M_ITEM(1, 0, 9, 3, -1);
    items[8] = M_ITEM(1, 0, 10, 4, -1);
    items[9] = M_ITEM(2, 1, 3, 1, -1);
    items[10] = M_ITEM(2, 1, 3, 2, -1);
    items[11] = M_ITEM(2, 1, 3, 3, -1);
    items[12] = M_ITEM(2, 1, 3, 4, -1);
    items[13] = M_ITEM(2, 1, 3, 5, -1);
    items[14] = M_ITEM(2, 1, 3, 6, -1);
    items[15] = M_ITEM(2, 1, 3, 7, -1);
    items[16] = M_ITEM(2, 1, 3, 8, -1);
    items[17] = M_ITEM(3, 2, -1, 1, 12, 1);
    items[18] = M_ITEM(3, 2, -1, 2, 10, 20);
    items[19] = M_ITEM(3, 2, -1, 3, 11, 14);
    items[20] = M_ITEM(3, 2, -1, 4, 13, 23);
    items[21] = M_ITEM(4, 1, -1, 1, 170, 20);
    items[22] = M_ITEM(4, 1, -1, 2, 171, 20);
    items[23] = M_ITEM(4, 1, -1, 3, 172, 20);
    items[24] = M_ITEM(4, 1, -1, 4, 173, 20);
    items[25] = M_ITEM(5, 1, -1, 1, 0);
    items[26] = M_ITEM(5, 1, -1, 3, 0);
    items[27] = M_ITEM(6, 1, -1, 1, 160, 1);
    items[28] = M_ITEM(6, 1, -1, 2, 161, 35);
    items[29] = M_ITEM(6, 1, -1, 3, 162, 20);
    items[30] = M_ITEM(6, 1, -1, 4, 163, 14);
    items[31] = M_ITEM(6, 1, -1, 1, 164, 20);
    items[32] = M_ITEM(6, 1, -1, 2, 162, 14);
    items[33] = M_ITEM(7, 0, -1, 1, -1);
    items[34] = M_ITEM(7, 0, -1, 2, 0, 20);
    items[35] = M_ITEM(8, 0, -1, 1, 0);
    items[36] = M_ITEM(9, 0, -1, 1, 17);
    items[37] = M_ITEM(9, 0, -1, 2, 0, 14);
    items[38] = M_ITEM(10, 0, -1, 1, 0);
    items[39] = M_ITEM(11, -1, -2, 0, -1);
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
    if (actualScreen == 5)
    {
        moveCursor();
    }
    else
    {
        cursorPos = 0;
    }
    action = -1;
    int newScreen = -1;
    int newLine = -1;
    for (int i = 0; i <= sizeof(items); i++)
    {
        if (items[i].actualScreen == actualScreen)
        {
            if (items[i].line == actualLine)
            {
                newScreen = items[i].nextScreen;
                for (int j = 0; j <= sizeof(items); j++)
                {
                    if (items[j].actualScreen == newScreen)
                    {
                        newLine = items[j].line;
                        break;
                    }
                }
                break;
            }
        }
    }
    if (newScreen != -1)
    {
        actualScreen = newScreen;
        actualLine = newLine;
        action = 1;
    }
}

void MENU::backward()
{
    action = -1;
    int newScreen = -1;
    int newLine = -1;
    for (int i = 0; i <= sizeof(items); i++)
    {
        if (items[i].actualScreen == actualScreen)
        {
            if (items[i].line == actualLine)
            {
                newScreen = items[i].previousScreen;
                for (int j = 0; j <= sizeof(items); j++)
                {
                    if (items[j].actualScreen == newScreen)
                    {
                        newLine = items[j].line;
                        break;
                    }
                }
                break;
            }
        }
    }
    if (newScreen >= -1)
    {
        actualScreen = newScreen;
        actualLine = newLine;
        action = 1;
    }
}

void MENU::up()
{
    action = -1;
    int lines[8];
    int minLine = 9;
    int maxLine = -1;
    int newScreen = -1;
    int newLine = actualLine - 1;
    int lineCount = 0;
    cursorPos = 0;
    for (int i = 0; i <= sizeof(items); i++)
    {
        if (items[i].actualScreen == actualScreen)
        {
            lines[lineCount] = items[i].line;
            if (items[i].line < minLine)
            {
                minLine = items[i].line;
            }
            if (items[i].line > maxLine)
            {
                maxLine = items[i].line;
            }
            lineCount++;
        }
    }

    if (actualLine > minLine)
    {
        for (int i = sizeof(lines) - 1; i >= 0; i--)
        {
            if (lines[i] < actualLine)
            {
                actualLine = lines[i];
            }
        }
        action = 0;
        if (actualLine == 4)
        {
            action = 1;
        }
    }
    else
    {
        actualLine = maxLine;
        action = 1;
    }

    return action;
}

void MENU::down()
{
    action = -1;
    int lines[8];
    int minLine = 9;
    int maxLine = -1;
    int newScreen = -1;
    int newLine = actualLine - 1;
    int lineCount = 0;
    cursorPos = 0;
    for (int i = 0; i <= sizeof(items); i++)
    {
        if (items[i].actualScreen == actualScreen)
        {
            lines[lineCount] = items[i].line;
            if (items[i].line < minLine)
            {
                minLine = items[i].line;
            }
            if (items[i].line > maxLine)
            {
                maxLine = items[i].line;
            }
            lineCount++;
        }
    }

    if (actualLine < maxLine)
    {
        for (int i = 0; i < sizeof(lines); i++)
        {
            if (lines[i] < actualLine)
            {
                actualLine = lines[i];
            }
        }
        action = 0;
        if (actualLine == 5)
        {
            action = 1;
        }
    }
    else
    {
        actualLine = minLine;
        action = 1;
    }
}

void MENU::updateValue(int dir)
{
    int mem_adress;
    M_ITEM item = getItem();
    if (item.memValue > 0)
    {
        int mem_value;
        if (selectedEV == 0)
        {
            mem_adress = item.memValue;
            mem_value = eeprom.Read(mem_adress);
        }
        else
        {
            mem_adress = (item.memValue) + (10 * selectedEV);
            mem_value = eeprom.Read(mem_adress);
        }

        if (dir == 1)
        {
            mem_value++;
            if (mem_value > item.maxValue)
            {
                mem_value = 0;
            }
            eeprom.write(mem_adress, mem_value);
        }
        if (dir == 0)
        {
            mem_value = eeprom.Read(mem_adress);
            if (mem_value <= 0)
            {
                mem_value = item.maxValue;
            }
            else
            {
                mem_value--;
            }

            eeprom.write(mem_adress, mem_value);
        }
    }

    if (actualScreen == 5)
    {
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
    }
}

M_ITEM MENU::getItem()
{
    M_ITEM item;
    for (int i = 0; i <= sizeof(items); i++)
    {
        if (items[i].actualScreen == actualScreen)
        {
            if (items[i].line == actualLine)
            {
                item = items[i];
                break;
            }
        }
    }
    return item;
}

void MENU::moveCursor()
{
    cursorPos++;
    if (cursorPos == 3)
    {
        cursorPos = 0;
    }
}