#include <sys/wait.h>
#include "simulacion.h"

unsigned int finished = 0;
unsigned char dirSim[60];

unsigned int posMax = 500000;
/*(((12+256+256*256+256*256*256)-1)*BLOCKSIZE)/sizeof(struct Registro);*/

void enterrador(){
	while(wait3(NULL, WNOHANG, NULL) > 0){
		finished++;
		printf("Processos Acabats: %i de %i.\n", finished, MAX_PROCESOS);
	}
}

void CrearSimulacion(){
	memset(dirSim, 0, 60);
		
	struct tm *tm;
	time_t tiempo = time(NULL);
	tm = localtime(&tiempo);
	sprintf(dirSim, "/simul_%d%02d%02d%02d%02d%02d/", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	if(mi_creat(dirSim, 7) == -1){
		printf("simulacion.c, 25: Error en crear un Directori.\n");
	}
}

int CrearHijo(){
	struct Registro r;
	int i;
	char pathHijo[60];
	char p[60];
	memset(pathHijo, 0, 60);
	memset(p, 0, 60);
	sprintf(pathHijo, "%sproceso_%d/", dirSim, getpid());
	sprintf(p, "%sprueba.dat", pathHijo); 
	mi_creat(pathHijo, 7);
	mi_creat(p, 7);

	int max = 0;
	srand(time(NULL) + getpid());
	r.numEsc = 0;
	for(i = 0; i < MAX_PROCESOS/2; i++){
		r.time = time(NULL);
		r.pid = getpid();
		r.numEsc = i + 1;
		r.posFich = (rand() % posMax);
		if(r.posFich > max) max = r.posFich;
		mi_write(p, &r, r.posFich*sizeof(struct Registro), sizeof(struct Registro));
		usleep(50000);
	}
}

int main(int argc, char **argv){
	int i;
	if(argc != 2){
		printf("simulacion.c, 58: Error en nombre de paràmetres.\n");
		printf("L'ús del programa és:\n\t");
		printf("./simulacion [nom_del_dispositiu] \n");
		return -1;
	}

	if(bmount(argv[1]) == -1){
		printf("simulacion.c, 65: Error en Muntar el Fitxer.\n");
		return -1;
	}

	CrearSimulacion();
	signal(SIGCHLD, enterrador);

	for(i = 0; i < MAX_PROCESOS; i++){
		if(fork() == 0){
			CrearHijo();
			exit(0);
		}	
		usleep(200000);
	}
	while(finished < MAX_PROCESOS) pause();

	printf("Final de la Simulació.\n");
	bumount();
}
			
