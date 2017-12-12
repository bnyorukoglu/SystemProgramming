#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#define TWO 2
#define FIFO_WRITE (O_CREAT | O_WRONLY)
#define ZERO 0
#define FIFO_READ (O_CREAT | O_RDONLY)
#define MAX_SIZE 400
#define DEBUG 0
char commonMainFifo[255]; /*Ana fifo adini saklamak icin olusturulmus global array */

int  openFifo;
int openCLientFifo;
pid_t timerServerPid;
static int logDegiskeni=1;
char arrayForCreateClientFifo[255];
float kopyaArray1[25][25];
float tersBas[25][25];
float yedek1[25][25];
float yedek2[25][25];
float yedek3[25][25];
float yedek4[25][25];
float global1[25][25];
float copyForDortluTers[25][25];
int kernel[3][3]={{1,1,-1},{-1,1,-1},{1,1,1}};
float sonucArray[25][25];
int static flag=1;
float tersiAlinmisArray[25][25];

FILE *myLog;

void createFifo(char *fifoName);
void readFifoOrwriteFifo(int fd,int *location, int choose);
void cofactor(float num[25][25], float f);
void transpose(float num[25][25], float fac[25][25], float r);
float determinant(float a[25][25], float k);

void fonkDortluParcaTersi(float array[25][25],int size);
void TwoDConvolutionMatris(float myCopyArray[25][25],int matrixSize,int kCols,int kRows);

void matrixLogFiles(float **myMatrix,int matrisBoyut,int choose);
int kill(pid_t pid,int signal);

static void MySignal(int signo) {
	

		if(signo==SIGINT){
			
			if(DEBUG==0){


			fprintf(stderr,"CLIENT'A CTRL C GELDİ.....\n");

			}
			
			kill(timerServerPid,SIGUSR1);

			exit(ZERO);
		}
		else if(signo==SIGHUP){

			if(DEBUG==1){


				fprintf(stderr, "SERVER KAPANIYOR!!!!\n");

			}
			
			exit(ZERO);
		}
}

int main(int argc, char  **argv)
{
	int i=0,j=0;
	int myPidOfClient=0;
	
	int matrisBoyut=0;
	float orjinalDet=0;
	float digerDet=0;
	float digerDet2=0;
	float sonuc1=0;
	float sonuc2=0;
	float **myMatrix=NULL;
	float sentMatrix[MAX_SIZE];
	float copyForConvolution[25][25];
	char logArray[MAX_SIZE];
	pid_t pid;					/* to use on fork() */
	int checkIndex=0; /* serverdan gelen matrisi client matrisine tasimak icin */

	struct sigaction act;
	act.sa_handler = MySignal;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask,SIGINT);
	sigaddset(&act.sa_mask,SIGUSR2);
	sigaddset(&act.sa_mask,SIGHUP);
	myPidOfClient=getpid();

	if ((sigaction(SIGUSR2, &act, NULL) == -1) || (sigaction(SIGINT, &act, NULL) == -1) || (sigaction(SIGHUP, &act, NULL) == -1)) 
	{
		perror("Failed to set Signal handler");
		exit(0);
	}


	if(argc!= TWO){

		fprintf(stderr, " Eksik veya Yanlis sayida arguman girdiniz !!!! \n");
	}

	sprintf(commonMainFifo, "%s" , argv[1]);
	createFifo(commonMainFifo);
	openFifo= open(commonMainFifo,FIFO_WRITE);

	if(openFifo== -1){

		perror("seeWhat Fifo acilamadi !!!");
		exit(ZERO);
	}


	fprintf(stderr, "%d\n",myPidOfClient);

	readFifoOrwriteFifo(openFifo,&myPidOfClient,1);

	sprintf(arrayForCreateClientFifo,"%d",getpid());

	createFifo(arrayForCreateClientFifo);

	openCLientFifo=open(arrayForCreateClientFifo,FIFO_READ);

	/*-----------------matris boyutu al ------------*/
	readFifoOrwriteFifo(openCLientFifo,&matrisBoyut,2);


	myMatrix=(float**)malloc(matrisBoyut*sizeof(float*));

	while(i < matrisBoyut){


		myMatrix[i] = (float*)malloc(matrisBoyut*sizeof(float)); 

		++i;
	}

	/*-----------------server pidini clienta al ------------*/
	read(openCLientFifo,&timerServerPid,sizeof(pid_t));



	while(1){
		/*-----------------matrisi serverdan al ------------*/


		sprintf(logArray, "%d. islemLog" , logDegiskeni);
		myLog=fopen(logArray,"a");

		logDegiskeni++;


		read(openCLientFifo,sentMatrix,MAX_SIZE);
		checkIndex=0;
		i=0;
	    while (i<matrisBoyut){
	    	j=0;
	       while (j<matrisBoyut){
	           myMatrix[i][j]=sentMatrix[checkIndex];
	           ++checkIndex;
	           j++;
	       }
	       i++;
	    }
	    if(flag==1)
	    	matrixLogFiles(myMatrix,matrisBoyut,1);
			
			/*-----------------------------------------------------------*/
		        /*---------------MATRİSİ KOPYALA Convolution icin-----------------------------*/
				    for (i=0;i<matrisBoyut;i++){

				       for (j=0;j<matrisBoyut;j++){

				           copyForConvolution[i][j]=myMatrix[i][j];
				           copyForDortluTers[i][j]=myMatrix[i][j];
				       }
				      
				    }
		
	    orjinalDet=determinant(kopyaArray1,matrisBoyut);

	    fonkDortluParcaTersi(copyForDortluTers,matrisBoyut);
	    digerDet=determinant(tersiAlinmisArray,matrisBoyut);
	    TwoDConvolutionMatris(copyForConvolution,matrisBoyut,3,3);
	    cofactor(kopyaArray1,matrisBoyut);
	    if(flag==1){
	    	 matrixLogFiles(myMatrix,matrisBoyut,2);
	 	   matrixLogFiles(myMatrix,matrisBoyut,3);

	    }

	    orjinalDet=determinant(kopyaArray1,matrisBoyut);

	    digerDet2=determinant(sonucArray,matrisBoyut);

	
	    fclose(myLog);
	   /* printf("%f \n",sonuc2); */
	    flag=0;

		if((pid = fork()) == -1)
		{
			printf("Error : server cannot create fork!");
			exit(1);
		}



		else if(pid == 0)
		{

	    
			sonuc1=orjinalDet-digerDet;
			 
			 fprintf(stderr, "%f\n",sonuc1 );
			 exit(ZERO);

		}


		else{

			while(wait(NULL)!=-1);

		}


		if((pid = fork()) == -1)
		{
			printf("Error : server cannot create fork!");
			exit(1);
		}



		else if(pid == 0)
		{

	    
			sonuc2=orjinalDet-digerDet2;
			 
			 fprintf(stderr, "%f\n",sonuc2 );
			 exit(ZERO);

		}


		else{

			while(wait(NULL)!=-1);

		}


		flag=1;
}


return 0;
}
void matrixLogFiles(float **myMatrix,int matrisBoyut,int choose){

	int i,j;



	if(choose==1){

	    fprintf(myLog,"MY ORIGINAL MATRIX= \n");
	    fprintf(myLog,"[");
	        for (i=0;i<matrisBoyut;i++){

				       for (j=0;j<matrisBoyut;j++){

				           kopyaArray1[i][j]=myMatrix[i][j];
				           fprintf(myLog, "%f ", kopyaArray1[i][j]);
				       }
				      fprintf(myLog, "\n");
		   }

		fprintf(myLog,"]");
		fprintf(myLog,"\n");
		fprintf(myLog,"\n");


	}
	else if(choose==2){

	    fprintf(myLog,"MY CONVOLUTION MATRIX= \n");


	    fprintf(myLog,"[");
	        for (i=0;i<matrisBoyut;i++){

				       for (j=0;j<matrisBoyut;j++){

				          
				           fprintf(myLog, "%f ", sonucArray[i][j]);
				       }
				      fprintf(myLog, "\n");
		   }

		fprintf(myLog,"]");
		fprintf(myLog,"\n");

	}
	else{
		fprintf(myLog,"MY INVERSE MATRIX= \n");


	    fprintf(myLog,"[");
	        for (i=0;i<matrisBoyut;i++){

				       for (j=0;j<matrisBoyut;j++){

				          
				           fprintf(myLog, "%f ", tersBas[i][j] );
				       }
				      fprintf(myLog, "\n");
		   }

		fprintf(myLog,"]");
		fprintf(myLog,"\n");

	}




}
void createFifo(char *fifoName){

	if( (mkfifo(fifoName,0666)== -1) && (errno!=EEXIST) )
	{

		perror("  FİFO  olusturulamadi !!!! " );
		exit(ZERO);
	}


}
void readFifoOrwriteFifo(int fd,int *location, int choose){


	if (choose==1)
	{
		write(fd,location,sizeof(int));
	}
	else{
		read(fd,location,sizeof(int));
	}

}



void cofactor(float num[25][25], float f)
{
 float b[25][25], fac[25][25];
 int p=0, q=0, m=0, n=0, i=0, j=0;
 for (q = 0;q < f; q++)
 {
   for (p = 0;p < f; p++)
    {
     m = 0;
     n = 0;
     for (i = 0;i < f; i++)
     {
       for (j = 0;j < f; j++)
        {
          if (i != q && j != p)
          {
            b[m][n] = num[i][j];
            if (n < (f - 2))
             n++;
            else
             {
               n = 0;
               m++;
               }
            }
        }
      }
      fac[q][p] = pow(-1, q + p) * determinant(b, f - 1);
    }
  }
  transpose(num, fac, f);
}
/*Finding transpose of matrix*/ 
void transpose(float num[25][25], float fac[25][25], float r)
{
  int i=0, j=0;
  float b[25][25], inverse[25][25], d=0;
 
  for (i = 0;i < r; i++)
    {
     for (j = 0;j < r; j++)
       {
         b[i][j] = fac[j][i];
        }
    }
  d = determinant(num, r);
  for (i = 0;i < r; i++)
    {
     for (j = 0;j < r; j++)
       {
        inverse[i][j] = b[i][j] / d;
        }
    }


    for (i = 0;i < r; i++)
    {
     for (j = 0;j < r; j++)
       {
        global1[i][j] = inverse[i][j] ;
        tersBas[i][j]=inverse[i][j] ;
        }
    }
  
}


/*For calculating Determinant of the Matrix */
float determinant(float a[25][25], float k)
{
  float s = 1, det = 0, b[25][25];
  int i, j, m, n, c;
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


void TwoDConvolutionMatris(float myCopyArray[25][25],int matrixSize,int kCols,int kRows)

{
    int i,j,m,n;
    int mm,nn,ii,jj;

    int kCenterX,kCenterY;

    kCenterX = kCols / 2;
    kCenterY = kRows / 2;

    for(i=0; i < matrixSize; ++i)             
    {
        for(j=0; j < matrixSize; ++j)      
        {
            for(m=0; m < kRows; ++m)    
            {
                mm = kRows - 1 - m;      

                for(n=0; n < kCols; ++n) 
                {
                    nn = kCols - 1 - n;  

                    ii = i + (m - kCenterY);
                    jj = j + (n - kCenterX);

                    if( ii >= 0 && ii < matrixSize && jj >= 0 && jj < matrixSize )
                        sonucArray[i][j] += myCopyArray[ii][jj] * kernel[mm][nn];
                }
            }
        }
    }
}


void fonkDortluParcaTersi(float array[25][25],int matrisBoyut){


			int k=0,j=0,d=0,m=0,i=0;


				/*  ILK PARCAYİ AL  */
		    for(i=0 ; i< (matrisBoyut/2) ; ++i){

		    	for(m=0 ; m<(matrisBoyut/2) ; ++m){

		    			yedek1[i][m]=array[i][m];
		    	

		    	}
		    
		    	

		    }
		     d = determinant(yedek1, (matrisBoyut/2));
		  if (d == 0){
		  		if(DEBUG==1)
		  			 printf("\nInverse of Entered Matrix is not possible\n");
		  		for(i=0 ; i< (matrisBoyut/2) ; ++i){

			    	for(m=0 ; m<(matrisBoyut/2) ; ++m){

			    			tersiAlinmisArray[i][m]=yedek1[i][m];
			    			
			    	}
			    
		 	    }

		  }
		
		  else{

		  		cofactor(yedek1, (matrisBoyut/2));

		  		for(i=0 ; i< (matrisBoyut/2) ; ++i){

			    	for(m=0 ; m<(matrisBoyut/2) ; ++m){

			    			tersiAlinmisArray[i][m]=global1[i][m];
			    		

			    	}
			    
		  	    }


		  }

		    /*  ILK PARCA ALINDII  */

		    printf("\n\n\n\n");
		 	
		 	/* IKINCI PARCAYI AL  */
		 	for(i=0 ; i<(matrisBoyut/2) ; ++i){

		    	for(m=(matrisBoyut/2) ; m<matrisBoyut ; ++m){

		    			yedek2[i][m]=array[i][m];
		    			

		    	}
		    
		    }

		    d = determinant(yedek2, (matrisBoyut/2));
		  	if (d == 0){
		  		if(DEBUG==1)
		  	   		printf("\nInverse of Entered Matrix is not possible\n");


		  	   for(i=0 ; i<(matrisBoyut/2) ; ++i){

			    	for(m=(matrisBoyut/2) ; m<matrisBoyut ; ++m){

			    			tersiAlinmisArray[i][m]=yedek2[i][m];
			    			

			    	}
		   
		    	
		    	}

		  }
		
		  else{


		     cofactor(yedek2, (matrisBoyut/2));


		  	   for(i=0,j=0 ; i<(matrisBoyut/2) ; ++i,j++){

			    	for(m=(matrisBoyut/2),k=0 ; m<matrisBoyut ; ++m,k++){

			    			tersiAlinmisArray[i][m]=global1[j][k];
			    			 

			    	}
		    	
		    	
		    	}

		  }
		    /*  IKINCI PARCAYI ALINDI   */

		    printf("\n\n\n\n");


		    /* ÜCÜNCÜ PARCAYİ AL */
			for(i=(matrisBoyut/2),j=0; i<matrisBoyut ; ++i){

			    	for(m=0,k=0 ; m<(matrisBoyut/2) ; ++m){

			    			yedek3[i][m]=array[i][m];
			    			
			    	}
			    	 
		    }
		    d = determinant(yedek3, (matrisBoyut/2));
		  	if (d == 0){
		  		if(DEBUG==1)
		  		printf("\nInverse of Entered Matrix is not possible\n");
		  		 /* ÜCÜNCÜ PARCAYİ AL */
				for(i=(matrisBoyut/2); i<matrisBoyut ; ++i){

				    	for(m=0 ; m<(matrisBoyut/2) ; ++m){

				    			tersiAlinmisArray[i][m]=yedek3[i][m];
				    			

				    	}
			
			    }

		  }
		   
		  else{


		   		cofactor(yedek3, (matrisBoyut/2));


		       /* ÜCÜNCÜ PARCAYİ AL */
				for(i=(matrisBoyut/2),j=0; i<matrisBoyut ; ++i,j++){

			    	for(m=0,k=0; m<(matrisBoyut/2) ; ++m,k++){

			    			tersiAlinmisArray[i][m]=global1[j][k];
			    			

			    	}
			    	 
			    	
		   	    }
		  }
			/* ÜCÜNCÜ PARCA ALİNDİ */
		    printf("\n\n\n\n");

			    
			for(i=(matrisBoyut/2); i<matrisBoyut ; ++i){

				    	for(m=(matrisBoyut/2) ; m<matrisBoyut ; ++m){

				    			yedek4[i][m]=array[i][m];
				    			

				    	}
				    
				    	
		   }
		    d = determinant(yedek4, (matrisBoyut/2));
		  if (d == 0){
		  		if(DEBUG==1)
		  		  printf("\nInverse of Entered Matrix is not possible\n");

		  		  for(i=(matrisBoyut/2); i<matrisBoyut ; ++i){

				    	for(m=(matrisBoyut/2) ; m<matrisBoyut ; ++m){

				    			tersiAlinmisArray[i][m]=yedek4[i][m];
				    		

				    	}
				    	
				    	
		  		 }

		  }
		 
		  else{

		     cofactor(yedek4, (matrisBoyut/2));

		      for(i=(matrisBoyut/2),j=0; i<matrisBoyut ; ++i,j++){

				    	for(m=(matrisBoyut/2),k=0 ; m<matrisBoyut ; ++m,k++){

				    			tersiAlinmisArray[i][m]=global1[j][k];
				    		

				    	}
				    	
		       }


		  }


}