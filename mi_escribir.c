#include "ficheros.h"

int main(int argc, char **argv){

	if(argc != 5){
		printf("mi_escribir, 6: Error en els arguments.\n");
		printf("L'ús del programa és:\n \t.");
		printf("./mi_escribir [nom_del_dispositiu] [offset] [cami] '[text per escriure]' \n");
		exit(0);
	}
	
	if(bmount(argv[1]) == -1){
		printf("mi_escribir, 13: Error en obrir el dispositiu: %s.\n", argv[1]);
		return -1;
	}

	if(mi_write(argv[3], argv[4], atoi(argv[2]), strlen(argv[4])) == -1){
		printf("mi_escribir, 18: Error en escriure.\n");
		bumount();
		return -1;
	}
	
	bumount();
	return 0;
}
