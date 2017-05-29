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
}Share;
Share *share;

int PUN = 0;
int PISI = 1;
int PRAZAN = 2;
int Sid;
int shmid;
int A;
int B;

void Sget(int n){
        Sid = semget(IPC_PRIVATE, n, 0600);

        if(Sid == -1){
                cout<<"Nema semafora!"<<endl;
                exit(1);
        }
}

int Sop(int Snum, int Sop){
        struct sembuf Sbuf;
        Sbuf.sem_num = Snum;
        Sbuf.sem_op = Sop;
        Sbuf.sem_flg = 0;
        return semop(Sid, &Sbuf, 1);
}
int SSV(int Snum, int SemVal){
        return semctl(Sid, Snum, SETVAL, SemVal);
}



void Sremove(void){
        (void) semctl(Sid, 0, IPC_RMID, 0);
}
void izbrisi(int sig){
        (void) shmdt((char *) share);
        (void) shmctl(shmid, IPC_RMID, NULL);
        exit(0);
}
void Proizvodac(int j){
        srand(getpid());
        int i=0;
        for (i=0; i<B; i++){
                Sop(PUN,-1);
                Sop(PISI,-1);
                share->M[share->ULAZ] = rand()%1000;
                cout<<"Proizvodac "<<j<<" salje "<<share->M[share->ULAZ]<<endl;
                sleep(1);
                share->ULAZ = (share->ULAZ + 1) % 5;
                Sop(PISI, 1);
                Sop(PRAZAN, 1);
        }
        cout<<"Proizvodac "<<j<<" zavrsio sa slanjem."<<endl;;
        exit(0);
}
void Potrosac(){
        int suma = 0;
        int i = 0;
        for (i=0; i<(A*B); i++){
                Sop(PRAZAN,-1);
                cout<<"Potrosac prima "<<share->M[share->IZLAZ]<<endl;
                sleep(1);
                suma += share->M[share->IZLAZ];
                share->IZLAZ = (share->IZLAZ+1)%5;
                Sop(PUN, 1);
        }
        cout<<"Potrosac: suma primljenih brojeva = "<<suma<<endl;
        exit(0);
}


int main (int argc, char* argv[]) {
        if(argc < 3){
                cout<<"Neispravni argumenti, potrebno 2: broj procesa potrosaca i broj slucajnih brojeva"<<endl;
                return 0;
        }
        A = atoi(argv[1]);
        B = atoi(argv[2]);
        sigset(SIGINT, izbrisi);
        shmid = shmget(IPC_PRIVATE, sizeof(Share), 0600);
        if(shmid == -1){
                cout<<"Nedovoljno memorije!"<<endl;
                exit(1);
        }
        share = (Share*) shmat(shmid, NULL, 0);
        share->ULAZ = 0;
        share->IZLAZ = 0;
        Sget(3);
        SSV(PISI, 1);
        SSV(PUN, 5);
        SSV(PRAZAN, 0);
        int k;
 for(k=0; k<=A; k++){
                if (fork() == 0){
                        if(k==0){
                                Potrosac();
                        } else{
                                Proizvodac(k);
                        }
                }
        }
        int i;
        for(i=0; i<=A; i++){
                wait(NULL);
        }
        semctl(Sid, 0, IPC_RMID, 0);
        izbrisi(0);
        return 0;
}

