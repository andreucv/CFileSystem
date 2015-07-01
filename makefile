all: mi_mkfs leer_SF mi_mkdir mi_chmod mi_ls mi_stat mi_cat mi_ln mi_rm mi_escribir simulacion verificacion

mi_mkfs: mi_mkfs.c ficheros_basico.c bloques.c semaforo_mutex_posix.c
	gcc -pthread -o mi_mkfs mi_mkfs.c bloques.c ficheros_basico.c ficheros.c directorios.c semaforo_mutex_posix.c -lm
	
leer_SF: leer_SF.c ficheros_basico.c bloques.c ficheros.c semaforo_mutex_posix.c
	gcc -pthread -o leer_SF leer_SF.c ficheros_basico.c bloques.c ficheros.c semaforo_mutex_posix.c -lm
	
mi_mkdir: mi_mkdir.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c
	gcc -pthread -o mi_mkdir mi_mkdir.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c -lm
	
mi_chmod: mi_chmod.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c
	gcc -pthread -o mi_chmod mi_chmod.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c -lm

mi_ls: mi_ls.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c
	gcc -pthread -o mi_ls mi_ls.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c -lm

mi_stat: mi_stat.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c
	gcc -pthread -o mi_stat mi_stat.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c -lm

mi_cat: mi_cat.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c
	gcc -pthread -o mi_cat mi_cat.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c -lm

mi_ln: mi_ln.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c
	gcc -pthread -o mi_ln mi_ln.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c -lm
	
mi_rm: mi_rm.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c
	gcc -pthread -o mi_rm mi_rm.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c -lm
	
simulacion: simulacion.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c
	gcc -pthread -o simulacion simulacion.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c -lm

mi_escribir: mi_escribir.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c
	gcc -pthread -o mi_escribir mi_escribir.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c -lm

verificacion: verificacion.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c
	gcc -pthread -o verificacion verificacion.c ficheros_basico.c bloques.c ficheros.c directorios.c semaforo_mutex_posix.c -lm
	
clean : 
	rm -f *.o mi_mkfs leer_SF mi_ls mi_stat mi_cat mi_ln mi_rm mi_chmod simulacion disco.imagen mi_mkdir 
