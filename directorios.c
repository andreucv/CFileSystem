#include "directorios.h"

struct Entrada entradaGlobal;

int extraer_camino(const char *camino, char *inicial, char *final){
	char *dir;
	
	/* Comprovació per assegurar que el camí és correcte i es podrà
	 * fer operacions amb ell. */
	if(camino[0] != '/'){
		printf("directorios.c, 11: Camí no vàlid.\n");
		return -1;
	}

	/* Passam la primera Slash. */
	camino++;

	/* A 'dir' es trobarà la posició de la primera
	 * barra dins 'camino'.*/
	dir = strchr(camino, '/');

	/* Si 'dir' és diferent de NULL vol dir que hi ha una Slash,
	 * i per tant, vol dir que estam davant un Directori.
	 * Si no és així, som davant un fitxer. */
	if((dir-camino) < strlen(camino)){
		/* Copiam la part de 'camino' que correspon a 'inicial'. */
		strncpy(inicial, camino, (dir-camino)+1);
		inicial[(dir-camino)+1]='\0';
		/* Passam la part copiada a 'inicio'. I copiam a 'final'. */
		camino += dir-camino;
		strcpy(final, camino);
		return 0;
	}else{
		/* Copiam tot a 'inicial', i retornam que és un Fitxer. */
		strcpy(inicial, camino);
		strcpy(final, "");
		return 1;
	}
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char modo){

	/* Definició i Inicialització de Variables que Emprarem. */
	struct Inodo in;
	struct Entrada en;
	int BloquesFich = 0, BloquesLeidos = 0;
	int nEntradasFich = 0, nEntradasLeidas = 0, nEntradasFinal = 0;

	char inicial[60], final[500];
	memset(inicial, 0, 60);
	memset(final, 0, 500);
	
	int tipo;
	unsigned char tipoaux;
	
	/* Comprovam que sigui el Directori Arrel.
	 * Si ho és, el retornam, si no, seguim amb l'execució. */
	if(strcmp(camino_parcial, "/") == 0){		
		*p_inodo = 0;
		*p_entrada = 0;
		return 0;
	}

	/* Extreurem el camí que necessitam per fer la següent crida. */
	if((tipo = extraer_camino(camino_parcial, inicial, final)) == -1){
		printf("directorios.c, 66: Error tipus (-1): Error en Extraer_Camino.\n");
		return -1;
	}
	if(tipo == 1) tipoaux = 'f';
	else if(tipo == 0) tipoaux = 'd';
	
	/* Inicialitzam l'Entrada. */
	memset(en.nombre, 0, 60);
	en.ninodo = -1;

	/*Llegim l'Inodo del Directori que està en curs. */
	in = leer_inodo(*p_inodo_dir);

	/* Calculam els límits dels nostres bucles. */
	BloquesFich = in.tamBytesLogicos / BLOCKSIZE;
	if((in.tamBytesLogicos % BLOCKSIZE) != 0) BloquesFich++;
	nEntradasFich = in.tamBytesLogicos / (sizeof(struct Entrada));
	
	struct Entrada bufAux[BLOCKSIZE/(sizeof(struct Entrada))];
	memset(bufAux, 0, BLOCKSIZE);
	
	/* Per a Llegir Entrades per Blocs (més eficient):
	 * Bucle per trobar Entrades a partir de Blocs:
	 * 	Mentre hi hagi Blocs per Llegir, i no s'hagi trobat l'entrada:
	 * 	 |	Llegim Bloc;
	 * 	 |	Mentre hi hagi Entrades per Llegir i !(hagi trobat entrada):
	 * 	 |	 |	Llegim Entrada;
	 * 	 |	fMentre;
	 * 	fMentre;
	 */
	 while((BloquesLeidos < BloquesFich) && (strcmp(inicial, en.nombre) != 0)){
		if(mi_read_f(*p_inodo_dir, bufAux, BloquesLeidos*BLOCKSIZE, BLOCKSIZE) == -1){
			printf("directorios.c, 98: Error tipus (-2): Error en llegir Entrades.\n");
			return -2;
		}
		nEntradasLeidas = 0;
		while((nEntradasFinal < nEntradasFich) && (nEntradasLeidas < (BLOCKSIZE / sizeof(struct Entrada))) && (strcmp(inicial, en.nombre) != 0)){
			en = bufAux[nEntradasLeidas];
			nEntradasLeidas++;
			nEntradasFinal++;
		}
		BloquesLeidos++;
		memset(bufAux, 0, BLOCKSIZE);
	}
	nEntradasFinal--;
	
	/* Al finalitzar aquest bucle, sabrem:
	 * 		Si nEntradasFinal == nEntradasFich(-1): hem sortit perquè
	 * 		no haurem trobat cap entrada amb aquest Nom a l'Inodo.
	 *
	 * 		Si strcmp(inicial, en.nombre) == 0 voldrà dir que l'hem trobada.
	 */	

	/* Si no hem trobat l'Entrada que cercàvem i som al final, anirem a observar el
	 * 'reservar'.
	 * 		Si és 0 (mode Consulta): 	Retornarem un error: No existeix la Entrada.
	 * 		Si és 1 (mode Reserva): 	Reservarem l'Inodo segons si és de tipus Directori o Fitxer.
	 * 									Llavors, escriurem l'Entrada en el fitxer.
	 * Sino, si no hem trobat l'Entrada, no haurem trobat el directori Intermig.
	 */
	
	if(strcmp(inicial, en.nombre) != 0 && ((strcmp(final, "") == 0 || strcmp(final, "/") == 0))){
		switch(reservar){
			case 0: printf("directorios.c, 129: Error tipus (-3): No existeix Entrada.\n");
					return -3;
					break;
			case 1: strcpy(en.nombre, inicial);
					if((en.ninodo = reservar_inodo(tipoaux, modo)) == -1){
						printf("directorios.c, 134: Error en reservar un Inodo.\n");
						return -1;
					}
					nEntradasFinal++;
					if(mi_write_f(*p_inodo_dir, &en, (nEntradasFinal)*sizeof(struct Entrada), sizeof(struct Entrada)) == -1){
						if(en.ninodo != -1) liberar_inodo(en.ninodo);
						printf("directorio.c, 140: Error tipus (-4): Error en escriure una Entrada.\n");
						return -4;
					}
					break;
		}
	}
	else if(strcmp(inicial, en.nombre) != 0 && ((strcmp(final, "") != 0 || strcmp(final, "/") != 0))){
		printf("directorios.c, 147: Error tipus (-6): No existeix el Directori Intermig = %s.\n", inicial);
		return -6;
	}
	else if(strcmp(inicial, en.nombre) == 0 && ((((strcmp(final, "") == 0 || strcmp(final, "/") == 0)))) && (reservar == 1)){
		printf("directorios.c, 151: Warning tipus (-5): Ja existeix l'entrada.\n");
		return -5;
	}

	/* Si hem arribat al final, tallam la recursió. */
	if((strcmp(final, "") == 0 || strcmp(final, "/") == 0)){
		*p_inodo = en.ninodo;
		*p_entrada = nEntradasFinal;
		return 0;
	}
	else{
		*p_inodo_dir = en.ninodo;
		return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, modo);
	}
}

int mi_creat(char *camino, unsigned char permisos){
	/* Funció que crida a 'buscar_entrada' per crear un directori
	 * o un fitxer, especifict a camino. */
	/* Inicialitzam variables. */
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;

	/* Afegim els controls per entrar a la secció crítica. */
	mi_waitSem();
	/* Fem la crida. Els errors estan resolts a la capa superior. */
	int en = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos);

	memcpy(entradaGlobal.nombre, camino, 60);
	entradaGlobal.ninodo = p_inodo;
	
	mi_signalSem();
	return en;
}

int mi_dir(char *camino, char *buffer){
	/* Funció que imprimeix el contingut del directori que es posa a
	 * camino. */
	/* Inicialitzam variables. */
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	
	/* Cridam a 'buscar_entrada'.
	 * En aquest cas hem de resoldre en el mateix mètode els errors,
	 * perquè no podem continuar l'execució amb un error d'aquests. */
	switch(buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)){
		case -1: printf("directorios.c, 199: Error en Extraer Camino.\n"); return -1; break; 
		case -2: printf("directorios.c, 200: Error en Llegir Entrada.\n"); return -2; break; 
		case -3: printf("directorios.c, 201: Error: No existeix l'Entrada.\n"); return -3; break; 
		case -4: printf("directorios.c, 202: Error en Escriure Entrada.\n"); return -4; break; 
		case -5: printf("directorios.c, 203: Error: Entrada ja existent.\n"); return -5; break;
		case -6: printf("directorios.c, 204: Error en trobar Directori Intermig.\n"); return -6; break; 
	}

	/* Llegim l'Inodo i miram si és un Directori i té permisos de lectura. */
	struct Inodo in;
	in = leer_inodo(p_inodo);
	
	if(in.tipo == 'f' || in.permisos & 2 != 2){
		printf("directorios.c, 212: Errors en el tipus o els permisos.\n");
		return -1;
	}

	/* Calculam els límits dels nostres Bucles. */
	int BloquesFichero = in.tamBytesLogicos / BLOCKSIZE;
	int BloquesLeidos = 0;
	int EntradasFichero = in.tamBytesLogicos / sizeof(struct Entrada);
	int EntradasLeidas = 0, EntradasFinal = 0;
 	
	struct Entrada entrada;
	struct Entrada bAux[BLOCKSIZE / sizeof(struct Entrada)];

	/*
	 * Algoritme del Bucle:
	 * 		Mentre hi hagin blocs per llegir:
	 * 		 |	Llegeix un Bloc.
	 * 		 |	Mentre hi hagi Entrades per llegir:
	 * 		 |	 |	Llegeix Entrada;
	 * 		 |	 |	Concatena a un Buffer.
	 * 		 |	fMentre;
	 * 		fMentre.
	 *
	 * Llegim totes les entrades que té el nostre Inodo en particular. */
	for(BloquesLeidos = 0; BloquesLeidos <= BloquesFichero; BloquesLeidos++){
		printf("237: Inodo Llegim = %i.\n", p_inodo); 
		if(mi_read_f(p_inodo, bAux, BloquesLeidos*BLOCKSIZE, BLOCKSIZE) == -1){
			printf("directorios.c, 238: Error en llegir entrada.\n");
			return -1;
		}
		
		while(EntradasFinal < EntradasFichero && EntradasLeidas < (BLOCKSIZE/sizeof(struct Entrada))){
			entrada = bAux[EntradasLeidas];
			in = leer_inodo(entrada.ninodo);
			printf("246: Entrada.nombre = %s.\n", entrada.nombre);
			if((in.permisos & 4) == 4) strcat(buffer, "r");
			else strcat(buffer, "-");
			if((in.permisos & 2) == 2) strcat(buffer, "w");
			else strcat(buffer, "-");
			if((in.permisos & 1) == 1) strcat(buffer, "x");
			else strcat(buffer, "-");
			strcat(buffer, "\t");
			
			char nom[60];
			sprintf(nom, "%-15s", entrada.nombre);
			strcat(buffer, nom);
			strcat(buffer, "\t");

			char tam[20];
			sprintf(tam, "%-15i", in.tamBytesLogicos);
			strcat(buffer, tam);
						
			struct tm *tm; 
			char tmp[100];
			tm = localtime(&in.mtime); 
			sprintf(tmp,"%d-%02d-%02d %02d:%02d:%02d\t",tm->tm_year+1900, tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
			strcat(buffer, tmp);
		
			strcat(buffer, "|");
			EntradasLeidas++;
			EntradasFinal++;
		}
		EntradasLeidas = 0;
	}	
}

int mi_link(char *caminoOrigen, char *caminoDest){
	/* Inicialització de Variables. */
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	int en = 0;

	mi_waitSem();
	/*Cridam a 'buscar_entrada'. */
	if(en = buscar_entrada(caminoOrigen, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0) < 0){
		switch(en){
			case -1: printf("directorios.c, 287: Error en Extraer Camino.\n"); break; 
			case -2: printf("directorios.c, 288: Error en Llegir Entrada.\n"); break; 
			case -3: printf("directorios.c, 289: Error: No existeix l'Entrada.\n"); break; 
			case -4: printf("directorios.c, 290: Error en Escriure Entrada.\n"); break; 
			case -5: printf("directorios.c, 291: Error: Entrada ja existent.\n"); break;
			case -6: printf("directorios.c, 292: Error en trobar Directori Intermig.\n"); break; 
		}
		mi_signalSem();
		return en;
	}

	/* Seguirem endavant si l'Inodo és de tipus fitxer. */
	struct STAT stat;
	if((mi_stat_f(p_inodo, &stat) == -1) || (stat.tipo != 'f')){
		printf("directorios.c, 301: Error: Camí Origen no és un fitxer.\n", p_inodo);
		mi_signalSem();
		return -1;
	}

	/* Llegim l'Entrada que ens ha trobat l'anterior 'buscar_entrada'.*/
	struct Entrada entrada;
	if(mi_read_f(p_inodo_dir, &entrada, (p_entrada)*(sizeof(struct Entrada)), sizeof(struct Entrada)) == -1){
		printf("directorios.c, 309: Error en llegir una entrada.\n");
		mi_signalSem();
		return -1;
	}	

	/* Guardam l'apuntador a l'Inodo que ens ha retornat, per poder
	 * després linkar. */
	int ninodo = entrada.ninodo;
	
	/* Inicialitzam una altra vegada per poder començar des del
	 * principi. I cridam a 'buscar_entrada' per a que ens crei una
	 * nova entrada.*/
	p_inodo_dir = 0;
	p_inodo = 0;
	p_entrada = 0;

	if(en = buscar_entrada(caminoDest, &p_inodo_dir, &p_inodo, &p_entrada, 1, 6) < 0){
		switch(en){
			case -1: printf("directorios.c, 327: Error en Extraer Camino.\n"); break; 
			case -2: printf("directorios.c, 328: Error en Llegir Entrada.\n"); break; 
			case -3: printf("directorios.c, 329: Error: No existeix l'Entrada.\n"); break; 
			case -4: printf("directorios.c, 330: Error en Escriure Entrada.\n"); break; 
			case -6: printf("directorios.c, 332: Error en trobar Directori Intermig.\n"); break; 
		}
		mi_signalSem();
		return en;
	}
	if(en == -5){
		p_inodo_dir = 0;
		p_inodo = 0;
		p_entrada = 0;
		
		buscar_entrada(caminoDest, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
	}
	
	/* Seguirem endavant si l'Inodo és de tipus fitxer. */
	if((mi_stat_f(p_inodo, &stat) == -1) || (stat.tipo != 'f')){
		printf("directorios.c, 340: Error: Camí Destí no és un fitxer.\n");
		mi_signalSem();
		return -1;
	}
	
	if(mi_read_f(p_inodo_dir, &entrada, (p_entrada)*sizeof(struct Entrada), sizeof(struct Entrada)) == -1){
		printf("directorios.c, 346: Error en llegir una entrada.\n");
		mi_signalSem();
		return -1;
	}
	
	/* Alliberarem l'Inodo que s'ha reservat en la segona crida de
	 * 'buscar_entrada' perquè només necessitavem l'entrada. */	 
	if(liberar_inodo(entrada.ninodo) == -1){
		printf("directorios.c, 354: Error en alliberar l'Inodo: %i.\n", entrada.ninodo);
		mi_signalSem();
		return -1;
	}
	
	/* Esto es el enlace a la Entrada nueva (de Camino2) desde la entrada de Camino1. */ 
	entrada.ninodo = ninodo;
	if(mi_write_f(p_inodo_dir, &entrada, (p_entrada)*sizeof(struct Entrada), sizeof(struct Entrada)) == -1){
		printf("directorios.c, 362: Error en escriure una Entrada.\n");
		mi_signalSem();
		return -1;
	}

	struct Inodo in;
	in = leer_inodo(ninodo);
	in.EntradasDirectorio++;
	in.ctime = time(NULL);
	if(escribir_inodo(in, ninodo) == -1){
		printf("directorios.c, 372: Error en escriure l'Inodo: %i.\n", ninodo);
		mi_signalSem();
		return -1;
	}
	mi_signalSem();
	return 0;
}
	
int mi_unlink(char *camino){
	/* Funció que esborra l'entrada a un directori. Si és la darrera
	 * entrada, aquest directori s'ha d'eliminar. */
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	int en;

	mi_waitSem();
	if(en = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0) < 0){
	/* Anem a buscar l'Inodo en el qual hi ha l'entrada de camino. */
		switch(en){
			case -1: printf("directorios.c, 392: Error en Extraer Camino.\n"); break; 
			case -2: printf("directorios.c, 393: Error en Llegir Entrada.\n"); break; 
			case -3: printf("directorios.c, 394: Error: No existeix l'Entrada.\n"); break; 
			case -4: printf("directorios.c, 395: Error en Escriure Entrada.\n"); break; 
			case -5: printf("directorios.c, 396: Error: Entrada ja existent.\n"); break;
			case -6: printf("directorios.c, 397: Error en trobar Directori Intermig.\n"); break; 
		}
		mi_signalSem();
		return en;
	}
	/* Llegim l'Inodo i calculam el nombre d'entrades que té. */
	struct Entrada entrada;
	struct Inodo in;
	in = leer_inodo(p_inodo_dir);
	struct STAT stat;
	mi_stat_f(p_inodo, &stat);

	if(stat.tipo == 'd' && stat.tamBytesLogicos > 0){
		printf("directorios.c, 410: Error en esborrar un directori ple.\n");
		mi_signalSem();
		return -1;
	}

	if(p_inodo == 0){
		printf("directorios.c, 416: No es pot esborrar el directori Arrel.\n");
		mi_signalSem();
		return -1;
	}

	/* Llavors miram si és la darrera entrada:
	 * 		Si no ho és, esborrarem l'entrada.
	 * 			El que feim és llegir la darrera entrada i la
	 *			col·locam al lloc de la que hem d'eliminar. 
	 * 		Si ho és, alliberarem només l'Entrada. */
	if(p_entrada != (in.tamBytesLogicos / (sizeof(struct Entrada)))-1){
		mi_read_f(p_inodo_dir, &entrada, in.tamBytesLogicos - sizeof(struct Entrada), sizeof(struct Entrada));
		mi_write_f(p_inodo_dir, &entrada, p_entrada*sizeof(struct Entrada), sizeof(struct Entrada));
	}
	mi_truncar_f(p_inodo_dir, in.tamBytesLogicos-(sizeof(struct Entrada)));
	
	in = leer_inodo(p_inodo);
	in.EntradasDirectorio--;
	
	if(in.EntradasDirectorio <= 0) liberar_inodo(p_inodo);
	else{
		in.ctime = time(NULL);

		/* Finalment, escrivim l'Inodo si no ha estat alliberat. */
		if(escribir_inodo(in, p_inodo) == -1){
			printf("directorios.c, 440: Error en escriure l'Inodo: %i.\n", p_inodo);
			mi_signalSem();
			return -1;
		}
	}
	mi_signalSem();
	return 0;
}

int mi_chmod(char *camino, unsigned char permisos){
	/* Funció que canvia els permisos d'un Inodo segons els que
	 * s'especifiquen per paràmetre en aquesta funció. */
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	int en = 0;
	
	if((strcmp(entradaGlobal.nombre, camino)) == 0) p_inodo = entradaGlobal.ninodo;
	/* Anem a buscar l'Inodo en el qual hi ha l'entrada de camino. */
	else{
		if(en = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0) < 0){
			switch(en){
				case -1: printf("directorios.c, 462: Error en Extraer Camino.\n"); break; 
				case -2: printf("directorios.c, 463: Error en Llegir Entrada.\n"); break; 
				case -3: printf("directorios.c, 464: Error: No existeix l'Entrada.\n"); break; 
				case -4: printf("directorios.c, 465: Error en Escriure Entrada.\n"); break; 
				case -5: printf("directorios.c, 466: Error: Entrada ja existent.\n"); break;
				case -6: printf("directorios.c, 467: Error en trobar Directori Intermig.\n"); break; 
			}
			mi_signalSem();
			return en;
		}
		memcpy(entradaGlobal.nombre, camino, 60);
		entradaGlobal.ninodo = p_inodo;
	}

	/* Cridam a la funció que modifica els permisos al nivell
	 * immediat inferior. */
	mi_chmod_f(p_inodo, permisos);
	mi_signalSem();
	return 0;
}

int mi_stat(char *camino, struct STAT *p_stat){
	/* Funció que retorna una struct STAT de l'Inodo al qual es
	 * refereix un 'camino'. */
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	
	int e = 0;
	
	/* Anem a buscar l'Inodo en el qual hi ha l'entrada de camino. */
	switch(e = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)){
		case -1: printf("directorios.c, 492: Error en Extraer Camino.\n"); return e; break; 
		case -2: printf("directorios.c, 493: Error en Llegir Entrada.\n"); return e; break; 
		case -3: printf("directorios.c, 494: Error: No existeix l'Entrada.\n"); return e; break; 
		case -4: printf("directorios.c, 495: Error en Escriure Entrada.\n"); return e; break; 
		case -5: printf("directorios.c, 496: Error: Entrada ja existent.\n"); return e; break;
		case -6: printf("directorios.c, 497: Error en trobar Directori Intermig.\n"); return e; break; 
	}

	/* Per acabar, es crida a la funció implicada. */
	mi_stat_f(p_inodo, p_stat);
	return 0;
}

int mi_read(char *camino, void *buf, unsigned int offset, unsigned int nbytes){
	/* Funció que llegeix d'un Inodo al qual es refereix 'camino'. */
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	int en = 0;
	
	/* Anem a buscar l'Inodo en el qual hi ha l'entrada de camino. */
	if((strcmp(entradaGlobal.nombre, camino)) == 0) p_inodo = entradaGlobal.ninodo;
	else{
		if(en = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0) < 0){
			switch(en){
				case -1: printf("directorios.c, 517: Error en Extraer Camino.\n"); break; 
				case -2: printf("directorios.c, 518: Error en Llegir Entrada.\n"); break; 
				case -3: printf("directorios.c, 519: Error: No existeix l'Entrada.\n"); break; 
				case -4: printf("directorios.c, 520: Error en Escriure Entrada.\n"); break; 
				case -5: printf("directorios.c, 521: Error: Entrada ja existent.\n"); break;
				case -6: printf("directorios.c, 522: Error en trobar Directori Intermig.\n"); break; 
			}
			return en;
		}		
		memcpy(entradaGlobal.nombre, camino, 60);
		entradaGlobal.ninodo = p_inodo;
	}

	/* Llegeix al Inodo que ha sortit producte de l'anterior operació. */
	if(mi_read_f(p_inodo, buf, offset, nbytes) == -1){
		printf("directorios.c: mi_read: 532: Error en llegir amb mi_read_f.\n");
		return -1;
	}
	return 0;
}	  

int mi_write(char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
	/* Funció que escriu d'un Inodo al qual es refereix 'camino'. */
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	int en = 0;

	if(offset < 0){
		printf("directorios.c, 546: Error en l'offset: %i.\n", offset);
		return -1;
	}
	
	mi_waitSem();
	if((strcmp(entradaGlobal.nombre, camino)) == 0){
		p_inodo = entradaGlobal.ninodo;
	}
	else{
		if(en = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0) < 0){ 
			switch(en){
				case -1: printf("directorios.c, 557: Error en Extraer Camino.\n"); break; 
				case -2: printf("directorios.c, 558: Error en Llegir Entrada.\n"); break; 
				case -3: printf("directorios.c, 559: Error: No existeix l'Entrada.\n"); break; 
				case -4: printf("directorios.c, 560: Error en Escriure Entrada.\n"); break; 
				case -5: printf("directorios.c, 561: Error: Entrada ja existent.\n"); break;
				case -6: printf("directorios.c, 562: Error en trobar Directori Intermig.\n"); break; 
			}
			mi_signalSem();
			return en;
		}
		strcpy(entradaGlobal.nombre, camino);
		entradaGlobal.ninodo = p_inodo;
	}
	
	/* Escrivim al Inodo que ha sortit producte de l'anterior operació. */
	if(mi_write_f(p_inodo, buf, offset, nbytes) == -1){
		printf("directorios.c, 573: Error en escriure amb mi_write_f a l'Inodo: %i.\n", p_inodo);
		mi_signalSem();
		return -1;
	}
	mi_signalSem();
	return 0;
}
