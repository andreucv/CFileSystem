#include "directorios.h"

int main(int argc, char **argv){

	if(argc != 3){
		printf("mi_ls, 6: Error en els arguments.\n");
		printf("L'ús del programa és:\n \t");
		printf("./mi_ls [nom_del_dispositiu] [cami]\n");
		exit(0);
	}

	if(bmount(argv[1]) == -1){
		printf("mi_ls, 13: Error en obrir el dispositiu: %s.\n", argv[1]);
		return -1;
	}
	
	unsigned char buf[BLOCKSIZE*30];
	memset(buf, 0, sizeof(buf));	
	
	if(mi_dir(argv[2], buf) < 0){
		printf("mi_ls, 21: Error en mi_dir.\n");
		bumount();
		return -1;
	}
	bumount();

	char *p_buf = NULL;
	p_buf = strtok(buf, "|");
	while (p_buf != NULL){
		printf ("%s\n", p_buf);
		p_buf  = strtok(NULL, "|");
	}	
}
