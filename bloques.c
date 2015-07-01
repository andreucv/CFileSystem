#include "bloques.h"

static int descriptor = 0;
static sem_t *mutex;

int bmount(const char *camino){
	/* Funció que munta un dispositiu: si està creat, l'obre;
	 * si no, el crea de bell nou. */
	descriptor =  open(camino, O_RDWR|O_CREAT, 0666);
	if(descriptor == -1) printf("bloques.c, 10: Error en obrir o crear el dispositiu: %s.\n", camino);
	mutex = initSem();
	return descriptor;
}

int bumount(){
	/* Funció que desmunta un dispositiu ja creat i obert. */
	int d;
	d = close(descriptor);
	if(d == -1) printf("bloques.c, 19: Error en tancar el dispositiu.\n");
	deleteSem();
	return d;
}

int bwrite(unsigned int bloque, const void *buf){
	/* Funció que escriu el contingut d'un buffer de memòria en un
	 * bloc de dades que ha estat indicat. */
	int d;	
	lseek(descriptor, bloque*BLOCKSIZE, SEEK_SET); 
	d = write(descriptor, buf, BLOCKSIZE);	
	if(d == -1) printf("bloques.c, 30: Error a l'escriure en el bloc: %i.\n", bloque);
	return d; 
}

int bread(unsigned int bloque, void *buf){
	/* Funció que llegeix el contingut d'un bloc de dades del
	 * dispositiu i el retorna pel buffer passat per paràmetre. */
	int d;
	lseek(descriptor, bloque*BLOCKSIZE, SEEK_SET);
	d = read(descriptor, buf, BLOCKSIZE);
	if(d == -1) printf("bloques.c, 40: Error en llegir el bloc: %i.\n", bloque);
	return d;
}

/* Funcions pròpies per al control dels Semàfors. */ 
void mi_waitSem(){
	waitSem(mutex);
}

void mi_signalSem(){
	signalSem(mutex);
}
	
