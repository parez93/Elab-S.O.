#ifndef PROCESSORE2_H
#define PROCESSORE2_H
void sem_wait(int sem_number, int semid);
void sem_signal(int sem_number,int semid);
void *processore2 (void *arg);
#endif
