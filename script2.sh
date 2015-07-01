#!/bin/bash

# $1 --> Dispositiu Virtual

# Intentarem llistar el dispositiu buit
echo 
echo MI_LS -- ./mi_ls $1 /-------------------------
echo
./mi_ls $1 /

# Crearem un directori dins l'Arrel
echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /directori/-------
echo
./mi_mkdir $1 7 /directori/

# Tornarem llistar el contingut del dispositiu i veurem que s'ha creat
echo
echo MI_LS -- ./mi_ls $1 /-------------------------
echo
./mi_ls $1 /

# Llistarem el contingut del directori creat, que serà buit.
echo
echo MI_LS -- ./mi_ls $1 /directori/---------------
echo
./mi_ls $1 /directori/

# Crearem un fitxer dins l'Arrel
echo
echo MI_MKDIR -- ./mi_mkdir $1 7 /fitxer-----------
echo
./mi_mkdir $1 7 /fitxer

# Tornarem a llistar el contingut del dipositiu i veurem que s'ha creat el fitxer.
echo
echo MI_LS -- ./mi_ls $1 /-------------------------
echo
./mi_ls $1 /

# Escriurem un contingut al fitxer creat que ocupi més d'un bloc.
echo
echo MI_ESCRIBIR -- ./mi_escribir $1 25120 /fitxer-
echo
./mi_escribir $1 25120 /fitxer "Aixo es una prova d'escriptura de text que
ocupa mes o menys 500 caracters, i per tant, s'escriu en blocs logics diferents,
perque 25*1024 = 25600. Si es vol escriure mes text, es pot escriure aqui.
Si se'n vol menys, s'haura de borrar. Es pot provar d'escriure menys text,
pero es un exemple poc il·lustratiu. S'escriu sense accents perque es
possible que doni errors amb la codificacio del fitxer de text, perque no
s'obre amb naturalitat amb 'gedit' i per aixo emprarem 'Okteta'. Creat
dia 04-08-2013."

# Mostrarem el tamany del fitxer anterior.
echo
echo MI_LS -- ./mi_ls $1 /-------------------------
echo
./mi_ls $1 /

# Llegirem el contingut del fitxer, i es direccionarà la sortida a un
# fitxer. Ara es pot comprovar que són igual de grans.
# Es pot obrir amb l'editor 'Okteta', perquè amb 'gedit' no respon. 
echo
echo MI_CAT -- ./mi_cat $1 /fitxer \> sortidaMiCat--
echo
./mi_cat $1 /fitxer > sortidaMiCat
echo
