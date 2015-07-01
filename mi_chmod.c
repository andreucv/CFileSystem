#include "directorios.h"

int main(int argc, char **argv){

	if(argc != 4){
		printf("mi_chmod, 6: Error en els arguments.\n");
		printf("L'ús del programa és:\n \t.");
		printf("/mi_chmod [nom_del_dispositiu] [permisos] [cami]\n");
		exit(0);
	}

	if(bmount(argv[1]) == -1){
		printf("mi_chmod, 13: Error en obrir el dispositiu: %s.\n", argv[1]);
		return -1;
	}
	
	/* Hem de comprovar que els permisos són correctes. */
	int per = atoi(argv[2]);
	if(per > 7 || per < 0){
		printf("mi_chmod, 20: Error en els permisos.\n");
		return -1;
	}
	
	if(mi_chmod(argv[3], atoi(argv[2])) < 0){
		printf("mi_chmod, 25: Error en mi_chmod.\n");
		bumount();
		return -1;
	}
	bumount();
	return 0;
}
