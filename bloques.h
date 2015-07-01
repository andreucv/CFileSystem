#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h> 
#include <string.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include "semaforo_mutex_posix.h"

#define BLOCKSIZE 1024 

int bmount(const char *camino);
int bumount();
int bwrite(unsigned int bloque, const void *buf);
int bread(unsigned int bloque, void *buf);
void mi_waitSem();
void mi_signalSem();
