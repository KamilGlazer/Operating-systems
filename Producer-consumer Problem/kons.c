#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
struct bufor{
	int mtype;
	int mvalue;
};
int *pam;
#define MAX2 12
#define MAX 10
#define CZYTAJ 4
#define ZAPISZ 3
#define PELNY 2
#define PUSTY 1
#define zapis pam[MAX+1]
#define odczyt pam[MAX]

int main()
{
   key_t klucz, kluczm;
   int msgID, shmID;
   int i;
   struct bufor komunikat,newMess,komunikat2,newMess2;
   sleep(1);
   printf("konsument--------------------------------\n");
   if ( (klucz = ftok(".", 'A')) == -1 )
   {
      printf("Blad ftok (A)\n");
      exit(2);
   };
   msgID=msgget(klucz, IPC_CREAT | 0666);
   if (msgID==-1)
   {
      printf("blad klejki komunikatow\n");
      exit(1);
    };

   kluczm=ftok(".",'B');
   shmID=shmget(kluczm, MAX2*sizeof(int), IPC_CREAT|0666);
   pam=(int*)shmat(shmID,NULL,0);
   //odbiorpowiedniego komunikatu
    struct sembuf operacje;

// w sekcji krytycznej odczyt z pamiedzielonej spod indeksu oczyt
// uaktualnienie wskazdo odczytu (mod MAX
//wyslanie odpowiedniego komunkat


	if(msgrcv(msgID,&komunikat,sizeof(int),PELNY,0)==-1){
		perror("Blad przy odbieraniu wiadomosci z producenta!\n");
		exit(1);
	}

	sleep(getpid()%10);


	if(msgrcv(msgID,&komunikat2,sizeof(int),CZYTAJ,0)!=-1){
		int index=pam[10];
        	int save=index;
        	int pidFromSharedMemory=pam[index];
        	printf("Konsument - odczyt z bufora %d - PID: %d\n",index,pidFromSharedMemory);

        	pam[index]=0;
        	pam[10] = (index+1)%MAX;

        	newMess2.mtype=CZYTAJ;
                newMess2.mvalue=0;

                if(msgsnd(msgID,&newMess2,sizeof(int),0)==-1){
                        perror("Blad wyslania wiadomosci CZYTAJ.\n");
                        exit(1);
                }

		newMess.mtype=PUSTY;
        	newMess.mvalue=save;

        	if(msgsnd(msgID,&newMess,sizeof(int),0)==-1){
                	perror("Blad wyslania wiadomosci PUSTY.\n");
                	exit(1);
        	}
	}
}


