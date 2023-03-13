#ifndef MYEEPROM_h
#define MYEEPROM_h

class MYEEPROM
{
private:
public:
    MYEEPROM();
    void write(int memAdress, int value);
    int Read(int memAdress);
};
#endif