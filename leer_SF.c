#include "directorios.h"

int main(int argc, char **argv){
	if(argc != 2){
		printf("leer_SF, 5: Error en el nombre d'arguments.\n");
		printf("L'ús és el següent:\n\t");
		printf("./leer_SF [nom_del_dispoditiu]");
		exit(0);
	}

	if(bmount(argv[1]) == -1){
		printf("leer_SF, 12: Error en obrir el dispositiu: %s.\n", argv[1]);
		return -1;
	}

	struct SB a;
	if(bread(SBpos, &a) == -1){
		printf("leer_SF, 18: Error en llegir el SuperBloque.\n");
		return -1;
	}

	printf("\t INFORMACIÓ del SUPERBLOQUE.\n");
	printf("\t Blocs del Mapa de Bits: Primer: %i; Darrer: %i.\n", a.PrimerBloqueMB, a.UltimoBloqueMB);
	printf("\t Blocs de l'Array d'Inodos: Primer: %i; Darrer: %i.\n", a.PrimerBloqueAI, a.UltimoBloqueAI);
	printf("\t Blocs del Bloc de Dades: Primer: %i; Darrer: %i.\n", a.PrimerBloqueData, a.UltimoBloqueData);
	printf("\t Blocs de Dades Lliures: %i. Totals: %i.\n", a.BloquesLibres, a.TotalBloques);
	printf("\t Nombre d'Inodos Lliures: %i. Totals: %i.\n", a.InodosLibres, a.TotalInodos);
	printf("\t Inodo Directori Arrel: %i.\n", a.InodoDirectorioRaiz);

	printf("\n\t sizeof(struct Inodo) = %i.\n", sizeof(struct Inodo));

	printf("\n\t INFORMACIÓ del MAPA DE BITS.\n");
	printf("\t Hi ha Ocupats %i de %i Blocs.\n", a.BloquesLibres, a.TotalBloques);
	 
	printf("\n\t INFORMACIÓ dels INODOS Ocupats.\n");
	struct tm *ts;
	char atime[80];
	char mtime[80];
	char ctime[80];
	struct STAT stat;
	int ninodo;
	for (ninodo=0; ninodo < a.TotalInodos; ninodo++) {
		mi_stat_f(ninodo, &stat);
		if(stat.tipo != 'l'){
			ts = localtime(&stat.ultimoAccesoDatos);
			strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
			ts = localtime(&stat.ultimaModificacionDatos);
			strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
			ts = localtime(&stat.ultimaModificacionINODO);
			strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
			printf("\rID: %d     \tTamany: %i       \tATIME: %s   MTIME: %s   CTIME: %s\n", ninodo, stat.tamBytesLogicos, atime, mtime, ctime);
		}
	}
	printf("\n");
	
	return 0;
}
