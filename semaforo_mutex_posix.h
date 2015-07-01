#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SEM_NAME "/mymutex" 
#define SEM_INIT_VALUE 1 

sem_t *initSem();
void deleteSem();
void signalSem(sem_t *sem);
void waitSem(sem_t *sem);
