#include "ev.h"

EV::EV(byte pin, byte numEV)
{
    num = numEV;
    evPin = pin;
    evState = 0;
    remainingTimeOn = 0;
    nextDayOn = 0;
    pinMode(evPin, OUTPUT);
}

void EV::init()
{
    OFF();
    remainingTimeOn = 0;
    nextDayOn = 0;
}

void EV::ON()
{
    evState = 1;
    digitalWrite(evPin, LOW);
}

void EV::OFF()
{
    evState = 0;
    digitalWrite(evPin, HIGH);
}

void EV::update_state()
{
    if (remainingTimeOn == 0)
        OFF();
    else
        ON();
}

void EV::calculate_next_day(MYEEPROM eeprom, byte rtc_day, byte rtc_month, int rtc_year)
{
    byte day_to_add;
    byte day_in_month;
    byte next_day;

    day_to_add = eeprom.Read(mem_autoFreq + (10 * num));
    next_day = rtc_day + day_to_add;

    switch (rtc_month)
    {
    case 1: // January / March / May / July / August / October / December 
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        day_in_month = 31;
        break;
    case 2: // February
        day_in_month = leap_year(rtc_year);
        break;
    case 4: // April / June / September /November
    case 6:
    case 9:
    case 11:
        day_in_month = 30;
        break;
    }
    if (next_day > day_in_month)
    {
        next_day = next_day - day_in_month;
    }
    nextDayOn = next_day;
}

int EV::leap_year(int rtc_year)
{
    byte days_in_month;
    // leap year if perfectly divisible by 400
    if (rtc_year % 400 == 0)
    {
        days_in_month = 29;
    }
    // not a leap year if divisible by 100
    // but not divisible by 400
    else if (rtc_year % 100 == 0)
    {
        days_in_month = 28;
    }
    // leap year if not divisible by 100
    // but divisible by 4
    else if (rtc_year % 4 == 0)
    {
        days_in_month = 29;
    }
    // all other years are not leap years
    else
    {
        days_in_month = 28;
    }

    return days_in_month;
}

void EV::updateRemainingTime()
{
    if (remainingTimeOn != 0)
    {
        remainingTimeOn--;

        if (remainingTimeOn < 0)
            remainingTimeOn = 0;
    }
}

void EV::updateTimeOn(MYEEPROM eeprom, byte rtc_hour, byte rtc_min, byte rtc_day, byte rtc_month, int rtc_year)
{
    int time_on = 0;
    //  if mode auto on
    if (eeprom.Read(mem_autostate + (10 * num)) == 1)
    {
        // if ev state is activated
        if (eeprom.Read(mem_state + (10 * num)) == 1)
        {
            //  if auto start time hour
            if (rtc_hour == eeprom.Read(mem_autoStartHour + (10 * num)))
            {
                //  if  auto start time min
                if (rtc_min == eeprom.Read(mem_autoStartMin + (10 * num)))
                {
                    if (remainingTimeOn == 0)
                    {
                        // if not started once set next day as today
                        if (nextDayOn == 0)
                            nextDayOn = rtc_day;
                        // if next start day
                        else if (nextDayOn == rtc_day)
                        {
                            // mise en route
                            time_on = eeprom.Read(mem_autoTimeOn + (10 * num)) * 60;
                            if (time_on <= 0)
                                time_on = 0;
                            if (time_on >= max_time_on_ev)
                                time_on = max_time_on_ev;

                            remainingTimeOn = time_on;
                            calculate_next_day(eeprom, rtc_day, rtc_month, rtc_year);
                        }
                    }
                }
            }
        }
        else
        {
            nextDayOn = 0;
        }
    }
    else
    {
        nextDayOn = 0;
    }
}