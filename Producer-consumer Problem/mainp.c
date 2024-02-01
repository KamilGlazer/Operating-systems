#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <signal.h>

#define P 12   // liczba  procesow producent i konsument
#define MAX 10 //rozmiar puli buforó
#define MAX2 12// rozmiar pamieci dzielonej
#define CZYTAJ 4
#define ZAPISZ 3
#define PUSTY 1 //typ komunikatu
#define PELNY 2 //typ komunikatu
//struktura komunikatu
struct bufor{
	long mtype;
	int mvalue;
};
int shmID,msgID;  //ID semafora, kolejki kom. pamieci dzielonej

//funkcja koniec -- do obslugi przerwania
void koniec(int sig)
{
   msgctl(msgID,IPC_RMID,NULL);
   shmctl(shmID,IPC_RMID, NULL);
   printf("MAIN - funkcja koniec sygnal %d: Koniec.\n",sig);
   exit(1);

}
int main()
{
   key_t klucz, kluczm;  //klucze do IPC
   int i;
   struct bufor komunikat;
   struct sigaction act;
   act.sa_handler=koniec;
   sigemptyset(&act.sa_mask);
   act.sa_flags=0;
   sigaction(SIGINT,&act,0);
   if ( (klucz = ftok(".", 'A')) == -1 )
   {
      printf("Blad ftok (main)\n");
      exit(1);
   }

   msgID=msgget(klucz,IPC_CREAT|IPC_EXCL|0666);
   if (msgID==-1)
	{printf("blad kolejki komunikatow\n"); exit(1);}

   kluczm=ftok(".",'B');
   shmID=shmget(kluczm, MAX2*sizeof(int), IPC_CREAT|IPC_EXCL|0666);
   if (shmID==-1) {printf("blad pamieci dzielonej\n"); exit(1);}


   komunikat.mtype=PUSTY;
//komunikat.mvalue=0;
   for(i=0;i<MAX;i++)
	 {
	if( msgsnd(msgID,&komunikat,sizeof(komunikat.mvalue),0)==-1)
		{
			printf("blad wyslania kom. pustego\n");
			exit(1);
		};
	   printf("wyslany pusty komunikat %d\n",i);
	}

	struct bufor komunikat2,komunikat3;
	komunikat2.mtype=ZAPISZ;
	komunikat3.mtype=CZYTAJ;

	if(msgsnd(msgID,&komunikat2,sizeof(komunikat2.mvalue),0)==-1){
		printf("blad wyslania kom. zapisz.\n");
		exit(1);
	}

	if(msgsnd(msgID,&komunikat3,sizeof(komunikat3.mvalue),0)==-1){
		printf("blad wyslania kom. czytaj.\n");
		exit(1);
	}

   for (i = 0; i < P; i++)
      switch (fork())
      {
         case -1:
            perror("Blad fork (mainprog)");
            exit(2);
         case 0:
            execl("./prod","prod", NULL);
      }

   for(i=0;i<P;i++)
	switch (fork())
	{
	case -1:
            printf("Blad fork (mainprog)\n");
	    exit(2);
	case 0:
		execl("./kons","kons",NULL);
	}

for(i=0;i<2*P;i++)
   wait(NULL);

   msgctl(msgID,IPC_RMID,NULL);
   shmctl(shmID,IPC_RMID, NULL);
   printf("MAIN: Koniec.\n");
}
