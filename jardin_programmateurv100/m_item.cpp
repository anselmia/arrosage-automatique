#include "m_item.h"

M_ITEM::M_ITEM(int actual, int prev, int next, int l, int mem, int max = -1)
{
    actualScreen = actual;
    previousScreen = prev;
    nextScreen = next;
    line = l;
    memValue = mem;
    maxValue = max;
}
