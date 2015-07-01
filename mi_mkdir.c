#include "directorios.h"

int main(int argc, char **argv){

	if(argc != 4){
		printf("mi_mkdir, 6: Error en els arguments.\n");
		printf("L'ús del programa és:\n \t");
		printf("./mi_mkdir [nom_del_dispositiu] [permisos] [cami]\n");
		exit(0);
	}

	if(bmount(argv[1]) == -1){
		printf("mi_mkdir, 13: Error en obrir el dispositiu: %s.\n", argv[1]);
		return -1;
	}

	/* Hem de comprovar que els permisos són correctes. */
	int per = atoi(argv[2]);
	if(per > 7 || per < 0){
		printf("mi_mkdir, 20: Error en els permisos.\n");
		return -1;
	}

	int e = 0;
	if(e = mi_creat(argv[3], atoi(argv[2])) < 0){
		switch(e){
			case -1: printf("mi_mkdir, 27: Error en Extraer Camino.\n"); break; 
			case -2: printf("mi_mkdir, 28: Error en Llegir Entrada.\n"); break; 
			case -3: printf("mi_mkdir, 29: Error: No existeix l'Entrada.\n"); break; 
			case -4: printf("mi_mkdir, 30: Error en Escriure Entrada.\n"); break; 
			case -5: printf("mi_mkdir, 31: Error: Entrada ja existent.\n"); break;
			case -6: printf("mi_mkdir, 32: Error en trobar Directori Intermig.\n"); break; 
		}
		bumount();
		return e;
	}
	bumount();
	return 0;
}


