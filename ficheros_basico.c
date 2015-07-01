#include "ficheros_basico.h"

int tamMB(unsigned int nbloques){
	/* Funció que retorna el nombre de Blocs que ha de tenir
	 * el Mapa de Bits del dispositiu.*/
	int n = nbloques / 8;
	if(n % BLOCKSIZE == 0) return (n / BLOCKSIZE);
	else return ((n / BLOCKSIZE) + 1);
}

int tamAI(unsigned int ninodos){
	/* Funció que retorna el nombre de Bloc que ha de tenir
	 * l'Array d'Inodos del dispositiu. */
	int n = sizeof(struct Inodo) * ninodos;
	if(n % BLOCKSIZE == 0)return (n / BLOCKSIZE);
	else return ((n / BLOCKSIZE) + 1);
}

int initSB(unsigned int nbloques, unsigned int ninodos){
	/* Funció que inicialitza el SuperBloque del dispositiu
	 * segons el nombre de blocs totals (nbloques) i
	 * el nombre d'inodos (ninodos). */

	struct SB a;

	/* Nombre dels Blocs Delimitadors de les parts. */
	a.PrimerBloqueMB = 1; 
	a.UltimoBloqueMB = 1 + tamMB(nbloques) - 1;
	a.PrimerBloqueAI = a.UltimoBloqueMB + 1;
	a.UltimoBloqueAI = 1 + tamMB(nbloques) + tamAI(ninodos) - 1;
	a.PrimerBloqueData = a.UltimoBloqueAI + 1;
	a.UltimoBloqueData = nbloques - 1;
	
	a.InodoDirectorioRaiz = 0;
	a.PrimerInodoLibre = 0;
	
	a.BloquesLibres = nbloques;	
	a.InodosLibres = ninodos;
	
	a.TotalBloques= nbloques;
	a.TotalInodos = ninodos;
	
	memset(&a.padding, 0, sizeof(a.padding));
	
	if(bwrite(SBpos, &a) == -1){
		printf("ficheros_basico.c, 46: Error a l'escriure al dipositiu a la posició: %i.\n", SBpos);
		return -1;
	}
	return 0;
}

int initMB(unsigned int nbloques){
	/* Funció que inicialitza el Mapa de Bits del dispositiu. */

	struct SB a;
	if(bread(SBpos, &a) == -1){
		printf("ficheros_basico.c, 57: Error al llegir el SB.\n");
		return -1;
	}

	/* Definim un buffer i l'inicialitzam a 0. */
	unsigned char buffer[BLOCKSIZE];	
	memset(buffer, 0, BLOCKSIZE);
	
	/* Fem un recorregut per tots els blocs del MB col·locant el
	 * buffer ple de zeros al MB. */
	int i = 0;
	for(i = a.PrimerBloqueMB; i < a.PrimerBloqueAI; i++){
		if(bwrite(i, buffer) == -1){
			printf("ficheros_basico.c, 70: Error a l'escriure al dispositiu a la posició: %i.\n", i);
			return -1;
		}
	}
	
	/* Posam els valors al Mapa de Bits que corresponen als Blocs
	 * ocupats pel SuperBloque, Mapa de Bits i Array de Inodos. */
	for(i = 0; i < a.PrimerBloqueData; i++){
		if(escribir_bit(i, 1) == -1){
			printf("ficheros_basico.c, 79: Error en escriure a 1 el bit: %i.\n", i);
			return -1;
		}
	}	
	return 0;	
}

int initAI(){	
	/* Funció que inicialitza l'Array d'Inodos. */

	/* Llegim el SuperBloque. */
	struct SB a;
	if(bread(SBpos,&a) == -1){
		printf("ficheros_basico.c, 92: Error al llegir el SB.\n");
		return -1;
	}

	/* Iteram per tots els Inodos del dispositiu:
	 * 		Per cada Bloc de l'Array d'Inodos entram dins cada un
	 * 		dels 8 Inodos de cada Bloc.
	 * 		I els inicialitzam.
	 * Cada Inodo Lliure contindrà en el punteroDirecto[0] un punter
	 * al següent Inodo Lliure. */ 
	int i = 0, j = 0, x = 1;
	struct Inodo inodos[BLOCKSIZE/(sizeof(struct Inodo))]; 
	memset(inodos, 0, BLOCKSIZE);
	for(i = a.PrimerBloqueAI; i <= a.UltimoBloqueAI; i++){
		for(j = 0; j < BLOCKSIZE/(sizeof(struct Inodo)); j++){
			inodos[j].tipo = 'l';
			if(x < a.TotalInodos){
				inodos[j].punterosDirectos[0] = x;
				x++;
			}
			else{
				inodos[j].punterosDirectos[0] = UINT_MAX;
				break;
			}							
		}
		if(bwrite(i,&inodos) == -1){
			printf("ficheros_basico.c, 118: Error a l'escriure al dispositiu a la posició: %i.\n", i);
			return -1;
		}
		memset(inodos, 0, BLOCKSIZE);
	}
	
	if(bwrite(SBpos, &a) == -1) {
		printf("ficheros_basico.c, 125: Error a l'escriure el SB.\n");
		return -1;
	}
	return 0;
}

int escribir_bit(unsigned int nbloque, unsigned int bit){
	/* Funció en la que escriurem el bit del bloc (nbloque) que
	 * vulguem amb el signe (bit) que vulguem. */
	int e = 0;

	/* Llegim el SuperBloque. */
	struct SB a;
	if(bread(SBpos, &a) == -1){
		printf("ficheros_basico.c, 139: Error en llegir SB.\n");
		return -1;
	}

	/* Calculam la posició del bit que haurem de llegir.
	 * A posbyte hi ha la posició del byte, amb la que calcularem
	 * el Bloc que hem de llegir.
	 * A posbit hi ha la posició del bit dins el byte que haurem
	 * de canviar. */
	 
	int posbyte = nbloque / 8;
	int posbit = nbloque % 8;
	int numbloque = posbyte / BLOCKSIZE + a.PrimerBloqueMB;
	posbyte = posbyte % BLOCKSIZE;
	
	unsigned char buf[BLOCKSIZE];
	
	unsigned char mascara = 128;
	mascara >>= posbit;

	if(numbloque == 0){
		printf("ficheros_basico.c, 160: Error en bloc SB.\n");
		return -1;
	}
	
	/* Llegirem el Bloc en el que hem de canviar el bit de valor. */
	if(bread(numbloque, buf) == -1){
		printf("ficheros_basico.c, 166: Error en llegir el bloc: %i.\n", numbloque);
		return -1;
	}

	/* Canviam el bit de valor sense tocar els demés, i actualitzam
	 * la informació del SuperBloc.
	 * Si el nombre inserit és erroni es retornarà un valor d'error. */
	switch(bit){
		case 0: buf[posbyte]  &= ~mascara;
				a.BloquesLibres++;
				break;
		case 1: buf[posbyte] |= mascara;
				a.BloquesLibres--;
				break;
		default:
				printf("ficheros_basico.c, 181: El bit no té el valor apropiat.\n");
				return -1;
	}

	/* Primer escriurem el Bloc del Mapa de Bits ja canviat i
	 * després el SuperBloc de manera que tot quedi actualitzat. */	
	if(bwrite(numbloque, buf) == -1){
		printf("ficheros_basico.c, 188: Error en escriure el bloc: %i.\n", numbloque);
		return -1;
	}
		
	if(bwrite(SBpos, &a) == -1){
		printf("ficheros_basico.c, 193: Error en escriure el bloc del SB.\n");
		return -1;
	}
	return 0;
}

unsigned char leer_bit(unsigned int nbloque){
	/* Funció que retorna el valor del Bloc del Mapa de Bits
	 * corresponent al número de Bloc (nbloque).
	 * El valor retornat és un unsigned char, però és fàcilment
	 * intercanviable amb un càsting (int). */
	struct SB a;
	if(bread(SBpos, &a) == -1){
		printf("ficheros_basico.c, 206: Error en llegir SB.\n");
		return -1;
	}
	/* Calculam la posició del bit que haurem de llegir.
	* A posbyte hi ha la posició del byte, amb la que calcularem
	* el Bloc que hem de llegir.
	* A posbit hi ha la posició del bit dins el byte que haurem
	* de canviar. */
	int posbyte = nbloque / 8;
	int posbit =  nbloque % 8;
	int numbloque = posbyte/BLOCKSIZE + a.PrimerBloqueMB;
	posbyte = posbyte % BLOCKSIZE;
	
	unsigned char buf[BLOCKSIZE];
	
	unsigned char mascara = 128;
	mascara >>= posbit;

	/* Llegim el bloc que necessitam i llavors calculam el valor
	 * que quedarà emmagatzemat a màscara. */
	if(bread(numbloque, buf) == -1){
		printf("ficheros_basico.c, 228: Error en llegir el bloc: %i.\n", numbloque);
		return -1;
	}
	
	mascara &= buf[posbyte];
	mascara >>= (7- posbit);
	
	return mascara;
}

int reservar_bloque(){
	/* Funció que reservarà un Bloc de Dades i retornarà
	 * el número del Bloc reservat. */
	
	struct SB a;
	if(bread(SBpos, &a) == -1){
		printf("ficheros_basico.c, 243: Error en llegir el SB. \n");
		return -1;
	}

	/* Hem de comprovar si existeixen blocs lliures per poder
	 * reservar-los. */
	if(a.BloquesLibres <= 0){
		printf("ficheros_basico.c, 250: No hi ha més blocs disponibles.\n");
		return -1;
	}

	int i = a.PrimerBloqueMB - 1;			
	unsigned char b[BLOCKSIZE];
	memset(b, 255, BLOCKSIZE);
	unsigned char bAux[BLOCKSIZE];
	memset(bAux, 255, BLOCKSIZE);

	/* Llegim el primer bloc del Mapa de Bits. Llavors,
	 * iterativament anirem comprovant si hi ha algun zero. */		
	while(memcmp(b, bAux, BLOCKSIZE) == 0){
		i++;
		if(bread(i, b) == -1){
			printf("ficheros_basico.c, 265: Error en llegir el bloc: %i. \n", i);
			return -1;
		}							
	}	
	/* Ara, la variable 'i' conté el valor del primer bloc del
	 * Mapa de Bits que conté, almenys, un 0. 
	 * I 'b' conté el bloc sencer. */	
		
	unsigned char mascara = 128;
	int posbyte = 0, posbit = 0;
	while(b[posbyte] == 255){
		posbyte++;
	}
	/* Ara ja tenim la posició del byte on es troba el 0. */
			
	while(b[posbyte] & mascara){
		posbit++;
		mascara >>= 1;
	}
	/* Finalitzat aquesta iteració, a posbit tenim el nombre de
	 * bits del byte que no són 0 davant del primer. */
			
	int numbloque = ((i - a.PrimerBloqueMB)*8)*1024 + posbyte * 8 + posbit;
	if(escribir_bit(numbloque, 1) == -1){
		printf("ficheros_basico.c, 289: Error en escriure el bit: %i.\n", numbloque);
		return -1;
	}
		
	/* El control de l'increment i el decrement dels blocs ocupats
	 * es duu a terme en un nivell més baix: escribir_bit.
	 * Ja que es controla amb el valor del bit el nombre de Blocs
	 * ocupats i lliures. */ 
	return numbloque;
}

int liberar_bloque(unsigned int nbloque){
	/* Funció que allibera un Bloc de Dades ocupat o no, donant el
	 * valor 0 en el Mapa de Bits corresponent al bloc. */
	if(escribir_bit(nbloque, 0) == -1){
		printf("ficheros_basico.c, 304: Error en escriure el bit: %i.\n", nbloque);
		return -1;
	}
	return nbloque;
}

int escribir_inodo(struct Inodo in, unsigned int ninodo){
	/* Funció que escriu un Inodo en la posició que és especificada
	 * per ninodo. */

	/* Llegim el SuperBloque. */
	struct SB a;
	if(bread(SBpos, &a) == -1){
		printf("ficheros_basico.c, 317: Error en llegir el SB.\n");
		return -1;
	}

	/* Calculam la Posició de l'Inodo: primer el Bloc on es troba i
	 * i llavors la posició dins el Bloc. */	
	int posBloque = ninodo / (BLOCKSIZE / (sizeof(struct Inodo))) + a.PrimerBloqueAI;
	int posInodo = ninodo % (BLOCKSIZE / (sizeof(struct Inodo)));

	if(posBloque == 0){
		printf("ficheros_basico.c, 327: Error en Bloc del SB.\n");
		return -1;
	}
	
	struct Inodo inodo[BLOCKSIZE / (sizeof(struct Inodo))];
	if(bread(posBloque, inodo) == -1){
		printf("ficheros_basico.c, 333: Error en llegir el Bloc d'Inodos.\n");
		return -1;
	}

	/* Assignam a l'Inodo del Bloc, l'Inodo que hem passat per
	 * paràmetre, i escrivim aquest en el bloc corresponent. */
	inodo[posInodo] = in;
	
	if(bwrite(posBloque, inodo) == -1){
		printf("ficheros_basico.c, 342: Error en escriure al Bloc d'Inodos.\n"); 
		return -1;
	}
	return ninodo;
}

struct Inodo leer_inodo(unsigned int ninodo){
	/* Funció que retorna l'Inodo apuntat per ninodo. */

	struct Inodo in;
	struct SB a;
	if(bread(SBpos, &a) == -1){
		printf("ficheros_basico.c, 354: Error en llegir el SB.\n");
		return in;
	}

	/* Hem de comprovar que l'Inodo al que apunta és possible de
	 * trobar. Sinó, botarà un error. */
	if(ninodo >= a.TotalInodos){
		printf("ficheros_basico.c, 361: No existeix aquest Inodo: %i.\n", ninodo);
		return in;
	}

	/* Calculam la posició en la que es troba i llegim el Bloc
	 * corresponent. Finalment, retornam l'Inodo calculat. */
	int posBloque = ninodo / (BLOCKSIZE / (sizeof(struct Inodo))) + a.PrimerBloqueAI;
	int posInodo = ninodo % (BLOCKSIZE / (sizeof(struct Inodo)));

	struct Inodo inodos[BLOCKSIZE / (sizeof(struct Inodo))];
	if(bread(posBloque, inodos) == -1){
		printf("ficheros_basico.c, 372: Error en llegir el Bloc d'Inodos.\n");
		return in;
	}

	return inodos[posInodo];
}

int reservar_inodo(unsigned char tipo, unsigned char permisos){
	/* Funció que reserva el primer Inodo Lliure i
	 * retorna el punter a aquest. */
	int i = 0, j = 0;

	/* Llegim el SuperBloque per tenir la darrera informació
	 * del sistema. */
	struct SB a;
	if(bread(SBpos, &a) == -1){
		printf("ficheros_basico.c, 388: Error en llegir el SB.\n");
		return -1;
	}

	/* Hem de comprovar que hi hagi Inodos Lliures, perquè si no
	 * no se'n podria reservar cap més. */
	if(a.InodosLibres < 1){
		printf("ficheros_basico.c, 395: No hi ha cap Inodo Lliure.\n");
		return -1;
	}
	
	/* Llegim el primer Inodo Lliure que marca el SuperBloque. */
	struct Inodo in;
	in = leer_inodo(a.PrimerInodoLibre);

	/* Inicialitzam el contingut de l'Inodo, i guardam el punter al
	 * següent Inodo Lliure dins la variable 'j'. */
	in.tamBytesLogicos = 0;
	in.EntradasDirectorio = 1;

	j = a.PrimerInodoLibre;
	a.PrimerInodoLibre = in.punterosDirectos[0];
		
	for(i = 0; i < 12; i++){
		in.punterosDirectos[i] = 0;
	}
	for(i = 0; i < 3; i++){
		in.punterosIndirectos[i] = 0;
	}
	in.atime = time(NULL);
	in.mtime = time(NULL);
	in.ctime = time(NULL);	
	in.tipo = tipo;
	in.permisos = permisos;

	/* Escrivim l'Inodo en la posició que toca.
	 * Després actualitzam la informació del SuperBloque. */
	if(escribir_inodo(in, j) == -1) {
		printf("ficheros_basico.c, 426: Error en escriure l'Inodo.\n");
		return -1;
	}
	
	a.InodosLibres--;
	if(bwrite(SBpos, &a) == -1){
		printf("ficheros_basico.c, 432: Error en escriure el SB.\n");
		return -1;
	}
	return j;
}

int traducir_recursivo(struct Inodo *in, int nivel, int pBSig, int blogico, unsigned int *bfisico, char reservar){
	/* Funció auxiliar recursiva per a la funció de 
	 * traducir_bloque_inodo mitjançant una recursió a través dels
	 * nivells de profunditat que necessitam. */
	int i = 0, blogicoAux; 
	unsigned int buf[BLOCKSIZE/(sizeof(unsigned int))];

	if(bread(pBSig, buf) == -1){
		printf("ficheros_basico.c, 446: Hi ha hagut error en llegir el bloc: %i.\n", pBSig);
		return -1;
	}
	int j = (int)pow((BLOCKSIZE/sizeof(unsigned int)), nivel);	
	blogicoAux = blogico / j;

	int r = 0, e = 0;
	if(reservar == 1) r = 1;
	if(buf[blogicoAux] != 0) e = 1;
	int matOpciones[2][2] = {-1, 0, 1, 0};

	switch(matOpciones[r][e]){
		case -1: 	nivel = 0; i = 0;
					break;
		case 1:		buf[blogicoAux] = reservar_bloque();
					in->BloquesDataOcupados++;
					if(bwrite(pBSig, buf) == -1){
						printf("ficheros_basico.c, 463: Hi ha hagut error en escriure un bloc.\n");
						return -1;
					}
		case 0: 	i = buf[blogicoAux];
	}
	
	if(nivel == 0){
		return i;
	}	
	else traducir_recursivo(in, nivel-1, i, blogico % j, bfisico, reservar);
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int blogico, unsigned int *bfisico, char reservar){
	/* Funció principal que donat un Inodo i un BloqueLógico d'aquest,
	 * es retorna el BlocFísic que es correspon.
	 * Es retorna el nombre per la
	 * variable 'bfisico', per complir amb el requeriment. */ 
	
	int nivel = -1;

	/* Llegim l'Inodo que ens fa falta per traduir. */
	struct Inodo in;
	in = leer_inodo(ninodo);
	
	/* Calcularem el nivell de profunditat del Bloc Lògic. */
	int inicio = 12;
	while(blogico >= inicio){
		nivel++;
		inicio += pow(((BLOCKSIZE / sizeof(unsigned int))), (nivel+1));
	}
	int i = 0;

	/* Implementació d'una Matriu d'Opcions:
	 * 		Fa que el procés de tria dels casos sigui més
	 * 		eficient i més ordenat que escriure sentències 'if'
	 * 		repetides.
	 *
	 * 		Es pot observar com, amb només dues sentències 'if' i
	 * 		una sentència 'switch', s'han direccionat tots els
	 * 		casos possibles.
	 *
	 * 		Descripció gràfica:
	 *						 Reservat
	 * 				 	 \	
	 * 				  	  \	Si__|___No_|
	 * 		Volem		Si_|_-1_|____1_|
	 * 		Reservar	No_|__0_|___-1_|	 */
	int r = 0, e = 0;
	if(reservar == 1) r = 1;
	switch(nivel){
		case -1: if(in.punterosDirectos[blogico] != 0) e = 1;
		default: if(in.punterosIndirectos[nivel] != 0) e = 1;
	}
	int matOpciones[2][2] = {-1, 0, 1, 0};
	switch(matOpciones[r][e]){
		case -1: 	break;
		case 1:		if((i = reservar_bloque()) == -1){
						printf("ficheros_basico.c, 521: Error! No s'ha pogut reservar el bloc.\n");
						return -1;
					}
					if(nivel == -1) in.punterosDirectos[blogico] = i;
					else in.punterosIndirectos[nivel] = i;
					in.BloquesDataOcupados++;
		case 0: 	if(nivel == -1) i = in.punterosDirectos[blogico];
					else i = in.punterosIndirectos[nivel];
	}
	if(nivel > -1){
		/* Si tenim nivells de traduccions a Punters Indirectes,
		 * s'entrarà a la traducció recursiva del Bloc Lògic. */
		int nAux;
		for(nAux = nivel; nAux > 0; nAux--){
			blogico -= (int) pow((BLOCKSIZE / sizeof(unsigned int)), nAux);
		}
		blogico -= puntDirectos;
		i = traducir_recursivo(&in, nivel, i, blogico, bfisico, reservar);
		/* En sortir d'aquesta recursió, dins la variable 'i'
		 * hi tindrem el punter al Bloc que s'ha traduït. */
	} 
	if(reservar == 1){
		/* Si finalment havíem de reservar el bloc, hem de guardar
		 * tots els canvis fets a l'Inodo. */
		if(escribir_inodo(in, ninodo) == -1){
			printf("ficheros_basico.c, 547: Error en escriure l'Inodo: %i.\n", ninodo);
			return -1;
		}
	}
	*bfisico = i;
	return 0;
}

int liberar_recursivo(struct Inodo *in, unsigned int blogico, int pBSig, int nivel){
	/* Funció que allibera el Bloc Físic associat al Bloc Lògic que
	 * es passa per paràmetre, i retorna el nombre de Blocs
	 * alliberats: Pot ser 1, o més, segons els blocs de Punters.
	 * Atenció: pBSig és l'acrònim de punterBlocSegüent. */

	int liberados = 0, blogicoAux;
	unsigned int buf[BLOCKSIZE/(sizeof(unsigned int))];

	if(bread(pBSig, buf) == -1){
		printf("ficheros_basico.c, 565: Error en llegir el bloc: %i.\n", pBSig);
		return -1;
	}

	int j = (int)pow((BLOCKSIZE/sizeof(unsigned int)), nivel);	
	blogicoAux = blogico / j;

	if(nivel == 0){
		liberar_bloque(buf[blogicoAux]);
		buf[blogicoAux] = 0;
		in->BloquesDataOcupados--;
		return liberados + 1;
	}
	else{
		liberar_recursivo(in, blogico % j, buf[blogicoAux], nivel - 1);

		unsigned int bufAux[BLOCKSIZE/(sizeof(unsigned int))];
		memset(bufAux, 0, BLOCKSIZE);
		
		if(memcmp(buf, bufAux, BLOCKSIZE) == 0){
			liberar_bloque(pBSig);
			in->BloquesDataOcupados--;
			liberados++;
		}
	}	
}
	 

int liberar_bloques_inodo(unsigned int ninodo, unsigned int blogico){
	/* Funció que allibera els Blocs Físics associats als Blocs Lògics
	 * de l'Inodo apuntat per 'ninodo' a partir del Bloc Lògic indicat
	 * per 'blogico' fins al final del Fitxer o Carpeta. */

	/* Llegim l'Inodo i calculam el final del Fitxer. */
	struct Inodo in;			
	in = leer_inodo(ninodo);

	int bFinalFichero = in.tamBytesLogicos / BLOCKSIZE;
	if((in.tamBytesLogicos % BLOCKSIZE) != 0) bFinalFichero++;
	
	/* Fem una iteració principal, alliberant tots els blocs que
	 * s'han especificat. */
	int i = 0, bLiberados = 0;
	for(i = (blogico/BLOCKSIZE); i < bFinalFichero; i++){
		if(i < puntDirectos && in.punterosDirectos[i] != 0){
			liberar_bloque(in.punterosDirectos[i]);
			in.punterosDirectos[i] = 0;
			bLiberados++;
		}
		else{
			/* Calculam el nivell del Bloc Lògic a alliberar. */
			int inicio = 12, nivel = -1;
			while(blogico >= inicio){
				nivel++;
				inicio += pow(((BLOCKSIZE / sizeof(unsigned int))), (nivel+1));
			}
			bLiberados += liberar_recursivo(&in, blogico - (puntDirectos - inicio) , in.punterosIndirectos[nivel], nivel);
		}
	}

	/* Actualitzam el Tamany en BytesLògics de l'Inodo amb els
	 * Blocs Alliberats després de tot el procés. */
	in.BloquesDataOcupados -= bLiberados;

	/* Finalment, escrivim l'Inodo en el lloc corresponent. */
	if(escribir_inodo(in, ninodo) == -1){
		printf("ficheros_basico.c, 631: Error en escriure l'Inodo: %i.\n", ninodo);
		return -1;
	}
	return bLiberados;
}

int liberar_inodo(unsigned int ninodo){
	/* Funció que allibera un Inodo per complet:
	 * 		Tant l'Inodo com tots els blocs d'aquest seran
	 * 		tornats a l'estat inicial, i enllaçat a la llista
	 * 		d'Inodos lliures. */

	/* Llegim el SuperBloc i l'Inodo implicat, per recuperar les
	 * dades necessàries. */	 
	struct SB a;
	if(bread(SBpos, &a) == -1){
		printf("ficheros_basico.c, 647: Error en llegir el SB.\n");
		return -1;
	}

	struct Inodo in;
	in = leer_inodo(ninodo);

	/* Hem de comprovar que l'Inodo no sigui lliure, perquè si no,
	 * no té sentit fer aquesta operació. */
	if(in.tipo == 'l'){
		printf("ficheros_basico.c, 657: Error en tipus d'Inodo.\n");
		return -1;
	}

	/* Canviam el tipus i els permissos (per poder borrar) de l'Inodo.*/
	in.tipo = 'l';
	in.permisos = 7;
	
	/* Alliberarem tots els blocs de l'Inodo. */
	liberar_bloques_inodo(ninodo, 0); 
	
	/* Intercanvis de Punters per posar el lliberat al principi de la
	 * llista de lliures.
	 * I actualitzam la informació del SuperBloque referent als
	 * Inodos Lliures. */
	in.punterosDirectos[0] = a.PrimerInodoLibre;
	a.PrimerInodoLibre = ninodo;
	
	a.InodosLibres++;

	if(escribir_inodo(in, ninodo) == -1){
		printf("ficheros_basico.c, 678: Error en escriure l'Inodo: %i.\n", ninodo);
		return -1;
	}
	if(bwrite(SBpos, &a) == -1){
		printf("ficheros_basico.c, 682: Error en escriure el SB.\n");	
		return -1;
	}
	return ninodo;
}
