#!/bin/bash

echo "Inserisci un operatore, un numero, o un'espressione aritmetica:"
read pattern

#con "sort -r" ordino il file in senso inverso poi con "cut" tolgo il numero e i : (esempio 4:) e infine con "fgrep" (veloce e compatto rispetto a "grep" o "egrep") trovo tutte le stringhe contenenti il pattern
stringhe=`sort -r $1 | cut -f2 -d: | fgrep "$pattern"`

#con "sort -r" ordino il file in senso inverso di nuovo e poi con "fgrep" trovo tutte le stringhe che prima avevo trovato tranne quella con 4: ad esempio

if [ -z "$stringhe" ];then                     #il -z indiche che l'operando ha lunghezza zero
	echo "nessuna occorrenza trovata!"
  else
#con "sort -r" ordino il file in senso inverso di nuovo e poi con "fgrep" trovo tutte le stringhe che prima avevo trovato tranne quella con 4: ad esempio
		final=`sort -r $1 | fgrep "$stringhe"`
		echo "$final"
fi

./menu.sh $1
