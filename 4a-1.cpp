#include <iostream>
#include <cstdlib>
#include <pthread.h>
using namespace std;

pthread_mutex_t lockin;
int n;
int broj = 0;
pthread_cond_t barijera;

void *glavno (void *arg) {
	
	pthread_mutex_lock (&lockin);
	int trenutno = *((int*) arg);
	int dretva;
	
	cout << "Dretva " << trenutno << ". unesite broj" << endl;
	cin >> dretva;
	
	broj++;
	if (broj < n) 
		pthread_cond_wait (&barijera, &lockin);	
	else {
		broj = 0;
		pthread_cond_broadcast (&barijera);
	}
	
	cout << "Dretva " << trenutno << ". uneseni broj je " << dretva << endl;
	pthread_mutex_unlock (&lockin);
}

int main (int argc, char *argv []) {
	if(argv[1]==NULL){
 	cout<<"Neispravan unos."<<endl;
 	return 0;
  	}	
	n = atoi (argv [1]);
	
	cout << "Broj dretvi=" << n << endl;
		
	int *rednibroj = new int [n];
	for (int i = 0; i < n; i++) 
		rednibroj [i] = i;
		
	pthread_t *polje = new pthread_t [n];
	for (int i = 0; i < n; i++) 
		if (pthread_create (&polje [i], NULL, &glavno, &rednibroj [i]) == -1 && (cout << "Greska pri stvaranju " << i << ". dretve!" << endl)) 
			exit(1);
	 	
	int i = 0;
	while (i < n) {
		pthread_join (polje [i], NULL);
		i++;
	}
	
	pthread_mutex_destroy (&lockin);
	pthread_cond_destroy (&barijera);
	delete [] rednibroj;
	return 0;
}
