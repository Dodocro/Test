#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;
typedef struct PODACI{
        int IN;
        int OUT;
        int M[5];
}Shared;
Shared *shared;
int shmId;
int N;
int K;
int PUNI = 0;
int PISI = 1;
int PRAZAN = 2;
int SemaforId;

void SemaforGet(int n){
        SemaforId = semget(IPC_PRIVATE, n, 0600);

        if(SemaforId == -1){
                cout<<"Nema semafora!"<<endl;
                exit(1);
        }
}
int SemSet(int SemNum, int SemVal){
        return semctl(SemaforId, SemNum, SETVAL, SemVal);
}



int SemaforOp(int SemNum, int SemaforOp){
        struct sembuf SemaforBuf;
        SemaforBuf.sem_num = SemNum;
        SemaforBuf.sem_op = SemaforOp;
        SemaforBuf.sem_flg = 0;
        return semop(SemaforId, &SemaforBuf, 1);
}

void brisi(int sig){
        (void) shmdt((char *) shared);
        (void) shmctl(shmId, IPC_RMID, NULL);
        exit(0);
}

void SemRemove(void){
        (void) semctl(SemaforId, 0, IPC_RMID, 0);
}

void Proiz(int y){
        srand(getpid());
        int x=0;
        for (x=0; x<K; x++){
                SemaforOp(PUNI,-1);
                SemaforOp(PISI,-1);
                shared->M[shared->IN] = rand()%1000;
                cout<<"Proizvodac "<<y<<" salje "<<shared->M[shared->IN]<<endl;
                sleep(1);
                shared->IN = (shared->IN + 1) % 5;
                SemaforOp(PISI, 1);
                SemaforOp(PRAZAN, 1);
        }
        cout<<"Proizvodac "<<y<<" zavrsio sa slanjem."<<endl;;
        exit(0);
}
void Potro(){
        int suma = 0;
        int x = 0;
        for (x=0; x<(N*K); x++){
                SemaforOp(PRAZAN,-1);
                cout<<"Potrosac prima "<<shared->M[shared->OUT]<<endl;
                sleep(1);
                suma += shared->M[shared->OUT];
                shared->OUT = (shared->OUT+1)%5;
                SemaforOp(PUNI, 1);
        }
        cout<<"Potrosac - suma primljenih brojeva = "<<suma<<endl;
        exit(0);
}


int main (int argc, char* argv[]) {
        if(argc < 3){
                cout<<"Neispravni argumenti, potrebno 2: broj procesa potrosaca i broj slucajnih brojeva"<<endl;
                return 0;
        }
        N = atoi(argv[1]);
        K = atoi(argv[2]);
        sigset(SIGINT, brisi);
        shmId = shmget(IPC_PRIVATE, sizeof(Shared), 0600);
        if(shmId == -1){
                cout<<"Nedovoljno memorije!"<<endl;
                exit(1);
        }
        shared = (Shared*) shmat(shmId, NULL, 0);
        shared->IN = 0;
        shared->OUT = 0;
        SemaforGet(3);
        SemSet(PISI, 1);
        SemSet(PUNI, 5);
        SemSet(PRAZAN, 0);
        int k;
 for(k=0; k<=N; k++){
                if (fork() == 0){
                        if(k==0){
                                Potro();
                        } else{
                                Proiz(k);
                        }
                }
        }
        int x;
        for(x=0; x<=N; x++){
                wait(NULL);
        }
        semctl(SemaforId, 0, IPC_RMID, 0);
        brisi(0);
        return 0;
}

