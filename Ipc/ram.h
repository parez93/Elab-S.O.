#ifndef RAM_H
#define RAM_H
void sem_wait(int sem_number, int semid);
void sem_signal(int sem_number,int semid);
void *ram (void *arg);
#endif
