#include <stdio.h>
#include <signal.h>

void restart(int );

void main()
{
     signal(SIGUSR1, restart);
    // kill( , SIGUSR1);
}

void restart(int arg)
{
	printf("printer has been reset and is working \n");
}

