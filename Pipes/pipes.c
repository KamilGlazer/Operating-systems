#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(){
	fprintf(stderr,"MAIN\n");
	int potok[2];
	pipe(potok);
	int pdesk;

	int child_pid;
	child_pid=fork();

	if(child_pid==-1){
		exit(1);
	}else if(child_pid==0){
		fprintf(stderr,"...Tworzenie potoku FIFO - potomny 1.\n");
                        if(mkfifo("kFIFO",0777)==-1){
                                fprintf(stderr,"Blad mkfifo().\n");
                                exit(1);
                        }

			child_pid=fork();

		        if(child_pid==-1){
	                	        exit(1);
			}else if(child_pid==0){
	                        	fprintf(stderr,"...Wczytywanie liczby sekund - potomny 2.\n");
        	                        pdesk=open("kFIFO",O_WRONLY);

                                        time_t czas,seconds;
                                        time(&czas);
                                        fprintf(stderr,"Biezacy czas - %d\n",czas);


                                        fprintf(stderr,"Podaj liczbe sekund: ");
                                        scanf("%ld",&seconds);

                                        write(pdesk,&seconds,sizeof(seconds));
                                        close(pdesk);

                                        exit(1);
        	        }else{
                		        fprintf(stderr,"...Formatowanie czasu - potomny 1.\n");

                                        pdesk=open("kFIFO",O_RDONLY);
                                        time_t czas2;
                                        read(pdesk,&czas2,sizeof(czas2));
                                        close(pdesk);

                                        fprintf(stderr,"Odczytany czas z FIFO: %d\n",czas2);

                                        struct tm *timeInfo = gmtime(&czas2);

                                        fprintf(stderr,"\nMiesiac = %d\nDzien = %d\nDzien tygodnia = %d\nRok = %d\n",(timeInfo->tm_mon)+1,
								timeInfo->tm_mday,(timeInfo->tm_wday)+1,timeInfo->tm_year);

                                        int year = timeInfo->tm_year;
                                        //przesylanie przez potok!
                                        write(potok[1],&year,sizeof(year));
                                        close(potok[0]);
                                        close(potok[1]);

                                        exit(1);
            	       }
	}else{
		fprintf(stderr,"...Main\n");

		int year2;
		read(potok[0],&year2,sizeof(year2));

		close(potok[0]);
		close(potok[1]);


		//usuniecie lacza
		unlink("kFIFO");
		fprintf(stderr,"Tekst z potoku - %d.\n",year2+1900);
		exit(1);
	}
}
