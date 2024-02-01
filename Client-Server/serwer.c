#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>

#include <unistd.h>
#include <string.h>


//maksymalny rozmiar wiadomosci
#define MAX 80
#define SERWER 1  // typ komunikatu do serwera
char temp[15];
int wezpid(char[]);//odbiera pid procesu
void sig_hand(int);
//struktura komunikatu
struct komunikat {
        long mtype;
        char mtext[MAX];
};
int main(int argc, char *argv[])
{
    char *text;
    int msize;
    int i,pid;
    key_t key;  //unikalny klucz kolejki komunikatow
    int IDkolejki;  //identyfikator kolejki
    struct komunikat kom;  //przesylany komunikat
    //tworzenie unikalnego klucza urzadzenia IPC dla kolejki komunikatow
    key = ftok(".", 33);
    if( (IDkolejki = msgget(key, IPC_CREAT | 0660)) == -1)       //tworzenie kolejki
    {
        perror("msgget()");
        exit(1);
    }
    signal(SIGCLD,SIG_IGN);
    signal(SIGINT,sig_hand);	//po nacisnieciu przez uzytkownika CTRL+C wywoluje sie funkcja sig_hand()
    printf("^C konczy prace serwera\a\n");
    sleep(1);
    printf("\a");
    while(1)
    {
    printf("S: Czekam na komunikat...\n");
    kom.mtype = SERWER;//odczytuje z kolejki serwera-typ 1
    msgrcv(IDkolejki, (struct msgbuf *)&kom, MAX, kom.mtype, 0);
    printf("S: Odebrano od: %s\n", kom.mtext);
    //przetwarzamy wiadomosc
    msize = strlen(kom.mtext);
    //text = malloc(sizeof(char) * msize);
    for(i=0; i<msize; i++)
    {
      //kom.mtext[i] -= 32; //zamiana liter na duze
      kom.mtext[i]=toupper(kom.mtext[i]);
    }
    pid=wezpid(kom.mtext);
    //wysylanie wiadomosci
    kom.mtype = pid;
    printf("S: Wysylanie... %s -> %ld\n", kom.mtext, kom.mtype);
    msgsnd(IDkolejki, (struct msgbuf *)&kom, strlen(kom.mtext)+1, 0);
    }
}
int wezpid(char text[MAX])
{
 int i,pid,len,oldi;
 len=strlen(text);
	for(i=0;i<12;i++)
	{
		temp[i]=text[i];
		if(temp[i]=='~')
		{
			temp[i+1]='\n';
			break;
		}
	}
  oldi=i;
  for(i=0;i<len-oldi;i++)
  {
  	text[i]=text[i+1+oldi];
  }
  pid=atoi(temp);
  return pid;
}
void sig_hand(int sig_n)
{
  key_t key;
  int IDkolejki;
  if((sig_n==SIGTERM) || (sig_n==SIGINT))
  {
    printf("SIGTERM\n");
    key = ftok(".", 33);
    //tworzenie kolejki
    IDkolejki = msgget(key, IPC_CREAT | 0660);
    //usuwanie
    msgctl(IDkolejki, IPC_RMID, 0);
    exit(0);
  }
}
