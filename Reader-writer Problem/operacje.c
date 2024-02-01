#include "operacje.h"


int alokujSemafor(key_t klucz, int number, int flagi)
{
   int semID;
   if ( (semID = semget(klucz, number, flagi)) == -1)
   {
      perror("Blad semget (alokujSemafor): ");
      exit(1);
   }
   return semID;
}


void inicjalizujSemafor(int semID,int number,int val){
	if(semctl(semID,number,SETVAL,val)==-1){
		perror("Blad semctl(inicjalizujSemafor).\n");
		exit(1);
	}
}

int zwolnijSemafor(int semID,int number){
	return semctl(semID,number,IPC_RMID,NULL);
}

int waitSemafor(int semID,int number,int flags){
	int result;
	struct sembuf operacje[1];
	operacje[0].sem_num=number;
	operacje[0].sem_op=-1;
	operacje[0].sem_flg = 0| flags;

	if(semop(semID,operacje,1)==-1){
		printf("Blad waitSemafor.\n");
		return -1;
	}
	return 1;
}

void signalSemafor(int semID,int number){
	struct sembuf operacje[1];
	operacje[0].sem_num=number;
	operacje[0].sem_op=1;
	if(semop(semID,operacje,1)==-1){
		printf("Blad signalSemafor.\n");
	}
	return;
}
