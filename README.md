# CFileSystem
File System in C Language

It includes the functions to create, list, write, link and remove files.
Add permissions and remove them is also developed.

The implementation of it was in phases, from core to latest functions:

CORE:
bloques
ficheros_basico
ficheros
directorios
semaforo_mutex_posix

FUNCTIONS:
mi_mkfs -> Creates a memory device of 1024 Bytes blocks needed
mi_mkdir -> Creates a directory
mi_stat -> List the permissions and metadata of files and directories
mi_chmod -> Change the permissions of files and directories
mi_cat -> Show the content of a file
mi_escribir -> Writes the specified characters in the specified position in Bytes of the file
mi_leerSF -> Show the metadata of SuperBlock (the info of the memory device)
mi_ls -> List the files and directories in a path of device
mi_ln -> Links the directory to another directory
mi_rm -> Remove the file or the directory in the device


