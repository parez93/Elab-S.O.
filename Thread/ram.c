/** @file ram.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>


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
	wait_b.sem_op = -1;			                /// la 'wait' decrementa il valore del semaforo
	wait_b.sem_flg = 0;				            ///nessuna flag
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
	signal_b.sem_num = sem_number;	            /// setto il numero del semaforo
	signal_b.sem_op = 1;						/// la 'signal' incrementa il valore del semaforo
	signal_b.sem_flg = 0;						/// nessuna flag
	if (semop(semid, &signal_b, 1) == -1) {	    /// eseguo la signal
		write(2,"SIGNAL abortita. TERMINO.\n",27);
		exit(2);
	}
}
/*@} */


int main (int argc, char *argv[]){
///variabili per i semafori
	int semid = atoi(argv[3]);

///altre variabili
	int cont;
	char temp[100];
	int ramDim = atoi(argv[1]);
	int shmid;
	key_t key = atoi(argv[2]); ///chiave della memoria condivisa
	int *buffer_comune;
	int memoria[ramDim];


	sprintf(temp,"Ram: Sono dentro la Ram di dimensione %d\n",ramDim);
    if((write(1,temp,41)) == -1){
        write(2,"Write fallita\n",15);
        exit(1);
    }

///inizializzo l'array della memoria a 0
    for(cont=0; cont<ramDim; cont++)
        memoria[cont]=0;

	if ((shmid = shmget(key, 0 , 0)) < 0) {
		write(2,"Non sono riuscito a creare la memoria condivisa\n",49);
		exit(1);
	}

/// Attacco il segmento all ’ area dati del processo
	if ((buffer_comune = (int *)shmat(shmid , NULL, 0)) == (int *) -1) {
		write(2,"Non sono riuscito ad attaccare il segmento alla memoria condivisa\n",67);
		exit(1);
	}

///entro in un loop;
	while(1) {
		sem_wait(1, semid);
		if(buffer_comune[0] == 87){     ///se è una Write
			memoria[buffer_comune[2]] = buffer_comune[1];
			sprintf(temp,"Ram: ho scritto in memoria il numero %d alla posizione %d\n", buffer_comune[1], buffer_comune[2]);
			if((write(1,temp,57)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
			}
        }
		if(buffer_comune[0] == 82) {	///se è una Read
			buffer_comune [1] = memoria[buffer_comune[2]];
			if((write(1,"Ram: ho letto, ora il processore dovrebbe darmi l'ok\n",55)) == -1){
                write(2,"Write fallita\n",15);
                exit(1);
			}
        }
		sem_signal(0, semid);
	}

	///stacco la memoria condivisa dal processo
	if((shmdt(buffer_comune)) == -1){
        write(2,"Shmdt fallita\n",15);
        exit(1);
    }
	return 0;
}
