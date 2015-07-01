#include "directorios.h"

int main(int argc, char **argv){

	if(argc != 3){
		printf("mi_rm, 6: Error en els arguments.\n");
		printf("L'ús del programa és:\n \t");
		printf("./mi_rm [nom_del_dispositiu] [enllaç] \n");
		exit(0);
	}

	if(bmount(argv[1]) == -1){
		printf("mi_rm, 13: Error en obrir el dispositiu: %s.\n", argv[1]);
		return -1;
	}

	if(mi_unlink(argv[2]) == -1){
		bumount();
		printf("mi_rm, 19: Error en mi_rm.\n");
		return -1;
	}

	bumount();
	return 0;
}


