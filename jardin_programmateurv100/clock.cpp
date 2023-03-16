#include "clock.h"
#include "Arduino.h"

CLOCK::CLOCK()
{
    int secs = 0;
    int mins = -1; // it is -1 because when program start  the minutes are incremmented by 1 when secundes are zero
    int hrs = 0;
    unsigned long currmillis;
    unsigned long prevMillis;
}

// function of minutes
void CLOCK::updateMin()
{
    if (mins > 59) // when minutes take a value bigger than 59 it returns to 0 and hours are incremented by 1
    {
        mins = 0;
        hrs = hrs + 1; // hours incremented by 1
        updateHrs();   // call the hours function to update the value of hours
    }
}

// function of hours
void CLOCK::updateHrs()
{
    if (hrs > 23) // when hours take a value bigger than 23 it returns to 0
    {
        hrs = 0;
        days = days + 1;
        updateDays();
    }
}

void CLOCK::updateDays()
{
    switch (days)
    {
    case 1: // 31
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        if (days > 31) // when hours take a value bigger than 23 it returns to 0
        {
            days = 1;
            months = months + 1;
            updateMonths();
        }
        break;
    case 2:            // 28
        if (days > 28) // when hours take a value bigger than 23 it returns to 0
        {
            days = 1;
            months = months + 1;
            updateMonths();
        }
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        if (days > 30) // when hours take a value bigger than 23 it returns to 0
        {
            days = 1;
            months = months + 1;
            updateMonths();
        }
        break;
    }
}

void CLOCK::updateMonths()
{
    if (months > 12) // when hours take a value bigger than 23 it returns to 0
    {
        months = 1;
        years = years + 1;
    }
}

void CLOCK::updateTime(int (&rtc)[7])
{
    // obtain seconds from arduino time
    currmillis = (millis() / 1000.0); // currmilis== current milliseconds

    // next every time when secs is equal to 0 minutes are increased by 1 initial value of minutes is -1
    if (currmillis > prevMillis) // use minlock because otherwise when secs==0 mins increase more than +1
    {
        prevMillis = currmillis;
        secs++;
    }
    if (secs > 60)
    {
        secs = 0;
        mins = mins + 1; // when program start and secs==0 minutes become 0
        updateMin();     // call the minutes function to update the value of minutes
    }

    rtc[0] = secs;
    rtc[1] = mins;
    rtc[2] = hrs;
    rtc[3] = 0;
    rtc[4] = days;
    rtc[5] = months;
    rtc[6] = years;
}