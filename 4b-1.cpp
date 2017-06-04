#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>

using namespace std;

class pod{
	public: 
		pthread_cond_t mNakupac;
		pthread_cond_t mVeletrgovac;
		pthread_mutex_t monitor;
		int rand;
		bool dost;
};
pod *podatak;

void init(){
	pthread_mutexattr_t monitora;
	pthread_mutexattr_init(&monitora);
	pthread_mutexattr_setpshared(&monitora, PTHREAD_PROCESS_SHARED);
	
	pthread_mutex_init(&podatak->monitor, &monitora);
	
	
	pthread_condattr_t cond_attr;
	pthread_condattr_init(&cond_attr);
	pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
	
	pthread_cond_init(&podatak->mNakupac, &cond_attr);
	pthread_cond_init(&podatak->mVeletrgovac, &cond_attr);
	
	pthread_condattr_destroy(&cond_attr);
	pthread_mutexattr_destroy(&monitora);
}

string komp[] = {"monitorima", "racunalima", "tipkovnicama"};
string uzima[] = {"racunalo i tipkovnicu", "tipkovnicu i monitor", "monitor i racunalo"};
bool a = true;

void nakupac(int IDnakupac) {
		sleep(1);
    pthread_mutex_lock(&podatak->monitor);
    while(true){
        while(podatak->rand!=IDnakupac||!podatak->dost){
			pthread_cond_wait( &podatak->mNakupac, &podatak->monitor);
		}
		podatak->dost = false;
		cout << "Nakupac s "  << komp[IDnakupac] << " uzeo " << uzima[podatak->rand] << "." << endl<<endl;
		
		sleep(1);
		pthread_cond_signal(&(podatak->mVeletrgovac));
    }
    pthread_mutex_unlock(&podatak->monitor);
}
int ID;

void veletrgovac() {
		sleep(1);
    srand(time(NULL));
    pthread_mutex_lock(&podatak->monitor);
    while(true){
        podatak->rand = rand() % 3;
        
        cout << "Veletrgovac stavio " << uzima[podatak->rand] << endl;
		sleep(1);
		podatak->dost = true;
        pthread_cond_broadcast(&(podatak->mNakupac));
        pthread_cond_wait(&podatak->mVeletrgovac, &podatak->monitor);
    }
    pthread_mutex_unlock(&podatak->monitor);
}

void izbrisi(int sig){
	shmdt((char *) podatak);
   	shmctl(ID, IPC_RMID, NULL);
   	exit(0);
}

int main() {
	sigset (SIGINT, izbrisi);
	sigset (SIGQUIT, izbrisi);
	sigset (SIGTSTP, izbrisi);
	
    ID = shmget(IPC_PRIVATE, sizeof(pod), 0600);
    podatak = (pod*) shmat(ID, NULL, 0);
    podatak->rand = -1;
	podatak->dost = false;
    init();
    
    int pid = fork();
    if(pid == 0) {
		veletrgovac();
	}
	else{
		
		cout<<"pid veletrgovac: "<<pid<<endl;
	}
	pid = fork();
    if(pid == 0) nakupac(0);
	else{
		cout<<"pid nakupac0: "<<pid<<endl;
	}
	pid = fork();
    if(pid == 0) nakupac(1);
	else{
		cout<<"pid nakupac1: "<<pid<<endl;
	}
	pid = fork();
    if(pid == 0) nakupac(2);
	else{
		cout<<"pid nakupac2: "<<pid<<endl;
	}
    
    
    
    for(int u = 0 ; u < 4; u++) wait(NULL);
    
    izbrisi(0);
    return 0;
}
