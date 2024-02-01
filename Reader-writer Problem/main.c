#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <signal.h>
#include "operacje.c"

#define N 5
#define P 20
#define MAX 20
#define MAX2 24

int semID,shmID;

void koniec(int sig){
	shmctl(shmID,IPC_RMID,NULL);
	semctl(semID,N,IPC_RMID,NULL);
	printf("MAIN - funkcja koniec sygnal %d: Koniec.\n",sig);
	exit(1);
}

int main(){

	key_t kluczs,klucz2;
	int i;

	struct sigaction act;
	act.sa_handler=koniec;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	sigaction(SIGINT,&act,0);

	if((kluczs=ftok(".",'A'))==-1){
		printf("Blad ftok.\n");
		exit(1);
	}

	if((klucz2=ftok(".",'B'))==-1){
		printf("Blad ftok.\n");
		exit(1);
	}

	semID=semget(kluczs,N,IPC_CREAT | IPC_EXCL | 0666);
	if(semID==-1){
		printf("Blad semaforow.\n");
		exit(1);
	}

	for(i=0;i<N;i++){
		inicjalizujSemafor(semID,i,1);
	}

	shmID=shmget(klucz2,MAX2*sizeof(int),IPC_CREAT | IPC_EXCL | 0666);
	if(shmID==-1){
		printf("Blad pamieci dzielonej.\n");
		exit(1);
	}


	for(i=0;i<P;i++){
		switch(fork()){
			case -1:
				perror("Blad fork(main).\n");
				exit(2);
			case 0:
				execl("./pisz","pisz",NULL);
		}
	}

	for(i=0;i<P;i++){
		switch(fork()){
			case -1:
				perror("Blad fork(main).\n");
				exit(2);
			case 0:
				execl("./czyt","czyt",NULL);
		}
	}


	for(i=0;i<2*P;i++)
		wait(NULL);
	shmctl(shmID,IPC_RMID,NULL);
	semctl(semID,N,IPC_RMID,NULL);
	printf("MAIN: Koniec.\n");

}
