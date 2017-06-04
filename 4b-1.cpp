#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <signal.h>
#include <time.h>
#include <semaphore.h>
#include <sys/wait.h>
using namespace std;

class podaci{
	public: 
		pthread_cond_t monitorNa;
		pthread_cond_t monitorVele;
		pthread_mutex_t monitor;
		int rand;
		bool ima;
};
podaci *data;

void init(){
	pthread_mutexattr_t monitora;
	pthread_mutexattr_init(&monitora);
	pthread_mutexattr_setpshared(&monitora, PTHREAD_PROCESS_SHARED);
	
	pthread_mutex_init(&data->monitor, &monitora);
	
	
	pthread_condattr_t cond_attr;
	pthread_condattr_init(&cond_attr);
	pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
	
	pthread_cond_init(&data->monitorNa, &cond_attr);
	pthread_cond_init(&data->monitorVele, &cond_attr);
	
	pthread_condattr_destroy(&cond_attr);
	pthread_mutexattr_destroy(&monitora);
}

string komponente[] = {"monitorima", "racunalima", "tipkovnicama"};
string uzmi[] = {"racunalo i tipkovnicu", "tipkovnicu i monitor", "monitor i racunalo"};
bool a = true;

void nakupac(int ID_nakupca) {
		sleep(1);
    pthread_mutex_lock(&data->monitor);
    while(true){
        while(data->rand!=ID_nakupca||!data->ima){
			pthread_cond_wait( &data->monitorNa, &data->monitor);
		}
		data->ima = false;
		cout << "Nakupac s "  << komponente[ID_nakupca] << " uzeo " << uzmi[data->rand] << "." << endl<<endl;
		
		sleep(1);
		pthread_cond_signal(&(data->monitorVele));
    }
    pthread_mutex_unlock(&data->monitor);
}
int ID;

void veletrgovac() {
		sleep(1);
    srand(time(NULL));
    pthread_mutex_lock(&data->monitor);
    while(true){
        data->rand = rand() % 3;
        
        cout << "Veletrgovac stavio " << uzmi[data->rand] << endl;
		sleep(1);
		data->ima = true;
        pthread_cond_broadcast(&(data->monitorNa));
        pthread_cond_wait(&data->monitorVele, &data->monitor);
    }
    pthread_mutex_unlock(&data->monitor);
}

void brisi(int sig){
	shmdt((char *) data);
   	shmctl(ID, IPC_RMID, NULL);
   	exit(0);
}

int main() {
	sigset (SIGINT, brisi);
	sigset (SIGQUIT, brisi);
	sigset (SIGTSTP, brisi);
	
    ID = shmget(IPC_PRIVATE, sizeof(podaci), 0600);
    data = (podaci*) shmat(ID, NULL, 0);
    data->rand = -1;
	data->ima = false;
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
    
    brisi(0);
    return 0;
}
