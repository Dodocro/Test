#include <cstdlib>
#include <iostream>
#include <pthread.h>
using namespace std;

pthread_mutex_t lockin;
int noft;
int num = 0;
pthread_cond_t barijera;

void *zadatak_dretve (void *arg) {
	
	pthread_mutex_lock (&lockin);
	int trenutna_dretva = *((int*) arg);
	int a;
	
	cout << "Dretva " << trenutna_dretva << ". unesite broj" << endl;
	cin >> a;
	
	num++;
	if (num < noft) 
		pthread_cond_wait (&barijera, &lockin);	
	else {
		num = 0;
		pthread_cond_broadcast (&barijera);
	}
	
	cout << "Dretva " << trenutna_dretva << ". uneseni broj je " << a << endl;
	pthread_mutex_unlock (&lockin);
}

int main (int argc, char *argv []) {	
	noft = atoi (argv [1]);
	
	cout << "Broj dretvi=" << noft << endl;
		
	int *redni_broj = new int [noft];
	for (int i = 0; i < noft; i++) 
		redni_broj [i] = i;
	
	
	pthread_t *polje = new pthread_t [noft];
	for (int i = 0; i < noft; i++) 
		if (pthread_create (&polje [i], NULL, &zadatak_dretve, &redni_broj [i]) == -1 && (cout << "Greska pri stvaranju " << i << ". dretve!" << endl)) 
			exit(1);
	 	
	int i = 0;
	while (i < noft) {
		pthread_join (polje [i], NULL);
		i++;
	}
	
	pthread_mutex_destroy (&lockin);
	pthread_cond_destroy (&barijera);
	delete [] redni_broj;
	return 0;
}
