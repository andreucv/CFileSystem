#include "simulacion.h"

int main(int argc, char **argv){

	if(argc != 2){
		printf("verificacion.c, 6:Error en els arguments.\n");
		printf("L'ús del programa és:\n \t.");
		printf("/verificacion [nom_del_dispositiu] \n");
		exit(0);
	}

	if(bmount(argv[1]) == -1){
		printf("verificacion.c, 13: Error en obrir el dispositiu: %s.\n", argv[1]);
		return -1;
	}

	unsigned char dirSim[60];
	sprintf(dirSim, "/");
	
	struct Entrada en;
	mi_read(dirSim, &en, 0, sizeof(struct Entrada));
	sprintf(dirSim, "/%s", en.nombre);
	
	struct STAT stat;
	mi_stat(dirSim, &stat);
	
	if((stat.tamBytesLogicos / (sizeof(struct Entrada))) != MAX_PROCESOS){
		printf("verificacion.c, 28: Error en el nombre de processos.\n");
		return -1;
	}

	int i = 0, j = 0;
	int ninodo = en.ninodo;
	struct Entrada enAux;
	struct STAT statAux;
	struct Registro registros[BLOCKSIZE/sizeof(struct Registro)];

	unsigned char blanco[sizeof(struct Registro)];
	memset(blanco, 0, sizeof(struct Registro));
	
	int BloquesInicial = 0, BloquesFinal;
	FILE *f = fopen("informe.txt", "w");
	
	for(i = 0; i < MAX_PROCESOS; i++){		
		mi_read_f(ninodo, &en, i*(sizeof(struct Entrada)), sizeof(struct Entrada));

		mi_read_f(en.ninodo, &enAux, 0, sizeof(struct Entrada));

		mi_stat_f(enAux.ninodo, &statAux);

		BloquesFinal = statAux.tamBytesLogicos / BLOCKSIZE;
		if((statAux.tamBytesLogicos % BLOCKSIZE) != 0) BloquesFinal++;

		struct Registro regMayor, regMenor, regPrincipio, regFinal;
		regMenor.posFich = UINT_MAX;
		regMayor.posFich = 0;
		regPrincipio.time = time(NULL);
		regFinal.time = time(NULL);
		
		char *p_pid;
		p_pid = strchr(en.nombre, '_');
		int pidAux = atoi(p_pid+1);
		int validados = 0;
		
		for(BloquesInicial = 0; BloquesInicial < BloquesFinal; BloquesInicial++){
			mi_read_f(enAux.ninodo, registros, BloquesInicial*BLOCKSIZE, BLOCKSIZE);
	
			for(j = 0; j < (BLOCKSIZE / sizeof(struct Registro)); j++){
				if(memcmp(&registros[j], blanco, sizeof(struct Registro)) != 0){
					if(pidAux == registros[j].pid){
						validados++;
						if(registros[j].posFich < regMenor.posFich){
							memcpy(&regMenor, &registros[j], sizeof(struct Registro));
						}
						if(registros[j].posFich > regMayor.posFich){
							memcpy(&regMayor, &registros[j], sizeof(struct Registro));
						}
						if(registros[j].time < regPrincipio.time){
							memcpy(&regPrincipio, &registros[j], sizeof(struct Registro));
						}
						if(registros[j].time > regFinal.time){
							memcpy(&regFinal, &registros[j], sizeof(struct Registro));
						}
					}
				}
			}
		}
		
		char tiempo[60];
		char cadena[500];
		struct tm *tm;
		sprintf(cadena, "Verificació del procés %i:\n", pidAux);
		fputs(cadena, f);
		printf("%s", cadena);
		sprintf(cadena, "\tNombre de Validacions: %i.\n", validados);
		fputs(cadena, f);
		printf("%s", cadena);
		sprintf(cadena, "\tInformació dels Registres:\n");
		fputs(cadena, f);
		printf("%s", cadena);
		sprintf(cadena, "\t\tPosició menor = %i.\n", regMenor.posFich);
		fputs(cadena, f);
		printf("%s", cadena);
		sprintf(cadena, "\t\tPosició major = %i.\n", regMayor.posFich);
		fputs(cadena, f);
		printf("%s", cadena);
		tm = localtime(&regPrincipio.time);
		sprintf(tiempo, "%d-%02d-%02d %02d:%02d:%02d",
                    tm->tm_year + 1900,
                    tm->tm_mon + 1,
                    tm->tm_mday,
                    tm->tm_hour,
                    tm->tm_min,
                    tm->tm_sec);
		sprintf(cadena, "\t\tTemps primer = %s\n", tiempo);
		fputs(cadena, f);
		printf("%s", cadena);
		tm = localtime(&regFinal.time);
		sprintf(tiempo, "%d-%02d-%02d %02d:%02d:%02d",
                    tm->tm_year + 1900,
                    tm->tm_mon + 1,
                    tm->tm_mday,
                    tm->tm_hour,
                    tm->tm_min,
                    tm->tm_sec);
		sprintf(cadena, "\t\tTemps darrer = %s\n", tiempo);
		fputs(cadena, f);
		printf("%s", cadena);
	}
	fclose(f);
}
