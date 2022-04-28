#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
//#include <stdbool.h> //Biblioteka za koriscenje bool vrednosti

const int N_ELVES = 7; //Definisanje broja vilenjaka
const int N_REINDEERS = 9; //Definisanje broja irvasa

int elves; //Brojac vilenjaka
int reindeers; //Brojac irvasa

/*** Definisanje semafora ***/
sem_t santaSem;
sem_t reindeerSem;
sem_t elfTex;
sem_t mutex;
/*** Definisanje semafora ***/

/*** Metode ***/
void* SantaClaus(void* arg);
void* Reindeer(void* arg);
void* Elf(void* arg);
/*** Metode ***/	

int main(void){
	elves = 0;
	reindeers = 0;
	int i = 0;
	int brojeviIrvasa[N_REINDEERS], brojeviVilenjaka[N_ELVES];
	
	printf("\nResenje 'The Santa Clauss problema' sa semaforima!\n\n");
	
/*** Inicijalizujemo semafore sa vredostima ***/
	sem_init(&santaSem, 0, 0);
	sem_init(&reindeerSem, 0, 0);
	sem_init(&elfTex, 0, 1);
	sem_init(&mutex, 0, 1);
/*** Inicijalizujemo semafore sa vredostima ***/
	
/*** Imenujemo niti ***/
	pthread_t santa;	
	pthread_t reindeers[N_REINDEERS];
	pthread_t elves[N_ELVES];
/*** Imenujemo niti ***/
	
	
/*** PTHREAD_CREATE ***/
	
	/* Deda Mraz */
	pthread_create(&santa, NULL, SantaClaus, NULL);
	/* Deda Mraz */
	
	/* Vilenjaci */
	for (i=0; i<N_ELVES; i++) {
		brojeviVilenjaka[i] = i;
	}
	for (i=0; i<N_ELVES; i++) {
		pthread_create(&elves[i], NULL, Elf, (void* )&brojeviVilenjaka[i]);
	}
	/* Vilenjaci */
	
	/* Irvasi */
	for (i=0; i<N_REINDEERS; i++) {
		brojeviIrvasa[i] = i;
	}
	for (i=0; i<N_REINDEERS; i++) {
		pthread_create(&reindeers[i], NULL, Reindeer, (void* )&brojeviIrvasa[i]);
	}
	/* Irvasi */
	
/*** PTHREAD_CREATE ***/	
	
	
/*** PTHREAD_JOIN ***/
	
	/* Vilenjaci */
	for (i=0; i<N_ELVES; i++) {
		pthread_join(elves[i], NULL);
	}
	/* Irvasi */
	for (i=0; i<N_REINDEERS; i++) {
		pthread_join(reindeers[i], NULL);
	}
	/* Deda Mraz */
	pthread_join(santa,NULL);
	
/*** PTHREAD_JOIN ***/
	
/*** Unistevanje semafora, zbog curenja memorije ***/	
	sem_destroy(&santaSem);
	sem_destroy(&reindeerSem);
	sem_destroy(&elfTex);
	sem_destroy(&mutex);
/*** Unistevanje semafora, zbog curenja memorije ***/
	return 0;
}

/**** Santa Claus ****/
void* SantaClaus(void* arg){
	printf("\tDeda mraz je tu!\n"); //Signalizira ulaz niti u funkciju
/*
	Kod je napisan da se izvrsi tri puta, ali moguce je napisati da se izvrsava
	beskonacno, uz jednu while(true) petlju.
*/
	int brojac = 0; //Uslov da petlja bude konacna i da program bude konacan
	while(brojac < 3){
		sem_wait(&santaSem);
		sem_wait(&mutex);
		if(reindeers == N_REINDEERS){ //Logika za irvase
			printf("DEDA MRAZ: Pripremanje sanki\n");
			for(int i = 0; i < N_REINDEERS; i++){
				sem_post(&reindeerSem); //Deda Mraz sprema irvase za put
			}
			sleep(1); //Obezbedjuje da se svi irvasi spreme pre nego krenu
			printf("DEDA MRAZ: Ucinimo svu decu na svetu srecnom!\n");
			brojac++;
			reindeers = 0;
		}else if(elves == 3){ //Logika za vilenjake
			printf("DEDA MRAZ: Pomaze vilenjacima\n");
			brojac++;
		}
		sem_post(&mutex);
	}	
}

/**** Reindeer ****/
void* Reindeer(void* arg){
	sleep(0.05); //Obezbedjuje da uvek Deda Mraz prvi pocne sa kodom, zbog neodredjenosti redosleda kreiranja niti
	int id = * (int* )arg;
	printf("\tIrvas %d je tu\n", id + 1); //Signalizira ulaz niti u funkciju
/*
	Kod je napisan da se izvrsi jednom, ali moguce je napisati da se izvrsava
	beskonacno, uz jednu while(true) petlju i sleep komandu zbog niti.
*/
	sem_wait(&mutex);
	reindeers++;
	if(reindeers == N_REINDEERS){
		sem_post(&santaSem); //Poziva se Deda Mraz, jer su svih 9 irvasa spremni
	}
	sem_post(&mutex);
	sem_wait(&reindeerSem); //Irvasi cekaju da Deda Mraz dodje i spremi ih za put
	printf("\tIrvas %d se vezuje\n", id + 1);
}

/**** Elf ****/
void* Elf(void* arg){
	sleep(0.05); //Obezbedjuje da uvek Deda Mraz prvi pocne sa kodom, zbog neodredjenosti kreiranja niti
	int id = * (int* ) arg;
	printf("\tVilenjak %d je tu\n", id + 1); //Signalizira ulaz niti u funkciju
	
/*	Kod je napisan da se izvrsi jednom, ali moguce je napisati da se izvrsava
	beskonacno, uz jednu while(true) petlju i sleep komandu zbog niti.
	Takodje kod je napisan da je svakom vilenjaku potrebna pomoc od Deda Mraza,
	ali moguce je napraviti if logiku uz nasumicno generisanje boolean vrednosti,
	koja bi odredjivala da li ce nekom vilenjaku biti potrebna pomoc ili ne.
*/

	sem_wait(&elfTex); //Obezbedjuje da ni jedan vilenjak ne trazi pomoc od Deda Mraza, dok pomaze drugim vilenjacima
	sem_wait(&mutex);
	elves++;
	if(elves == 3){
		sem_post(&santaSem); //Poziva se Deda Mraz jer tri vilenjaka trebaju pomoc
	}else{
		sem_post(&elfTex);
	}
	sem_post(&mutex);
	
	printf("\tVilenjak %d ce dobiti pomoc od Deda Mraza\n", id + 1);
	sleep(3); //Omogucava da se skupe vilenjaci i da im Deda Mraz pomogne
		
	sem_wait(&mutex);
	elves--;
	if(elves == 0){
		sem_post(&elfTex); //Nakon sto je Deda Mraz pomogao vilenjacima, dopusta drugim vilenjacima da traze pomoc
	}
	sem_post(&mutex);
	
	printf("\tVilenjak %d radi\n", id + 1);
}
