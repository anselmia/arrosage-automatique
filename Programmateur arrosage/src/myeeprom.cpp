#include "myeeprom.h"
#include <EEPROM.h>
#include "Arduino.h"
MYEEPROM::MYEEPROM()
{
}

void MYEEPROM::write(int mem_adress, int val)
{
    int mem_value;
    mem_value = EEPROM.read(mem_adress);
    if (mem_value != val)
    {
        EEPROM.write(mem_adress, val);
    }
}

int MYEEPROM::Read(int mem_adress)
{
    int mem_value;
    mem_value = EEPROM.read(mem_adress);
    return mem_value;
}

void MYEEPROM::init_memory()
{
    for (int i = 0; i < 6; i++)
    {
        write(mem_autoTimeOn + (10 * (i + 1)), 0);
        write(mem_autoFreq + (10 * (i + 1)), 0);
        write(mem_autostate + (10 * (i + 1)), 0);
        write(mem_autoStartHour + (10 * (i + 1)), 0);
        write(mem_state + (10 * (i + 1)), 0);
    }

    write(mem_dayTemp, 0);
    write(mem_dayHumidity, 0);
    write(mem_autoSeason, 0);
    write(mem_tempSeason, 0);
    write(mem_sumerTimeon, 0);
    write(mem_sumerFreq, 0);
    write(mem_winterTimeon, 0);
    write(mem_winterFreq, 0);
}
