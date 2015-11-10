/** @file ram.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

#include "ram.h"
#include "utility.h"

/** @name Ram */
/*@{ */

/** @brief funzione che esegue le funzionalità della ram
 * @param arg Contiene una struct contente: il numero dei processori(in questa thread non serve), la dimensione della memoria ram
  e un buffer per condividere con le thread che simulano i processori le varie istruzioni
 * @return Void
*/
void *ram (void *arg){

///recupero gli argomenti che passo alla thread
struttura *p = (struttura *)arg;

///altre variabili
	int cont;
	char temp[100];
	int ramDim = p->ramDim;
	int memoria[ramDim];


	sprintf(temp,"Ram: Sono dentro la Ram di dimensione %d\n",ramDim);
    if((write(1,temp,41)) == -1){
        write(2,"Write fallita\n",15);
        exit(1);
    }

///inizializzo l'array della memoria a 0
    for(cont=0; cont<ramDim; cont++)
        memoria[cont]=0;

///entro in un loop;
	while(1) {
///semaforo perchè sto per entrare in sezione critica
        if(pthread_mutex_lock(&mutexRam) != 0){
            write(2,"Lock fallita\n",14);
            exit(1);
        }

        if(killRam == 1) {
///faccio l'unlock del mutex perchè altrimenti poi non riesco a fare la pthread_mutex_destroy perchè da errore #16 = ebusy = semaforo bloccato
            if(pthread_mutex_unlock(&mutexRam) != 0){
                write(2,"Unock fallita\n",16);
                exit(1);
            }
            pthread_exit(NULL);
        }

		if(p->buffer[0] == 87){     ///se è una Write
			memoria[p->buffer[2]] = p->buffer[1];
			sprintf(temp,"Ram: ho scritto in memoria il numero %d alla posizione %d\n", p->buffer[1], p->buffer[2]);
			if((write(1,temp,57)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
			}
        }
		if(p->buffer[0] == 82) {	///se è una Read
			p->buffer[1] = memoria[p->buffer[2]];
			if((write(1,"Ram: ho letto, ora il processore dovrebbe darmi l'ok\n",54)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
			}
        }
///semaforo per sbloccare la thread chiamante perchè sono fuori dalla sezione critica
        if(pthread_mutex_unlock(&mutexProc) != 0){
            write(2,"Unock fallita\n",16);
            exit(1);
        }
	}
}
