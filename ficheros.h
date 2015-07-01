#include "ficheros_basico.h"

struct STAT{	
	unsigned int EntradasDirectorio;
	/* Quantitat d'enllaços d'entrades a Carpetes */
	unsigned int tamBytesLogicos;
	/* Tamany de bytes lògics */
	unsigned int BloquesDataOcupados;
	/* Nombre de Blocs de Dades Ocupats per l'Inodo */
	time_t ultimoAccesoDatos;
	/* Darrer accés a Dades de l'Inodo (atime) */
	time_t ultimaModificacionDatos;
	/* Darrera modificació de Dades de l'Inodo (mtime) */
	time_t ultimaModificacionINODO;
	/* Darrera modificació de l'Inodo (ctime) */
	unsigned char tipo;
	/* 3 opcions: lliure (l), directori (d) o fitxer (f) */
	unsigned char permisos;
	/* Permisos possibles: Escriptura, Lectura i/o Execució*/
	unsigned char padding[128 - (3*sizeof(unsigned int) +
		3*sizeof(time_t) + 2*sizeof(unsigned char))];
};

int mi_write_f(unsigned int ninodo, const void *buffer, unsigned int offset, unsigned int nbytes);
int mi_read_f(unsigned int ninodo, void *buffer, unsigned int offset, unsigned int nbytes);
int mi_chmod_f(unsigned int ninodo, unsigned char permisos);
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat);
