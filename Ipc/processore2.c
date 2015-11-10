/** @file processore2.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>

#include <pthread.h>


#include "utility.h"

/** @name Processore */

/** @brief funzione che esegue le funzionalità dei processori
 * @param arg Contiene una struct contente: il numero dei processori, la dimensione della memoria ram (in queste thread non serve)
  e un buffer per condividere con la thread che simula la ram le varie istruzioni
 * @return Void
*/
void *processore2 (void *arg){

///recupero gli argomenti che passo alla thread
struttura *p = (struttura *)arg;

 ///variabile ausiliaria per la memoria da condividere con la ram
	char buf[50];


///altre variabili
	int readEOF;
	ssize_t fdOpen;
	char bufferRead;


///stringa contenente il nome del file da aprire
	char nomeFile[10] = "cpu_";
///stringa ausiliaria che serve per creare il nome del file da aprire
	char temp[2];
///converto in intero il numero del processore
	int nProc = p->nProc;
	nProc++;
///preparo l'occorrent per la stampa a video di quale processore sono
	char o[100];
    char stampaVideo[100];
	sprintf(o,"%d",nProc);
	strcat(stampaVideo,"Sono il processore #");
	strcat(stampaVideo,o);
	strcat(stampaVideo,"\n");

///creo il nome del file nel formato: cpu_<id>.txt
	sprintf(temp,"%d",nProc);
	strcat(nomeFile,temp);
	strcat(nomeFile,".txt");


///apro il file in lettura e scrittura dando tutti permessi
	if((fdOpen = open(nomeFile,O_RDWR,0777)) == -1) {
		write(2,"Non sono riuscito ad aprire il file\n",36);
        exit(1);
	}
    int cont2;
    int cont;
    char tmp[sizeof(buf)];  ///array temporaneo che uso per contenere i valori della read,write,sleep prima di essere copiato
	while(1){
        cont2 = 0;
        cont = 0;
        do{
            if((readEOF = read(fdOpen, &bufferRead, 1)) == -1) {
                write(2,"Errore nella read\n", 19);
                exit(1);
		}
		if(readEOF == 0)		///se sono arrivato a leggere tutto il file (cioè leggo EOF) esco
			return 0;
		buf[cont] = bufferRead;
		cont++;
        } while (bufferRead != '\n');
        buf[cont] = '\0';
        cont = 1;

///inizio sezione critica
/// P tra tutte le thread in modo che in sezione critica entri solo uno
        if(pthread_mutex_lock(&mutexAllProc) != 0){
            write(2,"Lock fallita\n",14);
            exit(1);
        }

/// due stampe a video
        if((write(1,stampaVideo,23)) == -1){
            write(2,"Write fallita\n",15);
            exit(1);
        }

        if((write(1,"Entro in sezione critica\n",26)) == -1){
            write(2,"Write fallita\n",15);
            exit(1);
        }

/**salvo l'istruzione da eseguire
-buffer[1] ho il primo numero
-buffer[2] ho il secondo numero
-buffer[0] ho la lettera convertita ad intero in base alle tabella ascii
*/
        while(buf[++cont] != '\0'){
            temp[cont2++] = buf[cont];
            if(buf[cont] == ' '){
                temp[--cont2] = '\0';
                p->buffer[1] = atoi(temp);
                cont2 = 0;
            }
        }
        p->buffer[2] = atoi(temp);
        p->buffer[0] = buf[0];
///fine sezione critica
///eseguo l'istruzione nel caso sia una sleep
        if(p->buffer[0] == 83) {
            sprintf(o,"%c %d\n",p->buffer[0],p->buffer[2]);
            if((write(1,o,5)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
            }
            sleep(p->buffer[2]);
        }

///eseguo l'istruzione nel caso sia una write
        if(p->buffer[0] == 87) {
///faccio un po di stampa a video
            sprintf(o,"%c %d %d\n",p->buffer[0],p->buffer[1],p->buffer[2]);
            if((write(1,o,7)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
            }

    ///altre stampe a video
            sprintf(o,"Processore #%d: Il numero %d sarà scritto in memoria nella posizione %d\n",nProc,p->buffer[1],p->buffer[2]);
            if((write(1,o,71)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
            }

            if(pthread_mutex_unlock(&mutexRam) != 0){
                write(2,"Unlock fallita\n",16);
                exit(1);
            }
            if(pthread_mutex_lock(&mutexProc) != 0){
                write(2,"Lock fallita\n",14);
                exit(1);
            }
        }

///eseguo l'istruzione nel caso sia una read
        if(p->buffer[0] == 82) {
///faccio un po di stampa a video
            sprintf(o,"%c %d\n",p->buffer[0],p->buffer[2]);
            if((write(1,o,5)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
            }

            if(pthread_mutex_unlock(&mutexRam) != 0){
                write(2,"Unlock fallita\n",16);
                exit(1);
            }
            if(pthread_mutex_lock(&mutexProc) != 0){
                write(2,"Lock fallita\n",14);
                exit(1);
            }

///altre stampe a video
            sprintf(o,"Processore #%d: Il numero che si voleva leggere era:%d\n",nProc,p->buffer[1]);
            if((write(1,o,54)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
            }
        }

        if(pthread_mutex_unlock(&mutexAllProc) != 0){
            write(2,"Lock fallita\n",14);
            exit(1);
        }

    }

    pthread_exit(NULL);

}



