#include "directorios.h"

int main(int argc, char **argv){

	if(argc != 4){
		printf("mi_ln, 6: Error en els arguments.\n");
		printf("L'ús del programa és:\n \t");
		printf("./mi_ln [nom_del_dispositiu] [enllaç_origen] [enllaç_destí] \n");
		exit(0);
	}

	if(bmount(argv[1]) == -1){
		printf("mi_ln, 13: Error en obrir el dispositiu: %s.\n", argv[1]);
		return -1;
	}
	
	if(mi_link(argv[2], argv[3]) == -1){
		bumount();
		printf("mi_ln, 19: Error en mi_ln.\n");
		return -1;
	}
	
	bumount();
	return 0;
}
