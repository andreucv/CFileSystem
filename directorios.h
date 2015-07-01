#include "ficheros.h"

struct Entrada{
	char nombre[60];
	unsigned int ninodo;
};

int extraer_camino(const char *camino, char *inicial, char *final);
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char modo);
int mi_creat(char *camino, unsigned char permisos);
int mi_chmod(char *camino, unsigned char permisos);
int mi_dir(char *camino, char *buffer);
int mi_link(char *caminoOrigen, char *caminoDest);
int mi_read(char *camino, void *buf, unsigned int offset, unsigned int nbytes);
int mi_stat(char *camino, struct STAT *p_stat);
int mi_unlink(char *camino);
int mi_write(char *camino, const void *buf, unsigned int offset, unsigned int nbytes);
