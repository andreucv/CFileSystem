#include "directorios.h"

int main(int argc,char**argv){

	if(argc != 3){
		printf("mi_stat, 6: Error en els arguments.\n");
		printf("L'ús del programa és:\n \t");
		printf("./mi_stat [nom_del_dispositiu] [cami]\n");
		exit(0);
	}

    if(bmount(argv[1]) == -1){
		printf("mi_stat, 13: Error en obrir el dispositiu: %s.\n", argv[1]);
		return -1;
	}
	
    struct STAT p_stat;
    
    if(mi_stat(argv[2], &p_stat) < 0){
        printf("mi_stat, 20: Error en mi_stat.\n");
        return -1;
	}

	printf("\tTipus: \t\t\t\t\t");
	if(p_stat.tipo == 'd') printf("Directori\n");
	else if(p_stat.tipo == 'f') printf("Fitxer\n");

	printf("\tPermisos:  \t\t\t\t");
	if((p_stat.permisos & 4) == 4) printf("r");
	else printf("-");
	if((p_stat.permisos & 2) == 2) printf("w");
	else printf("-");
	if((p_stat.permisos & 1) == 1) printf("x");
	else printf("-");
	printf("\n");

	printf("\tNombre d'Entrades:\t\t\t%i\n", p_stat.EntradasDirectorio);	
	printf("\tBlocs Ocupats:\t\t\t\t%i\n", p_stat.BloquesDataOcupados);
	printf("\tTamany en Bytes Lògics:\t\t\t%i\n", p_stat.tamBytesLogicos);
	
	struct tm *tm;
	tm = localtime(&p_stat.ultimoAccesoDatos);
	printf("\tDarrer Accés a les Dades: \t\t%d-%02d-%02d %02d:%02d:%02d\t\n",tm->tm_year+1900, tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	tm = localtime(&p_stat.ultimaModificacionDatos);
	printf("\tDarrera Modificació de les Dades: \t%d-%02d-%02d %02d:%02d:%02d\t\n",tm->tm_year+1900, tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	tm = localtime(&p_stat.ultimaModificacionINODO);
	printf("\tDarrera Modificació de l'Inodo: \t%d-%02d-%02d %02d:%02d:%02d\t\n\n",tm->tm_year+1900, tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);

	bumount();
}
