# CFileSystem
File System in C Language

It includes the functions to create, list, write, link and remove files and others. </br>
Add permissions and remove them is also developed.

The implementation of it was in phases, from core to latest functions:

CORE:</br>
bloques</br>
ficheros_basico</br>
ficheros</br>
directorios</br>
semaforo_mutex_posix</br>

FUNCTIONS:</br>
mi_mkfs -> Creates a memory device of 1024 Bytes blocks needed </br>
mi_mkdir -> Creates a directory</br>
mi_stat -> List the permissions and metadata of files and directories</br>
mi_chmod -> Change the permissions of files and directories</br>
mi_cat -> Show the content of a file</br>
mi_escribir -> Writes the specified characters in the specified position in Bytes of the file</br>
mi_leerSF -> Show the metadata of SuperBlock (the info of the memory device) </br>
mi_ls -> List the files and directories in a path of device</br>
mi_ln -> Links the directory to another directory</br>
mi_rm -> Remove the file or the directory in the device</br>


