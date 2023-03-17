#include "ev.h"
#include "Arduino.h"

EV::EV(int pin, int numEV)
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
    digitalWrite(evPin, HIGH);
    Serial.print(F("ev "));
    Serial.print(evPin);
    Serial.print(" ON");
}

void EV::OFF()
{
    evState = 0;
    digitalWrite(evPin, LOW);
}

void EV::update_state()
{
    if (remainingTimeOn == 0)
    {
        OFF();
    }
    else
    {
        ON();
    }
}

void EV::calculate_next_day(int day, int month, int year)
{
    int day_to_add;
    int day_in_month;
    int next_day;

    day_to_add = eeprom.Read(mem_autoFreq + (10 * num));
    next_day = day + day_to_add;

    switch (month)
    {
    case 1: // 31
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        day_in_month = 31;
        break;
    case 2: // 28
        day_in_month = leap_year(year);
        break;
    case 4:
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

int EV::leap_year(int year)
{
    int days_in_month;
    // leap year if perfectly divisible by 400
    if (year % 400 == 0)
    {
        days_in_month = 29;
    }
    // not a leap year if divisible by 100
    // but not divisible by 400
    else if (year % 100 == 0)
    {
        days_in_month = 28;
    }
    // leap year if not divisible by 100
    // but divisible by 4
    else if (year % 4 == 0)
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

void EV::updateRemainingTime(int hr, int day, int month, int year)
{
    if (eeprom.Read(mem_state + (10 * num)) == 0)
    {
        remainingTimeOn = 0;
        nextDayOn = 0;
    }

    if (remainingTimeOn != 0)
    {
        remainingTimeOn--;

        if (remainingTimeOn < 0)
        {
            remainingTimeOn = 0;
        }
    }

    int time_on = 0;
    //  if mode auto on
    if (eeprom.Read(mem_autostate + (10 * num)) == 1)
    {
        // if ev state is on
        if (eeprom.Read(mem_state + (10 * num)) == 1)
        {
            //  if clock h == auto start time
            if (hr == eeprom.Read(mem_autoStartHour + (10 * num)))
            {
                if (remainingTimeOn == 0)
                {
                    // if not started once set next day as today
                    if (nextDayOn == 0)
                    {
                        nextDayOn = day;
                    }
                    // if clock day == next start day
                    if (nextDayOn == day)
                    {
                        // mise en route
                        time_on = eeprom.Read(mem_autoTimeOn + (10 * num));
                        if (time_on <= 0)
                        {
                            time_on = 0;
                        }
                        if (time_on >= max_time_on_ev)
                        {
                            time_on = max_time_on_ev;
                        }

                        remainingTimeOn = time_on;
                        calculate_next_day(day, month, year);
                    }
                }
            }
        }
        else
        {
            remainingTimeOn = 0;
            nextDayOn = 0;
        }
    }
}

void EV::updateSeason(int timeon, int freq)
{
    eeprom.write(mem_autoTimeOn + (10 * num), timeon);
    eeprom.write(mem_autoFreq + (10 * num), freq);
}