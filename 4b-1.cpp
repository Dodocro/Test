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

class klasa{
	public: 
		pthread_cond_t monitorN;
		pthread_cond_t monitorV;
		pthread_mutex_t monitor;
		int rand;
		bool ima;
};
klasa *data;

void in_it(){
	pthread_mutexattr_t monitora;
	pthread_mutexattr_init(&monitora);
	pthread_mutexattr_setpshared(&monitora, PTHREAD_PROCESS_SHARED);
	
	pthread_mutex_init(&data->monitor, &monitora);
	
	
	pthread_condattr_t cond_attr;
	pthread_condattr_init(&cond_attr);
	pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
	
	pthread_cond_init(&data->monitorN, &cond_attr);
	pthread_cond_init(&data->monitorV, &cond_attr);
	
	pthread_condattr_destroy(&cond_attr);
	pthread_mutexattr_destroy(&monitora);
}

void kupac(int ID_kupca) {
		sleep(1);
    pthread_mutex_lock(&data->monitor);
    while(true){
        while(data->rand!=ID_kupca||!data->ima){
			pthread_cond_wait( &data->monitorN, &data->monitor);
		}
		data->ima = false;
		cout << "Nakupac s "  << stvari[ID_kupca] << " uzeo " << uzmi[data->rand] << "." << endl<<endl;
		
		sleep(1);
		pthread_cond_signal(&(data->monitorV));
    }
    pthread_mutex_unlock(&data->monitor);
}

string stvari[] = {"monitorima", "racunalima", "tipkovnicama"};
string uzmi[] = {"racunalo i tipkovnicu", "tipkovnicu i monitor", "monitor i racunalo"};
bool a = true;


int ID;



void obrisi(int sig){
	shmdt((char *) data);
   	shmctl(ID, IPC_RMID, NULL);
   	exit(0);
}

void veletrg() {
		sleep(1);
    srand(time(NULL));
    pthread_mutex_lock(&data->monitor);
    while(true){
        data->rand = rand() % 3;
        
        cout << "Veletrgovac stavio " << uzmi[data->rand] << endl;
		sleep(1);
		data->ima = true;
        pthread_cond_broadcast(&(data->monitorN));
        pthread_cond_wait(&data->monitorV, &data->monitor);
    }
    pthread_mutex_unlock(&data->monitor);
}

int main() {
	sigset (SIGINT, obrisi);
	sigset (SIGQUIT, obrisi);
	sigset (SIGTSTP, obrisi);
	
    ID = shmget(IPC_PRIVATE, sizeof(klasa), 0600);
    data = (klasa*) shmat(ID, NULL, 0);
    data->rand = -1;
	data->ima = false;
    in_it();
    
    int pid = fork();
    if(pid == 0) {
		veletrg();
	}
	else{
		
		cout<<"pid veletrgovac: "<<pid<<endl;
	}
	pid = fork();
    if(pid == 0) kupac(0);
	else{
		cout<<"pid nakupac0: "<<pid<<endl;
	}
	pid = fork();
    if(pid == 0) kupac(1);
	else{
		cout<<"pid nakupac1: "<<pid<<endl;
	}
	pid = fork();
    if(pid == 0) kupac(2);
	else{
		cout<<"pid nakupac2: "<<pid<<endl;
	}
    
    
    
    for(int u = 0 ; u < 4; u++) wait(NULL);
    
    obrisi(0);
    return 0;
}
