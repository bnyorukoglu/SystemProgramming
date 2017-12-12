#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>

#define ZERO 0
#define FOUR 4
#define FIFO_WRITE (O_CREAT | O_WRONLY)
#define MAX_SIZE 400
#define DEBUG 0
#define SERVERLOG "serverlog.log"
char commonMainFifo[255]; /*Ana fifo adini saklamak icin olusturulmus global array */

int openFifo;
int openCLientFifo;
int clientPid;
int getClient[MAX_SIZE];
int countGetClient=0;
char arrayForCreateClientFifo[255];

void createFifo(char *fifoName);
float determinant(float a[25][25], float k);
void getClientFunction();
int kill(pid_t pid,int signal);


static void MySignal(int signo) {
	
	int i=0;


	 if(signo==SIGUSR2){
	
		fprintf(stderr, "---------CLIENTTAN SINYAL GELDİ----------\n");
	}
	else if(signo==SIGUSR1){

		if(DEBUG==1)
			fprintf(stderr, "SERVER KENDINI VE CLIENTLARI KAPATIYOR !!!\n");
		
		i=0;
		while( i < countGetClient)
		{	
			if(DEBUG==1){

				fprintf(stderr, "%d \n",getClient[i]);
			}
			kill(getClient[i],SIGHUP);
			++i;
		}		
		exit(ZERO);
	}
	else if(signo==SIGINT){

		fprintf(stderr, "SERVERDAN CTRL C GELDİ\n");
		i=0;
		while( i < countGetClient )
		{	
			if(DEBUG==1){

				fprintf(stderr, "%d \n",getClient[i]);
			}
			kill(getClient[i],SIGHUP);

			++i;
		}
		exit(ZERO);
	}
	
}

int main(int argc,char **argv){


	int i=0,j=0;
	int boyut=0;
	float det=0;

	float **myMatrix=NULL;
	FILE* logDosyam;

	pid_t timerServerPid;

	pid_t varFOrChild;

	float detMatrix[25][25];
	float sentMatrix[MAX_SIZE];
	struct sigaction act;

	
	 time_t curtime;
   struct tm *loc_time;
 	int checkIndex=0; /* serverdan gelen matrisi client matrisine tasimak icin */

   curtime = time (NULL);


	if(argc != FOUR){


		fprintf(stderr," Parametre sayisini yanlis girdiniz !!!!! ");
		exit(ZERO);
	}
	
	logDosyam=fopen(SERVERLOG,"a");
	
	
	act.sa_handler = MySignal;
	act.sa_flags = 0;

	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask,SIGINT);
	sigaddset(&act.sa_mask,SIGUSR2);
	sigaddset(&act.sa_mask,SIGHUP);
	
	if ((sigaction(SIGUSR2, &act, NULL) == -1) || (sigaction(SIGINT, &act, NULL) == -1) || (sigaction(SIGHUP, &act, NULL) == -1)) 
	{
		perror("Failed to set Signal handler");
		exit(0);
	}

	srand(time(NULL));
	boyut=atoi(argv[2])*2;
	/* server pid al loga yazdırrr */
	timerServerPid=getpid();


	/*-----------------matris alan alma------------*/
	myMatrix=(float**)malloc(boyut*sizeof(float*));

	while(i < boyut){


		myMatrix[i] = (float*)malloc(boyut*sizeof(float)); 

		++i;
	}
	/*-----------------------------------------------*/
	sprintf(commonMainFifo, "%s" , argv[3]);

	createFifo(commonMainFifo);

	openFifo = open( commonMainFifo, O_RDONLY ) ;

	if( openFifo == -1 ){

		perror(" FİFO okunmak icin acilamadi ");
		exit(ZERO);

	}
	

	while( read(openFifo , &clientPid , sizeof(int))> 0){


		
		/* KOntrol icin */


		/* FONKSIYONNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN*/

		if(DEBUG==1){
			fprintf(stderr, "%d\n", clientPid);
		}
		

		sprintf(arrayForCreateClientFifo,"%d",clientPid);

		openCLientFifo=open(arrayForCreateClientFifo,FIFO_WRITE);


		if(openCLientFifo==-1){


			perror("Fifo acilamadi");

			exit(ZERO);


		}

	

		if((varFOrChild = fork()) == -1)
			{
				perror("Error : server cannot create fork!");
				exit(ZERO);
			}

		/*child process */
		else if(varFOrChild==0){


			write(openCLientFifo,&boyut,sizeof(int));
		
			write(openCLientFifo,&timerServerPid,sizeof(pid_t));


			while(1){

					
				sleep(1);
						do{
  							 loc_time = localtime (&curtime);
						    for (i=0;i<boyut;i++){

						       for (j=0;j<boyut;j++){
						           myMatrix[i][j]=(int)(rand()%10);
						       }
						    }

						/*-----------------matris deneme ekrana bastırma ------------*/
						    for (i=0;i<boyut;i++){

						       for (j=0;j<boyut;j++){
						           printf("%.f ",myMatrix[i][j]);
						       }
						       printf("\n");
						    }
						/*-----------------------------------------------------------*/

						    for (i=0;i<boyut;i++){

						       for (j=0;j<boyut;j++){
						           detMatrix[i][j]=myMatrix[i][j];
						       }
						    }

						    det=determinant(detMatrix,boyut);

						}while(det==0);

						/* loga yazdııııııııııııııııır */
						det=determinant(detMatrix,boyut);

					    for (i=0;i<boyut;i++){

					       for (j=0;j<boyut;j++){
					           sentMatrix[checkIndex]=myMatrix[i][j];
					  
					           ++checkIndex;
					       }
					    }


						write(openCLientFifo,sentMatrix,MAX_SIZE);
						checkIndex=0;
						fprintf(logDosyam ,"%s ,", asctime (loc_time));
						fprintf(logDosyam,"timerServerPid: %d ,",timerServerPid);
						fprintf(logDosyam,"OLusturulan Matris determinant: %f\n",det);

			}

		} 
		/*	parent process	*/
		else{

			getClientFunction();
			

		}

	}
	while(wait(NULL)>0);


	return 0;
}

void createFifo(char *fifoName){

	if( (mkfifo(fifoName,0666)== -1)  )
	{

		perror("  FİFO  olusturulamadi !!!! \n " );
		exit(ZERO);
	}


}
void getClientFunction(){

	if(DEBUG==1){


		fprintf(stderr,"%d \n",clientPid);
	}


	getClient[countGetClient]=clientPid;
	++countGetClient;
}
/*For calculating Determinant of the Matrix */
float determinant(float a[25][25], float k)
{
  float s = 1, det = 0, b[25][25];
  int i=0, j=0, m=0, n=0, c=0;
  if (k == 1)
    {
     return (a[0][0]);
    }
  else
    {
     det = 0;
     for (c = 0; c < k; c++)
       {
        m = 0;
        n = 0;
        for (i = 0;i < k; i++)
          {
            for (j = 0 ;j < k; j++)
              {
                b[i][j] = 0;
                if (i != 0 && j != c)
                 {
                   b[m][n] = a[i][j];
                   if (n < (k - 2))
                    n++;
                   else
                    {
                     n = 0;
                     m++;
                     }
                   }
               }
             }
          det = det + s * (a[0][c] * determinant(b, k - 1));
          s = -1 * s;
          }
    }
 
    return (det);
}