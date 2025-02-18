/**
 * Simple tool to spit out a string of screen codes for ASCII text
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void a2s(char *s)
{
    printf(%lu\n,strlen(s));
    
    while (*s)
    {
        *s &= 0x7F;

        if (*s<32)
            *s+=64;
        else if (*s<96)
            *s-=32;

        printf("0x%02x, ",*s);

        s++;
    }

    printf("\n");
}

int main(int argc, char *argv[])
{

    if (argv[1])
        a2s(argv[1]);
    
    return 0;
}
