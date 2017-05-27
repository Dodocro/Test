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
    int broj[7];
}*semafor;


void izbrisi_memoriju(int signalId) {
    (void) shmdt((char*) semafor);
    (void) shmctl(id, IPC_RMID, NULL);
    exit(0);
    
void *glavno(int i)
{
     sleep(1);
     int n = i;
     if(n==2) sem_wait(&semafor->s[1]);
     if(n==3) sem_wait(&semafor->s[6]);
     if(n==4) sem_wait(&semafor->s[0]);
     if(n==5) sem_wait(&semafor->s[2]);
     if(n==6){
         sem_wait(&semafor->s[3]);
         sem_wait(&semafor->s[4]);
         sem_wait(&semafor->s[5]);
     }
     int j;
     for(j=0;j<semafor->broj[n];j++){
                     cout<<"Izvodim zadatak "<<n+1<<": "<<j+1<<"/"<<semafor->broj[n]<<endl;
                     sleep(1);
                     }
     if(n==1){
     	    sem_post(&semafor->s[1]);
            sem_post(&semafor->s[6]);
	 }
     if(n==0) sem_post(&semafor->s[0]);
 if(n==2) sem_post(&semafor->s[2]);
     if(n==3) sem_post(&semafor->s[3]);
     if(n==4) sem_post(&semafor->s[4]);
     if(n==5) sem_post(&semafor->s[5]);
     exit(0);
}

}
int main() {
    id=shmget(IPC_PRIVATE, sizeof(sem), 0600);
    if(id==-1) exit(1);
    sigset(SIGINT, izbrisi_memoriju);
    semafor=(sem *) shmat(id, NULL, 0);
    srand(time(NULL));
    rand();
    int i;
    for(i=0;i<7;i++){
            sem_init(&semafor->s[i], 1, 0);
                semafor->broj[i]=rand()%9+1;
            }
    sleep(1);
    for(i=0;i<7;i++){
            if(fork()==0){
                glavno(i);
            }
    }
 for(i=0;i<7;i++){
            wait(NULL);
    }
    izbrisi_memoriju(0);
    return 0;
}



