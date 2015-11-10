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

/** @name Operazioni sui semafori */
/*@{ */

/** @brief funzione che esegue la P dei semafori
 * @param sem_number Il numero del semaforo
 * @param semid ID del gruppo dei semafori
 * @return Void
*/
void sem_wait(int sem_number, int semid) {
	struct sembuf wait_b;
	wait_b.sem_num = sem_number;	            /// setto il numero del semaforo
	wait_b.sem_op = -1;						    /// la 'wait' decrementa il valore del semaforo
	wait_b.sem_flg = 0;						    /// nessuna flag
	if (semop(semid, &wait_b, 1) == -1) {		/// eseguo la wait
		write(2,"WAIT abortita. TERMINO.\n",25);
		exit(2);
	}
}

/** @brief funzione che esegue la V dei semafori
 * @param sem_number Il numero del semaforo
 * @param semid ID del gruppo dei semafori
 * @return Void
*/
void sem_signal(int sem_number,int semid) {
	struct sembuf signal_b;
	signal_b.sem_num = sem_number;            	/// setto il numero del semaforo
	signal_b.sem_op = 1;						/// la 'signal' incrementa il valore del semaforo
	signal_b.sem_flg = 0;						/// nessuna flag
	if (semop(semid, &signal_b, 1) == -1) { 	/// eseguo la signal
		write(2,"SIGNAL abortita. TERMINO.\n",27);
		exit(2);
	}
}

/*@} */




int main (int argc, char *argv[]){
///variabili per i semafori
	int semid = atoi(argv[3]);

///variabili per la memoria condivisa
	key_t key = atoi(argv[2]);
	int shmid;
	int *buffer_comune;     ///risorsa condivisa
	char buf[50];


///altre variabili
	int readEOF;
	char *stringaDaFare;
	ssize_t fdOpen;
	int statusRead;
	int lunghezzaFile = 0;
	char bufferRead;
    char stampaIstr[50];


///recupero l'id della memoria condivisa
	if ((shmid = shmget(key, sizeof(int)*3 , 0)) < 0) {
		write(2,"Non sono riuscito a creare la memoria condivisa\n",49);
		exit(1);
	}

	if ( (buffer_comune = (int *)shmat(shmid, NULL, 0) ) == (int *)-1) {
        write(2,"Memoria condivisa non attaccata all'area dati del processo figlio\n",67);
        exit(1);
  }

///stringa contenente il nome del file da aprire
	char nomeFile[10] = "cpu_";
///stringa ausiliaria che serve per creare il nome del file da aprire
	char temp[2];
///converto in intero il numero del processore
	int nProc = atoi(argv[1]);
	nProc++;
///preparo l'occorrente per la stampa a video di quale processore sono
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
    char tmp[sizeof(buf)];  ///array temporaneo che uso per contenere i valori della read,write,sleep prima di essere copiato in memoria condivisa
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
        sem_wait(2, semid);     /// P tra tutti i processi in modo che in sezione critica entri solo uno

/// due stampe a video
        if((write(1,stampaVideo,23)) == -1){
            write(2,"Write fallita\n",15);
            exit(1);
        }
        if((write(1,"Entro in sezione critica\n",26)) == -1){
            write(2,"Write fallita\n",15);
            exit(1);
        }

/**salvo in memoria in memoria condivisa l'istruzione da eseguire
-buffer_comune[1] ho il primo numero
-buffer_comune[2] ho il secondo numero
-buffer_comune[0] ho la lettera convertita ad intero in base alle tabella ascii
*/
        while(buf[++cont] != '\0'){
            temp[cont2++] = buf[cont];
            if(buf[cont] == ' '){
                temp[--cont2] = '\0';
                buffer_comune[1] = atoi(temp);
                cont2 = 0;
            }
        }
        buffer_comune[2] = atoi(temp);
        buffer_comune[0] = buf[0];
///fine sezione critica
///eseguo l'istruzione nel caso sia una sleep
        if(buffer_comune[0] == 83) {
            sprintf(o,"%c %d\n",buffer_comune[0],buffer_comune[2]);
            if((write(1,o,5)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
            }
            sleep(buffer_comune[2]);
        }

///eseguo l'istruzione nel caso sia una write
        if(buffer_comune[0] == 87) {
///faccio un po di stampa a video
            sprintf(o,"%c %d %d\n",buffer_comune[0],buffer_comune[1],buffer_comune[2]);
            if((write(1,o,7)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
            }
///altre stampe a video
            sprintf(o,"Processore #%d: Il numero %d sarà scritto in memoria nella posizione %d\n",nProc,buffer_comune[1],buffer_comune[2]);
            if((write(1,o,71)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
            }

            sem_signal(1, semid);
            sem_wait(0, semid);
        }

///eseguo l'istruzione nel caso sia una read
        if(buffer_comune[0] == 82) {
///faccio un po di stampa a video
            sprintf(o,"%c %d\n",buffer_comune[0],buffer_comune[2]);
            if((write(1,o,5)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
            }

            sem_signal(1, semid);
            sem_wait(0, semid);
///altre stampe a video
            sprintf(o,"Processore #%d: Il numero che si voleva leggere era:%d\n",nProc,buffer_comune[1]);
            if((write(1,o,54)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
            }
        }

        sem_signal(2, semid);
    }
	///stacco la memoria condivisa dal processo
	if((shmdt(buffer_comune)) == -1){
        write(2,"Shmdt fallita\n",15);
        exit(1);
    }
return 0;
}



