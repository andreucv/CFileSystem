#include "bloques.h"

#define SBpos 0 	
#define puntDirectos 12
#define puntIndirectos 3

struct SB{
	unsigned int PrimerBloqueMB;
	unsigned int UltimoBloqueMB;
	unsigned int PrimerBloqueAI;
	unsigned int UltimoBloqueAI;
	unsigned int PrimerBloqueData;
	unsigned int UltimoBloqueData;
	/* Posicions dels Primers i Darrers Blocs de parts del Dispositiu. */
	unsigned int InodoDirectorioRaiz;
	/* Punter a l'Inodo Arrel. */
	unsigned int PrimerInodoLibre;
	/* Punter al Primer Inodo Lliure.
	 * Es renovarà cada vegada que es Reservi o s'Alliberi un Inodo. */
	unsigned int BloquesLibres;
	/* Nombre de Blocs de Dades Lliures dins el dispositiu. */
	unsigned int InodosLibres;
	/* Nombre d'Inodos Lliures a l'AI. */
	unsigned int TotalBloques;
	/* Nombre de blocs totals del dispositiu. */
	unsigned int TotalInodos;
	/* Nombre total d'Inodos que hi ha a AI. */
	char padding[BLOCKSIZE-12*sizeof(unsigned int)]; 
	/* Padding: Un array de caràcters per a que el 'SuperBloque'
	 * ocupi exactament 1024 bytes. */
};

struct Inodo{	
	unsigned int EntradasDirectorio;
	/* Quantitat d'enllaços d'entrades. */
	unsigned int tamBytesLogicos;
	/* Tamany del fitxer o directori en bytes lògics. */
	unsigned int BloquesDataOcupados;
	/* Nombre de Blocs de Dades Ocupats per l'Inodo */
	unsigned int punterosDirectos[12];
	/* Són els 12 Punters a Blocs de Dades Directes */
	unsigned int punterosIndirectos[3];
	/* Són 3 Punters a Blocs Indirectes.
	 * 3 nivells:
	 * 		Nivell 0: Un Bloc de Punters.
	 * 		Nivell 1: Un Bloc de Blocs de Punters.
	 * 		Nivell 2: Un Bloc de Blocs de Blocs de Punters. */
	time_t atime;
	/* Darrer accés a Dades de l'Inodo (atime) */
	time_t mtime;
	/* Darrera modificació de Dades de l'Inodo (mtime) */
	time_t ctime;
	/* Darrera modificació de l'Inodo (ctime) */
	unsigned char tipo;
	/* 3 opcions: lliure (l), directori (d) o fitxer (f) */
	unsigned char permisos;
	/* Permisos possibles: Escriptura, Lectura i/o Execució*/
	unsigned char padding[128 - (10*sizeof(unsigned int) + 10*sizeof(time_t) + 
	  8*sizeof(unsigned char))];
	 /* El padding s'ha muntat d'aquesta manera, per qüestions internes
	  * del llenguatge.*/
};

int tamMB(unsigned int nbloques);
int tamAI(unsigned int TotalINODOS);
int initSB(unsigned int nbloques, unsigned int TotalINODOS);
int initMB(unsigned int nbloques);
int initAI();
int escribir_bit(unsigned int nbloque, unsigned int bit);
unsigned char leer_bit(unsigned int nbloque);
struct Inodo leer_inodo(unsigned int ninodo);
int reservar_bloque();
int traducir_bloque_inodo(unsigned int ninodo, unsigned int blogico, unsigned int *bfisico, char reservar);
int liberar_inodo(unsigned int ninodo);
int liberar_bloques_inodo(unsigned int ninodo, unsigned int blogico);
int liberar_recursivo(struct Inodo *in, unsigned int blogico, int pBSig, int nivel);
int traducir_recursivo(struct Inodo *in, int nivel, int pBSig, int blogico, unsigned int *bfisico, char reservar);
