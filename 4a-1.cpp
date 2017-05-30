#include <cstdlib>
#include <iostream>
#include <pthread.h>
using namespace std;

pthread_mutex_t lockin;
int no;
int br = 0;
pthread_cond_t Barijera;

void *zadatak (void *arg) {
	
	pthread_mutex_lock (&lockin);
	int tren_dretva = *((int*) arg);
	int a;
	
	cout << "Dretva " << tren_dretva << ". unesite broj" << endl;
	cin >> a;
	
	br++;
	if (br < no) 
		pthread_cond_wait (&Barijera, &lockin);	
	else {
		br = 0;
		pthread_cond_broadcast (&Barijera);
	}
	
	cout << "Dretva " << tren_dretva << ". uneseni broj je " << a << endl;
	pthread_mutex_unlock (&lockin);
}

int main (int argc, char *argv []) {	
	no = atoi (argv [1]);
	
	cout << "Broj dretvi=" << no << endl;
		
	int *red_br = new int [no];
	for (int x = 0; x < no; x++) 
		red_br [x] = x;
	
	
	pthread_t *polje = new pthread_t [no];
	for (int x = 0; x < no; x++) 
		if (pthread_create (&polje [x], NULL, &zadatak, &red_br [x]) == -1 && (cout << "Greska pri stvaranju " << x << ". dretve!" << endl)) 
			exit(1);
	 	
	int x = 0;
	while (x < no) {
		pthread_join (polje [x], NULL);
		x++;
	}
	
	pthread_mutex_destroy (&lockin);
	pthread_cond_destroy (&Barijera);
	delete [] red_br;
	return 0;
}
