#include <atari.h>
#include <stdio.h>

void main(void)
{
	while (1)
		printf("%u\n",OS.rtclok[2]&0x0F);
}
