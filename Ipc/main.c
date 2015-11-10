/** @file main.c
*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

#include <pthread.h>

#include "processore2.h"
#include "ram.h"
#include "utility.h"



int numCpu;
int ramDim;




int main (int argc, char *argv[]){

///altre variabili
	int cont = 0;           ///contatore che tiene conto delle cpu, serve per fare cont di fork di processori


 ///variabili pthread_create
	int statusThread;
	int statusRam;

/// converto i dati in ingresso forniti dall'utente
	numCpu = atoi(argv[1]);
	ramDim = atoi(argv[2]);

///una stampa a video per avvertire che i dati immessi dall'utente sono stati acquisiti
	if(write(1,"Ho acquisito il numero di processori e la memoria della ram\n",61) == -1) {
		write(2,"Write fallita\n",15);
		exit(1);
    }


    ///variabili thread
    pthread_t threads[numCpu + 1];       /// per tenere traccia degli identificatori delle thread
	int *buffers = (int*) malloc((sizeof (int)) * 3);

    ///inizializzo i semafori
    if(pthread_mutex_init(&mutexRam, NULL) != 0){
        write(2,"Init fallita\n",14);
		exit(1);
    }
    if(pthread_mutex_init(&mutexProc, NULL) != 0){
        write(2,"Init fallita\n",14);
		exit(1);
    }
    if(pthread_mutex_init(&mutexAllProc, NULL) != 0){
        write(2,"Init fallita\n",14);
		exit(1);
    }

    if(pthread_mutex_lock(&mutexRam) != 0){
        write(2,"Lock fallita\n",14);
		exit(1);
    }
///valore per far terminare la ram, se 0 non eseguirà mai l'if, se 1 lo esegue ed esce
    killRam = 0;

///alloco mememoria e inizializzo le variabili della struct che passarò come argomento quando farò la pthread_create
	struttura *parametri = (struttura *)malloc(sizeof(struttura));
    parametri->ramDim = ramDim;
	parametri->buffer = buffers;


///Creo la thread della ram
    if((statusRam = pthread_create(&threads[0], NULL, ram, parametri)) != 0){
      write(2,"Non sono riuscito a creare la thread della Ram\n",48);
      exit(1);
    }

    sleep(1);   ///aspetto un secondo affinchè la ram posso inizializzare tutti i dati e fermarsi al semaforo in attesa di essere sbloccata
///Creo i numCpu thread
    while(cont < numCpu) {
		struttura *p = (struttura *)malloc(sizeof(struttura));
        p->nProc = cont;
		p->buffer = buffers;
        if((statusThread = pthread_create(&threads[cont + 1], NULL, processore2, p)) != 0) {
            write(2,"Non sono riuscito a creare la thread\n",38);
            exit(1);
        }
        cont++;
    }


///il processo padre aspetta che tutti le thread figlie terminino per poter terminare pure lui
	for(cont = 0; cont < numCpu; cont++)
		if(pthread_join(threads[cont + 1], NULL) != 0){
        write(2,"Join fallita\n",14);
        exit(1);
    }

    killRam = 1;
    if(pthread_mutex_unlock(&mutexRam) != 0){
        write(2,"Unlock fallita\n",16);
        exit(1);
    }
    if(pthread_join(threads[0], NULL) != 0){
        write(2,"Join fallita\n",14);
        exit(1);
    }

    int i;
    ///distruggo i semafori
    if((i = pthread_mutex_destroy(&mutexRam)) != 0){
        //write(2,"Destroy fallita\n",17);
		exit(1);
    }

    if((i = pthread_mutex_destroy(&mutexProc)) != 0){
        write(2,"Destroy fallita\n",17);
		exit(1);
    }

    if((i = pthread_mutex_destroy(&mutexAllProc)) != 0){
        write(2,"Destroy fallita\n",17);
		exit(1);
    }

    ///libero la memoria
    free(parametri);

	exit(1);
}
