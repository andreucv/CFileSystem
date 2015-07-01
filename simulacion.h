#include <stdio.h>
#include <sys/wait.h>
#include "directorios.h"

#define MAX_PROCESOS 100

struct Registro{
	time_t time;
	unsigned int pid;
	unsigned int numEsc;
	unsigned int posFich;
};

