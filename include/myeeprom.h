#ifndef MYEEPROM_h
#define MYEEPROM_h

#include <Arduino.h>

const byte mem_state = 17;
const byte mem_autostate = 12;
const byte mem_autoFreq = 11;
const byte mem_autoTimeOn = 10;
const byte mem_autoStartHour = 13;
const byte mem_autoStartMin = 14;
const byte mem_sec = 100;
const byte mem_day = 101;
const byte mem_dayTemp = 150;
const byte mem_dayHumidity = 151;
const byte mem_autoSeason = 160;
const byte mem_tempSeason = 161;
const byte mem_sumerTimeon = 162;
const byte mem_sumerFreq = 163;
const byte mem_winterTimeon = 164;
const byte mem_winterFreq = 165;
/* memory address
0 : 20:90 auto time on for ev
1 : 21:91 auto frequency for ev
2 : 22:92 auto mode state for ev
3 : 23:93 auto start time for ev
4 : 27:97 active state for ev
5 : 100:101 sec day
6 : 150 day temp at 12
    151 day humidity at 12
7 : 160 auto mode with ath21 sensor
    161 winter/ete temp
    162 duration summer
    163 frequency summer
    164 duration winter
    165 frequency winter*/

class MYEEPROM
{
private:
public:
    MYEEPROM();
    void init_memory();
    void write(byte memAdress, byte value);
    byte Read(byte memAdress);
};
#endif