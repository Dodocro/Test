#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<semaphore.h>
#include<unistd.h>
#include<iostream>
#include<signal.h>
#include <sys/shm.h>
#include <sys/wait.h>

using namespace std;

int id;
struct sem{
    sem_t s[7];
    int br[7];
}*semaf;

void brisi_memoriju(int signalId) {
    (void) shmdt((char*) semaf);
    (void) shmctl(id, IPC_RMID, NULL);
    exit(0);
}

void *zad(int x)
{
     sleep(1);
     int n = x;
     if(n==2) sem_wait(&semaf->s[1]);
     if(n==3) sem_wait(&semaf->s[6]);
     if(n==4) sem_wait(&semaf->s[0]);
     if(n==5) sem_wait(&semaf->s[2]);
     if(n==6){
         sem_wait(&semaf->s[3]);
         sem_wait(&semaf->s[4]);
         sem_wait(&semaf->s[5]);
     }
     int y;
     for(y=0;y<semaf->br[n];y++){
                     cout<<"Izvodim zadatak "<<n+1<<": "<<y+1<<"/"<<semaf->br[n]<<endl;
                     sleep(1);
                     }
     if(n==0) sem_post(&semaf->s[0]);
     if(n==1){
            sem_post(&semaf->s[1]);
            sem_post(&semaf->s[6]);
     }
 if(n==2) sem_post(&semaf->s[2]);
     if(n==3) sem_post(&semaf->s[3]);
     if(n==4) sem_post(&semaf->s[4]);
     if(n==5) sem_post(&semaf->s[5]);
     exit(0);
}
int main() {
    id=shmget(IPC_PRIVATE, sizeof(sem), 0600);
    if(id==-1) exit(1);
    sigset(SIGINT, brisi_memoriju);
    semaf=(sem *) shmat(id, NULL, 0);
    srand(time(NULL));
    rand();
    int x;
    for(x=0;x<7;x++){
            sem_init(&semaf->s[x], 1, 0);
                semaf->br[x]=rand()%9+1;
            }
    sleep(1);
    for(x=0;x<7;x++){
            if(fork()==0){
                zad(x);
            }
    }
 for(x=0;x<7;x++){
            wait(NULL);
    }
    brisi_memoriju(0);
    return 0;
}



