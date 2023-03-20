#ifndef M_ITEM_h
#define M_ITEM_h

class M_ITEM
{
private:
public:
    int actualScreen;
    int previousScreen;
    int nextScreen;
    int line;
    int memValue;
    int maxValue;
    M_ITEM() = default;
    M_ITEM(int actual, int prev, int next, int line, int memValue, int max = -1);
};
#endif