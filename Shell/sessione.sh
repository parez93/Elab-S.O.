#!/bin/bash

#funzione che verrà richiamata nell'if per impostare la sessione
function impostaSessione(){
echo "Inserisci il nome della sessione corrente:"

#leggo da tastiera
read sessione

#verifico se nella directory corrente esiste un file con il nome letto da tastiera prima  e nel caso ci sia errore perchè non c'è redirigo l'output con 2>&1 sul file errore.txt che se non esiste viene creato
find $sessione".log"  > errore.txt 2>&1;

#verifico se esiste ( -e ./$sess.... )un file il cui nome è letto da tastiera con il .log aggiunto altrimenti lo creo vuoto
if [ -e ./$sessione".log" ];then
	echo "Sessione impostata";
	
else
	echo "file creato";
	touch $sessione".log";
fi

#l'esecuzione dello script torna al menù interattivo iniziale ritornando come parametro la sessione impostata
./menu.sh $sessione".log"
}

#funzione che verrà richiamata nell'if per rimuovere una sessione
function rimuoviSessione(){
echo "Inserisci il nome della sessione che vuoi rimuovere:"

#leggo da tastiera
read sessioneRimossa

#verifico se nella directory corrente esiste un file con il nome letto da tastiera prima e nel caso ci sia errore perchè non c'è redirigo l'output con 2>&1 sul file errore.txt che se non esiste viene creato
find $sessione".log"  > errore.txt 2>&1;

if [ -e ./$sessioneRimossa".log" ];then
	rm $sessioneRimossa".log";
	./menu.sh;
else
	echo "il file non esiste! IL programma verrà terminato";
	exit 0;	
fi
}



echo "Elenco file sessione"
#trovo tutti i file la cui estensione è .log che sono nella directory corrente ( find . ) il cui pattern è *.log
#elimino l'estensione .log selezionando per campi (es. /sessione.log --> /sessione)
#tolgo lo / selezionando per caratteri
#ordino in ordine alfabetico
find . -name "*.log" | cut -d . -f2 | cut -c2- | sort -d

#se non trovo file estensione stampo a video che non ne ho trovati
if [ "`find . -name "*.log" | cut -d . -f2 | cut -c2- | sort -d`" = "" ];then
echo "Nessun file trovato";
fi

#stampa il menù
echo "a) Imposta sessione corrente"
echo "b) Rimuovi sessione"

#leggo da tastiera la scelta e la salvo nella variabile "scelta"
read scelta

if [ "$scelta" = "a" ];then   #andava anche bene:       if test "$scelta"="a";then
impostaSessione;							#chiamata a funzione
fi

if test "$scelta" = "b";then
rimuoviSessione;							#chiamata a funzione
fi




