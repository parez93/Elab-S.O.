/** @file main.c
*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>



int numCpu;
int ramDim;

int main (int argc, char *argv[]){

///variabili che mi servono per creare la memoria condivisa
	int shmid;                  ///id della memoria condivisa
	int *buffer_comune;        ///risorsa condivisa

	key_t key = 5677;           ///chiave per ottenere l'id
	char keyToStr[1000];        ///chiave trasformata in carattere per passarla ai processi figli
	sprintf(keyToStr,"%d",key);


///variabili che mi servono per i semafori
	int semid;                          ///id dei semafori
	char semidToStr[1000];              ///id trasformato in carattere per passarlo ai processi figli
	key_t semkey;
	if((semkey = ftok("main.c", 'b')) == -1){ ///ottengo una chiave
        write(2,"Ftock fallita\n",15);
		exit(1);
	}

///union per la gestione del controllo sui semafori. ----> La variabile è "st_sem"
	union semun {
		int val;
		struct semid_ds *buf;
		ushort *array;
	} st_sem;


///altre variabili
	int cont = 0;           ///contatore che tiene conto delle cpu, serve per fare cont di fork di processori
    char countToStr[3];     ///converto in stringa il numero del processore corrente per poter aprire il file


 ///variabili pid processi
	pid_t statusProcessi;
	pid_t statusRam;

/// converto i dati in ingresso forniti dall'utente
	numCpu = atoi(argv[1]);
	ramDim = atoi(argv[2]);
///una stampa a video per avvertire che i dati immessi dall'utente sono stati acquisiti
	if(write(1,"Ho acquisito il numero di processori e la memoria della ram\n",61) == -1) {
		write(2,"Write fallita\n",15);
		exit(1);
    }


///creo 3 semafori
	if ((semid = semget(semkey, 3, 0777|IPC_CREAT)) == -1) {
		write(2,"Creazione semaforo non riuscita.\n",34);
		exit(1);
	}

///converto in stringa l'id dei semafori per poi passarlo ai processi
	sprintf(semidToStr,"%d",semid);

/**
semaforo 0 --> gestisce la sezione critica tra processi e ram
semaforo 1 --> gestisce la ram
semaforo 2 --> gestisce la sezione critica tra processi
*/
	st_sem.val = 0;
	if (semctl(semid, 0, SETVAL, st_sem) == -1) {			    ///gestisco comunicazione processo e ram
		write(2,"Semaforo processo non inizializzato.\n",38);
		semctl(semid, 0, IPC_RMID, 0);
		exit(1);
	}

	st_sem.val = 0;											    ///gestisco la ram
	if (semctl(semid, 1, SETVAL, st_sem) == -1) {
		write(2,"Semaforo ram non inizializzato.\n",33);
		semctl(semid, 0, IPC_RMID, 0);
		exit(1);
	}

	st_sem.val = 1;
	if (semctl(semid, 2, SETVAL, st_sem) == -1) {			    ///gestisco i processi
		write(2,"Semaforo processi non inizializzato.\n",38);
		semctl(semid, 0, IPC_RMID, 0);
		exit(1);
	}

///creo la memoria condivisa
	if ((shmid = shmget(key, sizeof(int)*3 , IPC_CREAT | 0777)) < 0) {
		write(2,"Non sono riuscito a creare la memoria condivisa\n",49);
		exit(1);
	}

///attacco la memoria condivisa all'area dati del processo padre
    if ((buffer_comune = (int *)shmat(shmid, 0, 0666)) == (int *)-1) {
		write(2,"Memoria condivisa non attaccata all'area dati del processo padre\n",68);
        exit(1);
    }

///Creo il processo della Ram
    if((statusRam = fork())== -1){
      write(2,"Non sono riuscito a creare il processo della Ram\n",50);
      exit(1);
    }
    else if (statusRam == 0)
        if((execl("ram", "ram", argv[2], keyToStr,semidToStr,  (char *) NULL)) == -1){
            write(2,"Non sono riuscito a fare l'execl\n",34);
            exit(1);
        }

    sleep(1);   ///aspetto un secondo affinchè la ram posso inizializzare tutti i dati e fermarsi al semaforo in attesa di essere sbloccata
///Creo i numCpu processi
    while(cont < numCpu) {
        if((statusProcessi=fork()) == -1) {
            write(2,"Non sono riuscito a creare il processo\n",40);
            exit(1);
        }
        if(statusProcessi == 0) {
            sprintf(countToStr,"%d",cont);
            if((execl("processore2", "processore2", countToStr, keyToStr,semidToStr, (char *) NULL)) == -1 ){
                write(2,"Non sono riuscito a fare l'execl\n",34);
                exit(1);
            }
        }
        cont++;
    }


///il processo padre aspetta che tutti i processi figli terminino per poter terminare pure lui
	for(cont = 0; cont < numCpu; cont++)
		wait(-1);

    sleep (1);
    kill(statusRam, 9);     ///faccio terminare la ram


    if(shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL) == -1){
        write(2,"Memoria non liberata.\n",23);
		exit(1);
    };

    if (semctl(semid, 0, IPC_RMID) == -1) {			///libero la memoria dai semafori
        write(2,"Semaforo processi non eliminato.\n",34);
		exit(1);
	}
	return 0;
}
