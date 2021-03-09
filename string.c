#include "string.h"

void *memset(void *dst, int v, uint n)
{
    char *castdst = (char *)dst;
    for (int i = 0; i < n; i++)
    {
        castdst[i] = v;
    }
    return dst;
}
