#ifndef UTILITY_H
#define UTILITY_H


pthread_mutex_t mutexRam;
pthread_mutex_t mutexProc;
pthread_mutex_t mutexAllProc;
int killRam;

typedef struct {
   int *buffer;
   int nProc;
   int ramDim;
 } struttura;
#endif
