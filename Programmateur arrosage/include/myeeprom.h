#ifndef MYEEPROM_h
#define MYEEPROM_h

const int mem_state = 17;
const int mem_autostate = 12;
const int mem_autoFreq = 11;
const int mem_autoTimeOn = 10;
const int mem_autoStartHour = 13;
const int mem_dayTemp = 150;
const int mem_dayHumidity = 151;
const int mem_autoSeason = 160;
const int mem_tempSeason = 161;
const int mem_sumerTimeon = 162;
const int mem_sumerFreq = 163;
const int mem_winterTimeon = 164;
const int mem_winterFreq = 165;
/* memory address
0 : 20:90 auto time on for ev
1 : 21:91 auto frequency for ev
2 : 22:92 auto mode state for ev
3 : 23:93 auto start time for ev
4 : 27:97 active state for ev
5 : 150 day temp at 12
    151 day humidity at 12
6 : 160 auto mode with ath21 sensor
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
    void write(int memAdress, int value);
    int Read(int memAdress);
};
#endif