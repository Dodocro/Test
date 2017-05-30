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
        int ULAZ;
        int IZLAZ;
        int M[5];
}Shared;
Shared *shared;
int shmId;
int N;
int K;
int PUN = 0;
int PISI = 1;
int PRAZAN = 2;
int SemId;

void SemGet(int n){
        SemId = semget(IPC_PRIVATE, n, 0600);

        if(SemId == -1){
                cout<<"Nema semafora!"<<endl;
                exit(1);
        }
}
int SemSetVal(int SemNum, int SemVal){
        return semctl(SemId, SemNum, SETVAL, SemVal);
}

int SemOp(int SemNum, int SemOp){
        struct sembuf SemBuf;
        SemBuf.sem_num = SemNum;
        SemBuf.sem_op = SemOp;
        SemBuf.sem_flg = 0;
        return semop(SemId, &SemBuf, 1);
}

void SemRemove(void){
        (void) semctl(SemId, 0, IPC_RMID, 0);
}
void Proizvodac(int j){
        srand(getpid());
        int i=0;
        for (i=0; i<K; i++){
                SemOp(PUN,-1);
                SemOp(PISI,-1);
                shared->M[shared->ULAZ] = rand()%1000;
                cout<<"Proizvodac "<<j<<" salje "<<shared->M[shared->ULAZ]<<endl;
                sleep(1);
                shared->ULAZ = (shared->ULAZ + 1) % 5;
                SemOp(PISI, 1);
                SemOp(PRAZAN, 1);
        }
        cout<<"Proizvodac "<<j<<" zavrsio sa slanjem."<<endl;;
        exit(0);
}
void Potrosac(){
        int suma = 0;
        int i = 0;
        for (i=0; i<(N*K); i++){
                SemOp(PRAZAN,-1);
                cout<<"Potrosac prima "<<shared->M[shared->IZLAZ]<<endl;
                sleep(1);
                suma += shared->M[shared->IZLAZ];
                shared->IZLAZ = (shared->IZLAZ+1)%5;
                SemOp(PUN, 1);
        }
        cout<<"Potrosac - suma primljenih brojeva = "<<suma<<endl;
        exit(0);
}
void brisi(int sig){
        (void) shmdt((char *) shared);
        (void) shmctl(shmId, IPC_RMID, NULL);
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
        shared->ULAZ = 0;
        shared->IZLAZ = 0;
        SemGet(3);
        SemSetVal(PISI, 1);
        SemSetVal(PUN, 5);
        SemSetVal(PRAZAN, 0);
        int k;
 for(k=0; k<=N; k++){
                if (fork() == 0){
                        if(k==0){
                                Potrosac();
                        } else{
                                Proizvodac(k);
                        }
                }
        }
        int i;
        for(i=0; i<=N; i++){
                wait(NULL);
        }
        semctl(SemId, 0, IPC_RMID, 0);
        brisi(0);
        return 0;
}

