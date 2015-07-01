#include "ficheros_basico.h"

int main(int argc, char **argv){
	if(argc != 3){
		printf("mi_mkfs.c, 5: Error en nombre de paràmetres.\n");
		printf("L'ús del programa és:\n\t");
		printf("./mi_mkfs [nom_del_dispositiu] [quantitat de blocs]\n");
		exit(0);
	}
	
	char *camino = argv[1];
	int numBloques;
	numBloques = atoi(argv[2]);

	/* Demanarem a l'usuari si està segur de fer un arxiu amb menys
	 * de 10000 blocs, ja que per a la simulació es necessiten almenys
	 * uns 9000 blocs en la majoria de casos. */ 
	if(numBloques < 10000){
		char c;
		printf("mi_mkfs, 20: Possiblement aquesta quantitat de blocs no seran suficients per fer la simulació.\n");
		printf("Voleu Continuar? [s/n]: ");
		scanf("%c", &c);
		if(c == 'n' || c == 'N') exit(0);
	}
	
	if(bmount(camino) == -1){
		printf("mi_mkfs, 27: Error en muntar el dispositiu: %s.\n", camino);
		return -1;
	}

	unsigned char buf[BLOCKSIZE];
	memset(buf, 0, BLOCKSIZE);

	int i;
	for(i = 0; i < numBloques; i++){
		if(bwrite(i, buf) == -1){
			printf("mi_mkfs, 37: Error en escriure el bloc: %i.\n", i);
			return -1;
		}
	}

	/* Inicialitzarem les tres parts del dispositiu. I reservarem
	 * l'Inodo Arrel. */
	initSB(numBloques, numBloques / 4);
	initMB(numBloques);
	initAI();
	
	reservar_inodo('d', 7);

	/* Finalment, desmuntam el dispositiu. */
	if(bumount() == -1){
		printf("mi_mkfs, 52: Error en desmuntar el dispositiu: %s.\n", camino);
		return -1;
	}
	
	printf("El dispositiu s'ha muntat correctament.\n");
	return 0;
}
