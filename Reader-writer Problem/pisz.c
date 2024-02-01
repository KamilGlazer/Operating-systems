#include <stdio.h>
#include "operacje.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include "operacje.c"

#define N 5
#define MAX 20
#define MAX2 24
#define P 20

int main(){
        key_t kluczs,kluczm;
        int semID,shmID;
        int *pam;

	#define odczyt pam[MAX]
	#define zapis pam[MAX+1]
	#define lc pam[MAX+2]
	#define lp pam[MAX+3]

        if((kluczs=ftok(".",'A'))==-1){
                printf("Blad ftok - piszacy.\n");
                exit(2);
        }

        if((kluczm=ftok(".",'B'))==-1){
                printf("Blad ftok - piszacy.\n");
                exit(2);
        }

        semID=alokujSemafor(kluczs,N,IPC_CREAT | 0666);
        if(semID==-1){
                printf("Blad semafora - piszacy.\n");
                exit(2);
        }

        shmID=shmget(kluczm,MAX2*sizeof(int),IPC_CREAT | 0666);
        if(shmID==-1){
                printf("Blad pamieci dzielonej.\n");
                exit(1);
        }


	// 0 -w1- dla wykluczenie wzajemnego procesow czytajacych
	// 1 -sp- dla wykluczenia wzajemnego procesow piszacych
	// 2 -sc- dla ochrony wejscia do sekcji krytycznej procesu czytajacego
	// 3 -w2- dla wykluczania wzajemnego procesow piszacych w chwili rozpoczynania i konczenia korzystania z zasobu
	// 4 -w3- dla zapewnienia priorytetu pisania nad czytaniem

	pam=(int*)shmat(shmID,NULL,0);

	waitSemafor(semID,3,0);

	lp++;
	if(lp==1){
		waitSemafor(semID,2,0);
	}

	signalSemafor(semID,3);

	waitSemafor(semID,1,0);

	pam[zapis]=getpid();
	printf("%d - Zapisuje\n",getpid());
	sleep(getpid()%3);
	printf("zapisalem %d pod indeks %d\n",pam[zapis],zapis);
	zapis=(zapis+1)% MAX;

	signalSemafor(semID,1);
	waitSemafor(semID,3,0);

	lp--;
	if(lp==0){
		signalSemafor(semID,2);
	}
	signalSemafor(semID,3);

}
