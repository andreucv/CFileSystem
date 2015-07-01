#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buffer, unsigned int offset, unsigned int nbytes){
	/* Funció que escriu el contingut del buffer que es passi per
	 * paràmetre a la posició del fitxer (offset) indicada. */

	/* Totes les variables que haurem d'emprar són aquestes. 
	 * Declaram un buffer, on anirem llegint i escrivint les
	 * dades que pertanyin al bloc, i llegim l'Inodo en el que hem
	 * d'escriure.*/
	struct Inodo in;
	unsigned int BloqueInicial = offset / BLOCKSIZE;
	unsigned int BloqueFinal = (offset + nbytes - 1) / BLOCKSIZE;
	unsigned int ByteInicial = offset % BLOCKSIZE;
	unsigned int ByteFinal = (offset + nbytes - 1) % BLOCKSIZE;
	unsigned int BloqueFisico = 0;
	int BytesEscritos = 0;
	unsigned char buf[BLOCKSIZE];
	
	in = leer_inodo(ninodo);
	
	/* Comprovam si tenim els permisos necessaris per escriure. */
	if((in.permisos & 2) != 2){
		printf("ficheros.c, 24: No es tenen permisos d'escriptura a l'Inodo %i.\n", ninodo);
		return -1;
	}

	/* Cridam a la funció per trobar el Bloc Físic corresponent. */
	if(traducir_bloque_inodo(ninodo, BloqueInicial, &BloqueFisico, 1) == -1){
		printf("ficheros.c, 30: Error en Traducir_Bloque_Inodo.\n");
		return -1;
	}

	/* Llegim el Bloc resultat de l'anterior operació. */
	if(bread(BloqueFisico, buf) == -1){
		printf("ficheros.c, 36: Error en llegir el bloc: %i.\n", BloqueFisico);
		return -1;
	}
	
	/* Comprovam que els Blocs Inici i Final, on hem d'escriure, siguin
	 * el mateix. Si és així, només farà falta cridar a 'bwrite' una
	 * vegada. Si no és així, l'haurem de cridar tantes vegades com
	 * faci falta. */ 
	if(BloqueInicial == BloqueFinal){
		memcpy(buf + ByteInicial, buffer, nbytes);
		
		if(bwrite(BloqueFisico, buf) == -1){
			printf("ficheros.c, 48: Error en escriure el bloc: %i.\n", BloqueFisico);
			return -1;
		}
		
		BytesEscritos = nbytes;
	}
	else{
		/* En cas que el Bloc Inicial i el Final no siguin el mateix,
		 * primer haurem d'escriure el que ens queda al primer bloc.*/
		memcpy(buf + ByteInicial, buffer, BLOCKSIZE - ByteInicial);

		/* I escrivim aquest primer bloc.*/
		if(bwrite(BloqueFisico, buf) == -1){
			printf("ficheros.c, 61: Error en escriure el bloc: %i.\n", BloqueFisico);
			return -1;
		}
		
		memset(buf, 0, BLOCKSIZE);

		/* Augmentam degudament els Bytes que hem escrit. */
		BytesEscritos += (BLOCKSIZE - ByteInicial);

		/* I feim l'iteració entre els Blocs que falten per escriure. */
		int j = 0;
		for(j = BloqueInicial + 1; j < BloqueFinal; j++){
			if(traducir_bloque_inodo(ninodo, j, &BloqueFisico, 1) == -1){
				printf("ficheros.c, 74: Error en reservar un bloc.\n");
				return -1;
			}
			
			memcpy(buf, buffer + BytesEscritos, BLOCKSIZE);
			
			if(bwrite(BloqueFisico, buf) == -1){
				printf("ficheros.c, 81: Error en escriure el bloc: %i.\n", BloqueFisico);
				return -1;
			}
			BytesEscritos += BLOCKSIZE;
		}
		/* Finalment, repetim el procés per al darrer bloc de tots. */
		if(traducir_bloque_inodo(ninodo, BloqueFinal, &BloqueFisico, 1) == -1){
			printf("ficheros.c, 88: Error en reservar un bloc (Traduir).\n");
			return -1;
		}
		
		if(bread(BloqueFisico, buf) == -1){
			printf("ficheros.c, 93: Error en llegir un bloc.\n");
			return -1;
		}
		
		memcpy(buf, buffer + BytesEscritos, ByteFinal + 1);
		
		if(bwrite(BloqueFisico, buf) == -1){
			printf("ficheros.c, 100: Error en escriure el bloc: %i.\n", BloqueFisico);
			return -1;
		}
		
		BytesEscritos += (ByteFinal + 1);
	}

	/* Finalment, llegim l'Inodo per guardar la nova informació. */
	in = leer_inodo(ninodo);
	
	if(offset + nbytes > in.tamBytesLogicos) in.tamBytesLogicos = offset + nbytes;
	in.ctime = time(NULL);
	in.mtime = time(NULL);
	
	if(escribir_inodo(in, ninodo) == -1){
		printf("ficheros.c, 115: Error en escriure l'Inodo: %i.\n", ninodo);
		return -1;
	}
	
	return BytesEscritos;
}

int mi_read_f(unsigned int ninodo, void *buffer, unsigned int offset, unsigned int nbytes){
	/* Funció que retorna en el buffer del paràmetre tot el contingut
	 * dins el fitxer a partir de l'offset fins a offset + nbytes. */

	/* Les variables i el buffer que ens farà falta per dur a
	 * terme l'operació. */
	struct Inodo in;
	unsigned int BloqueInicial = offset / BLOCKSIZE;
	unsigned int ByteInicial = offset % BLOCKSIZE;
	unsigned int BloqueFinal = (offset + nbytes - 1) / BLOCKSIZE;
	unsigned int ByteFinal = (offset + nbytes - 1) % BLOCKSIZE;
	unsigned int bFisico = 0;
	unsigned int BytesLeidos = 0;
	unsigned char buf[BLOCKSIZE];

	/* Llegim l'Inodo implicat, i miram si tenim permisos de lectura. */
	in = leer_inodo(ninodo);	
	if((in.permisos & 4) != 4){
		printf("ficheros.c, 140: No es tenen permisos de lectura a l'Inodo %i.\n", ninodo);
		return -1;
	}
	
	if((offset + nbytes) >= in.tamBytesLogicos) nbytes = in.tamBytesLogicos - offset;	
	if(nbytes <= 0 || offset >= in.tamBytesLogicos){
		nbytes = 0;
	}

	/* Cercarem el Bloc Físic que es correspon al Bloc Lògic Inicial, i el llegirem. */
	if((traducir_bloque_inodo(ninodo, BloqueInicial, &bFisico, 0) != -1)){
		if(bFisico != 0){
			if(bread(bFisico, buf) == -1){
				printf("ficheros.c, 153: Error en llegir el Bloc: %i.\n", bFisico);
				return -1;
			}
		}
		else memset(buf, 0, BLOCKSIZE);	
	}

	/* Si els blocs són els mateixos, copiarem del Bloc llegit.
	 * Si no, haurem de fer una iteració. */
	if(BloqueInicial == BloqueFinal){
		memcpy(buffer, buf + ByteInicial, nbytes);		
		BytesLeidos = nbytes;
	}
	else{
		memcpy(buffer, buf + ByteInicial, BLOCKSIZE - ByteInicial);
		BytesLeidos = BLOCKSIZE - ByteInicial;

		int j = 0;
		for(j = BloqueInicial + 1; j < BloqueFinal; j++){
			if(traducir_bloque_inodo(ninodo, j, &bFisico, 0) != -1){
				if(bFisico != 0){
					if(bread(bFisico, buf) == -1){
						printf("ficheros.c, 175: Error en llegir el Bloc: %i.\n", bFisico);
						return -1;
					}
				}
				else memset(buf, 0, BLOCKSIZE);
			}			
			memcpy(buffer + BytesLeidos, buf, BLOCKSIZE);	
			memset(buf, 0, BLOCKSIZE);		
			BytesLeidos += BLOCKSIZE;
		}
		
		if(traducir_bloque_inodo(ninodo, BloqueFinal, &bFisico, 0) != -1){
			if(bFisico != 0){
				if(bread(bFisico, buf) == -1){
					printf("ficheros.c, 189: Error en llegir el Bloc: %i.\n", bFisico);
					return -1;
				}
			}
			else memset(buf, 0, BLOCKSIZE);
		}
		
		memcpy(buffer + BytesLeidos, buf, ByteFinal + 1);		
		BytesLeidos += (ByteFinal + 1);
	}

	/* Actualitzam les dades referents al temps.*/
	in.atime = time(NULL);

	/* Finalment escriurem l'Inodo per actualitzar les dades. */
	if(escribir_inodo(in, ninodo) == -1){
		printf("ficheros.c, 205: Error en escriure l'Inodo: %i.\n", ninodo);
		return -1;
	}
	return BytesLeidos;
}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
	/* Funció que canvia els permisos d'un Inodo segons els que s'han
	 * especificat per paràmetre.
	 *
	 * Es llegeix l'Inodo, es canvien els permisos, s'actualitza el
	 * temps de modificació i s'escriu. */
	struct Inodo in;
	in = leer_inodo(ninodo);
	
	in.permisos = permisos;
	
	in.ctime = time(NULL);
	
	escribir_inodo(in, ninodo);
}

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){
	/* Funció que crida a liberar_bloques_inodo amb un valor diferent
	 * de 0.
	 * Es miren els permisos de l'Inodo i es lliberen els blocs,
	 * s'actualitzen els temps i el tamany en bytes lògics. */ 

	struct Inodo in;
	in = leer_inodo(ninodo);
	if((in.permisos & 2) != 2){
		printf("ficheros.c, 236: No es tenen permisos d'escriptura.\n");
		return -1;
	}

	liberar_bloques_inodo(ninodo, nbytes);
	in = leer_inodo(ninodo);
	
	in.mtime = time(NULL);
	in.ctime = time(NULL);
	
	in.tamBytesLogicos = nbytes;

	if(escribir_inodo(in, ninodo) == -1){
		printf("ficheros.c, 249: Error en escriure l'Inodo: %i.\n", ninodo);
		return -1;
	}
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){
	/* Funció que retorna els 'STAT' d'un Inodo. */

	struct Inodo in;
	in = leer_inodo(ninodo);
	
	p_stat->EntradasDirectorio = in.EntradasDirectorio;
	p_stat->tamBytesLogicos = in.tamBytesLogicos;
	p_stat->BloquesDataOcupados = in.BloquesDataOcupados;
	p_stat->ultimoAccesoDatos = in.atime;
	p_stat->ultimaModificacionDatos = in.mtime;
	p_stat->ultimaModificacionINODO = in.ctime;
	p_stat->tipo = in.tipo;
	p_stat->permisos = in.permisos;

	return 0;
}
