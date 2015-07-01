#!/bin/bash

# $1 --> Disco virtual

# Intentarem llistar el dispositiu buit
echo 
echo MI_LS -- ./mi_ls $1 /-------------------------
echo
./mi_ls $1 /

# Crearem un directori dins l'Arrel
echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori1/-------
echo
./mi_mkdir $1 7 /directori1/

# Tornarem llistar el contingut del dispositiu i veurem que s'ha creat
echo
echo MI_LS -- ./mi_ls $1 /-------------------------
echo
./mi_ls $1 /

# Crearem una sèrie de directoris i fitxers fins arribar al
# límit d'un bloc.
echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori2/-------
echo
./mi_mkdir $1 7 /directori2/

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /fitxer3-----------
echo
./mi_mkdir $1 7 /fitxer3

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori4/-------
echo
./mi_mkdir $1 7 /directori4/

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /fitxer5-----------
echo
./mi_mkdir $1 7 /fitxer5

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori6/-------
echo
./mi_mkdir $1 7 /directori6/

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /fitxer7-----------
echo
./mi_mkdir $1 7 /fitxer7

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori8/-------
echo
./mi_mkdir $1 7 /directori8/

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori9/-------
echo
./mi_mkdir $1 7 /directori9/

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori10/-------
echo
./mi_mkdir $1 7 /directori10/

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori11/-------
echo
./mi_mkdir $1 7 /directori11/

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori12/-------
echo
./mi_mkdir $1 7 /directori12/

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori13/-------
echo
./mi_mkdir $1 7 /directori13/

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori14/-------
echo
./mi_mkdir $1 7 /directori14/

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori15/-------
echo
./mi_mkdir $1 7 /directori15/

echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori16/-------
echo
./mi_mkdir $1 7 /directori16/

# Llistarem el contingut del dispositiu i veurem què s'ha creat
echo
echo MI_LS -- ./mi_ls $1 /-------------------------
echo
./mi_ls $1 /

# Mostrarem les dades de l'Arrel del dispositiu, comprovant que
# encara només té ocupat un sol Blocs de Dades.
echo
echo MI_STAT -- ./mi_stat $1 /---------------------
echo
./mi_stat $1 /

# Crearem, per mitjà de mi_ln, un link nou a un fitxer nou.
echo
echo MI_LN -- ./mi_ln $1 /fitxer3 /fitxer17--------
echo
./mi_ln $1 /fitxer3 /fitxer17

# Tornarem a mostrar les dades de l'Arrel del dispositiu per veure
# que ara ocupa un Bloc de Dades més.
echo
echo MI_STAT -- ./mi_stat $1 /---------------------
echo
./mi_stat $1 /

# Llistarem el contingut del dispositiu i veurem què s'ha creat
echo
echo MI_LS -- ./mi_ls $1 /-------------------------
echo
./mi_ls $1 /

# Eliminarem un directori de l'Arrel del dispositiu.
echo
echo MI_RM -- ./mi_rm $1 /directori14/
echo
./mi_rm $1 /directori14/

# Llistarem el contingut del dispositiu i veurem què s'ha creat
echo
echo MI_LS -- ./mi_ls $1 /-------------------------
echo
./mi_ls $1 /

# Tornarem a mostrar les dades de l'Arrel del dispositiu per veure
# que ara ocupa un Bloc de Dades menys.
echo
echo MI_STAT -- ./mi_stat $1 /---------------------
echo
./mi_stat $1 /

# Eliminarem un directori de l'Arrel del dispositiu.
echo
echo MI_RM -- ./mi_rm $1 /fitxer7
echo
./mi_rm $1 /fitxer7

# Llistarem el contingut del dispositiu i veurem què s'ha creat
echo
echo MI_LS -- ./mi_ls $1 /-------------------------
echo
./mi_ls $1 /

# Canviarem els permisos d'un directori.
echo
echo MI_CHMOD -- ./mi_chmod $1 5 /directori12/-----
echo
./mi_chmod $1 5 /directori12/

# Mostrarem les dades del directori anterior per veure
# que s'han actualitzat els permisos.
echo
echo MI_STAT -- ./mi_stat $1 /directori12/---------
echo
./mi_stat $1 /directori12/
