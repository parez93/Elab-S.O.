#!/bin/bash

#funzione che recupera l'operazione aritmetica corrispondente all'indice e ne stampa il risultato
function recuperoRisultato(){

#salvo dentro la variabile "espressione" la stringa corrispondente all'indice. Con "grep" cerco l'indice seguito da ":" (esempio "5:") e in pipe passo la stringa a "cut che con il comando -c4- tolgo i primi 3 caratteri (nel caso dell'esempio "5: ")
espressione=`grep $indice":" $1 | cut -c4-`

#stampo l'espressione e il risultato calcolato
echo "$espressione""="$((espressione))
}



echo "Inserisci l'indice di un'espressione aritmetica:"

#leggo da testiera l'indice dell'espressione
read indice

#calcolo il totale degli indici presenti nel file andando a vedere quante volte ":" si ripete, poi con "grep -c" ne ritorno il numero
totIndici=`grep -c : $1`;
if [ "$indice" -ge "1" ];then
	if [ "$indice" -le "$totIndici" ];then					#se l'indice dato dall'utente è <= al numero totale di indici, esegui il corpo del testo
		recuperoRisultato $1;													#chiamata funzione
		./menu.sh $1;
		else
			echo "Errore!Indice non valido.Il programma verrà terminato.";
			exit 0;
	fi
	else
		echo "Errore!Indice non valido.Il programma verrà terminato.";
		exit 0;
fi
