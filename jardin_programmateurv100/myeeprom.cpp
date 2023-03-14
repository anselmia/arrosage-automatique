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