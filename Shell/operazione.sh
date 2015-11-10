#!/bin/bash


#controllo se è stata impostata una sessione
if [ "$1" = "" ];then
	echo "Errore! Non hai impostato nessuna sessione. Il programma verrà terminato.";
	exit;
fi

echo "Inserisci un'espressione aritmetica da calcolare:"
read espressione

#cerco nel file (grep) quante volte si ripete il simbolo ":" grazie a -c e salvo tutto dentro la variabile "cont"
cont=`grep -c : $1`

#scrittura su file
echo $((cont+1))": "$espressione >> $1

#stampo a video il risultato dell'espressione aritmetica
echo $((espressione))

./menu.sh $1
