#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>
#include <sys/syscall.h>
#define ZERO 0
#define PATH_UZUNLUK 255
#define DEBUG 0
#define FILE_NAME "log.txt"

#define FILE_OLUSAN "linenumber.txt"
#define FILE_OLUSANTHREAD "maxthreadnumber.txt"
#define TOGETHER_THREAD "ayni.txt"
#define FORCASCADE "cascadeNum.txt"
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct{

    char kelime[PATH_UZUNLUK];
    char path[PATH_UZUNLUK];
    char file[PATH_UZUNLUK];

}myThread;


int threadSayisiHesapla();
/* fonksiyona yollanan kelimenin uzunlugunu bulur.                           */
int findSizeString(char* kelime);
/* bu fonksiyon icerisinde yollanan parametre adi ile dosya acilir           */
/* dosyadaki karakter sayisi bulunur ve karakter sayisi kadar uzunlugu olan  */
/* array olusturulur ve dosyadaki karakterler arraye yerlestirilir           */
void TakeToStringFromFile(char fileName[], char *array);
/* Bu Dosyadaki karakter sayısını bulur                                      */
int sayacBul(char *fileName);
/* konsoldan girilen stringin dosya icerisinde kac defa gectigini            */
/* ve başlangıc satir ve sutununu ekrana yazar                               */
void *howManyTimesAndWhere(void *myParam);

void ReadStringFromDirectories(char *gelenString, char *directory);


void okumaFonksiyonu(int secim);
/*  Bu wait fonksiyonu ders kitabımızdan alinmistir */
/* Unix Systems Programming , Communications, Concurrency and Threads-Kay A.Robins-Steven Robins */
/* Program 3.3 r_wait.c */
pid_t r_wait(int *stat_loc);
static int toplamSatirSayisi=0;
static int number_thread=0;
static int olusanKlasor=1;
static int dosyaSayisiBulan=0;
static int ayniAndaThread=0;
static int cascadeDegiskeni=0;
static int sayac = 0;
FILE *inpForLine;

FILE *inpForMaxThread;

FILE *degiskenForCascade;
FILE* inp;
clock_t bitis;
 clock_t baslangic;
 double gecenTime;


int chooseOne=1;
int chooseTwo=0;
int chooseThree=2;
int chooseFour=3;
int chooseFive=4;
int chooseSix=5;

static void Signal_Handler(int signo){

	bitis=clock();
	gecenTime=(double)(bitis-baslangic) /CLOCKS_PER_SEC;
	if(signo==SIGINT){
		 while (r_wait(NULL) > 0); 

		 perror("CTRL-C GELDİ!!PROGRAMDAN CİKİLDİ");




	fclose(inpForLine);
	fclose(inpForMaxThread);
	fclose(degiskenForCascade);
	fclose(inp);
   	okumaFonksiyonu(chooseOne);
   	okumaFonksiyonu(chooseTwo);
   	okumaFonksiyonu(chooseFive);

   	fprintf(stderr, "Total number of strings found: %d\n", sayac);
   	fprintf(stderr, "Number of directories searched: %d\n", olusanKlasor);
   	fprintf(stderr, "Number of files searched: %d\n", dosyaSayisiBulan);
   	fprintf(stderr, "Number of lines searched: %d\n", toplamSatirSayisi);
   	okumaFonksiyonu(chooseSix);
   	fprintf(stderr, "Number of search threads created: %d\n", number_thread);
   	fprintf(stderr, "Max # of threads running concurrently: %d\n",ayniAndaThread );
   	fprintf(stderr, "Total run time, in miliseconds:: %f\n", gecenTime);
   	
   	remove(FILE_OLUSAN);
   	remove(TOGETHER_THREAD);
   	remove(FILE_OLUSANTHREAD);
   	remove(FORCASCADE);


   	exit(0);

	}


}
int main(int argc, char** argv) {
	

	struct sigaction act;
    char *instruction1 = "./grepTh";
    char *instruction2 = "./exe";
    int arttir = 0, i = 0;
    char karakter;
    



    act.sa_handler= Signal_Handler;

    act.sa_flags=0;

    if( (sigemptyset(&act.sa_mask)==-1) || (sigaction(SIGINT, &act,NULL)==-1)  ){

    	fprintf(stderr, "Sinyal Handler edilemedi !!! \n" );
    	exit(ZERO);


    }
    if (argc != 3) {

        printf("Usage: exe string filename\n ");
        return -1;
    }

    if (findSizeString(argv[1]) <= 0) {

        perror("String is empty.\n");
        return -1;
    }


        for (i = 0; i < strlen(argv[0]); ++i) {
            if (instruction1[i] == argv[0][i]) {
                ++arttir;
            }
        }
        if (arttir != strlen(argv[0])) {
            arttir=0;
            for (i = 0; i < strlen(argv[0]); ++i) {
                if (instruction2[i] == argv[0][i]) {
                    ++arttir;
                }
            }
            if(arttir!=strlen(argv[0])){
                printf("listdir ya da exe olmalı\n");
                return -1;
            }
        }
    


    remove(FILE_NAME);
    remove(FILE_OLUSAN);
    remove(TOGETHER_THREAD);
    remove(FILE_OLUSANTHREAD);
    remove(FORCASCADE);
    baslangic=clock();
    ReadStringFromDirectories(argv[1], argv[2]);
    bitis=clock();

    gecenTime=(double)(bitis-baslangic)/ CLOCKS_PER_SEC;

    inp = fopen(FILE_NAME, "r");

    if (inp == NULL) {
        printf("dosya acilamadi");
        return -1;


    }

    karakter = fgetc(inp);
    while (karakter != EOF) {
        if (karakter == '\n') {
            sayac++;
        }
        karakter = fgetc(inp);
    }
    fclose(inp);
    
    inp = fopen(FILE_NAME, "a");
    fprintf(inp, "%d %s were found in total.", sayac,argv[1]);

    


    fclose(inp);
    
   okumaFonksiyonu(chooseOne);
   okumaFonksiyonu(chooseTwo);
   okumaFonksiyonu(chooseFive);

   fprintf(stderr, "Total number of strings found: %d\n", sayac);
   fprintf(stderr, "Number of directories searched: %d\n", olusanKlasor);
   fprintf(stderr, "Number of files searched: %d\n", dosyaSayisiBulan);
   fprintf(stderr, "Number of lines searched: %d\n", toplamSatirSayisi);
   okumaFonksiyonu(chooseSix);
   fprintf(stderr, "Number of search threads created: %d\n", number_thread);
   fprintf(stderr, "Max # of threads running concurrently: %d\n",ayniAndaThread );
   fprintf(stderr, "Total run time, in miliseconds:: %f\n", gecenTime);
    
    remove(FILE_OLUSAN);
    remove(TOGETHER_THREAD);
    remove(FILE_OLUSANTHREAD);
    remove(FORCASCADE);
    return 0;
}

void ReadStringFromDirectories(char *gelenString, char *directory) {


    myThread myThreadArray[PATH_UZUNLUK];
    pthread_t oneThread[PATH_UZUNLUK];

    int isimUzunlugu = 0,i=1;
    DIR *direc = NULL; /* Klasör */
    /* yeni path olusturmak icin bir array tutariz */
    char yeniYolArray[PATH_UZUNLUK];

    struct dirent *direntpoint = NULL; /* Directory entry */

    struct stat tempstat; /* Stat fonksiyonu icin gerekli */

    pid_t childProcess;

    direc = opendir(directory);

    if (direc == NULL && errno == ENOTDIR) {

        printf("Klasör acilamadi!!\n ");

        exit(0);
    }

    /* Büyük temel  dongumuz recursive bu dongunun icinde yapilacak */
    strcpy(yeniYolArray, directory);

    while ((direntpoint = readdir(direc)) != NULL) {

        isimUzunlugu = strlen(direntpoint->d_name);

        /* ignore edilmesi gereken ozel . ve .. dosyalari vardir ve sonu ~ ile*/
        /* biten dosyalar vardir.bu if kosulunda onlarin kontrolü yapilir     */
        /* eger gelen o dosyalardan biri degilse yolumuz cizilmeye baslanir */
        if ((strcmp(direntpoint->d_name, "..") != 0) &&(direntpoint->d_name[0] != '.')
                && (direntpoint->d_name[isimUzunlugu - 1] != '~')) {
            strcpy(yeniYolArray, directory);
            strcat(yeniYolArray, "/");
            strcat(yeniYolArray, direntpoint->d_name);
        }

        /* stat fonksiyonu cagirimi yapilir asagidaki 
         S_ISDIR ve S_ISREG kullanimi icin stat cagirilir*/
        stat(yeniYolArray, &tempstat);

        /* Eger klasor bulursa  bu kosulda islem yapilacak   */
        /* S_ISDIR klasor olup olmadigini kontrol eder ve dogru ise 1 return eder*/
        if (S_ISDIR(tempstat.st_mode) && (direntpoint->d_name[0] != '.'
                && direntpoint->d_name[isimUzunlugu - 1] != '~'
                && strcmp(direntpoint->d_name, "..") != 0)) {

        	olusanKlasor++;
            childProcess = fork();
            /*  Fork basarisiz ise                        */
            if (childProcess < 0) {
                printf("Hatali FORK \n");
                exit(ZERO);
            }                /* cocuk process */
            else if (childProcess == 0) {
			closedir(direc);
                /* olen child process konntrolu icin */
                if (DEBUG == 1) {
                    printf("File parent: %d, File child: %d\n", getppid(), getpid());
                }
                cascadeDegiskeni=0;
                /*   Recursive cagrisi yapilir            */
                ReadStringFromDirectories(gelenString, yeniYolArray);
                degiskenForCascade=fopen(FORCASCADE,"a");
                fprintf(degiskenForCascade,"%d ",cascadeDegiskeni);
                fclose(degiskenForCascade);
                exit(ZERO);

            }
            else{
			    /* Her dosya ve klasor icin fork ile olusturulan  processlerin olmesi 
			     beklenir */
			    while (r_wait(NULL) > 0);            	
            }

        }            /* Eger dosya bulur ise bu kosulda islem yapacak */
            /* S_ISREG dosya olup olmadigini kontrol eder     */
            /* dogru ise 1 return eder */
        else if (S_ISREG(tempstat.st_mode) && (direntpoint->d_name[0] != '.' &&
                direntpoint->d_name[isimUzunlugu - 1] != '~'
	                && strcmp(direntpoint->d_name, "..") != 0)) {

        	cascadeDegiskeni=cascadeDegiskeni+1;
                /* dosyanin icinde girilen stringin kac adet oldugu ve     */
                /* hangi konumda bulundugunu bulan gecen ödevde yazdıgımız */
                /* fonksiyon cagirilir                                     */
        		number_thread=threadSayisiHesapla();
                sprintf(myThreadArray[number_thread].kelime,"%s",gelenString);
                sprintf(myThreadArray[number_thread].path,"%s",yeniYolArray);
                sprintf(myThreadArray[number_thread].file,"%s",direntpoint->d_name);
                pthread_create(&(oneThread[number_thread]), NULL, howManyTimesAndWhere, &(myThreadArray[number_thread]));
                okumaFonksiyonu(chooseThree);
            
            
        }

    } /* Dongu sonuu */



    while(i<=number_thread){

        pthread_join(oneThread[i],NULL);

        ++i;
    }
    /* olen child process konntrolu icin */
    if (DEBUG == 1) {
        printf("File parent: %d, File child: %d\n", getppid(), getpid());
    }
    /* direc kapatilir*/
    closedir(direc);

}

/* fonksiyona yollanan kelimenin uzunlugunu bulur.                           */
int findSizeString(char* kelime) {
    int sayac;
    for (sayac = 0; kelime[sayac] != '\0'; ++sayac);

    return sayac;
}

int sayacBul(char *fileName) {
    int sayac = 0;
    char karakter;
    FILE *inp = fopen(fileName, "r");
    if (inp == NULL) {
        printf("Cannot open file : %s\n", fileName);
        exit(1);
    }
    /* ilk karakter EOF ise bu dosya bos bir dosyadir ekrana hata basar     */
    /* Degilse karakter sayisini bul                                        */
    karakter = fgetc(inp);
    
    if (karakter != EOF) {
        sayac++;
    }
    while (karakter != EOF) {

        karakter = fgetc(inp);
        if (karakter != EOF) {
            ++sayac;
        }
    }
    fclose(inp);
    return sayac;
}

/* bu fonksiyon icerisinde yollanan parametre adi ile dosya acilir           */
/* dosyadaki karakter sayisi bulunur ve karakter sayisi kadar uzunlugu olan  */

/* array olusturulur ve dosyadaki karakterler arraye yerlestirilir           */
void TakeToStringFromFile(char fileName[], char *array) {
    char karakter;
    int sayac = 0, i = 0;
    /* dosyamizi actik                                                       */
    FILE *inp = fopen(fileName, "r");
    /*Dosya olusturulamadiysa                                                */
    if (inp == NULL) {
        printf("Cannot open file : %s\n", fileName);
        exit(1);
    }

    karakter = fgetc(inp);
    if (karakter != EOF) {
        sayac++;
    }
    /* okunan karakter kadar sayac artar yani karakter sayisini bulur       */
    while (karakter != EOF) {

        karakter = fgetc(inp);
        if (karakter != EOF) {
            ++sayac;
        }
    }
    /* dosya kapatilir                                                      */
    fclose(inp);
    /* Dosya tekrar acilir                                                  */
    inp = fopen(fileName, "r");

    karakter = fgetc(inp);
    /* while dongusu ile dosyadan okunan karaktcerler tek tek arraye 	    */
    /* yerlestirlir                                                         */
    while (karakter != EOF) {

        array[i] = karakter;
        karakter = fgetc(inp);
        ++i;
    }
    /* Acilan dosya kapatilir                                               */
    fclose(inp);
}

/*void howManyTimesAndWhere(char *kelime, char *path, char *file) */

void *howManyTimesAndWhere(void *myParam){


    myThread *oneThread=(myThread *)myParam;
    FILE *inp = fopen(FILE_NAME, "a");
    FILE *inpFor=fopen(FILE_OLUSAN,"a");
    int sizeOfWord;
    char *tempArray = NULL;
    char *array = NULL;
    int sayac;
    int satir = 1, sutun = 1, i = 0, j = 0;
    int eslesenKelime = 0, eslesenHarf = 0;
    int temporary = 0, gecici = 0;
    pid_t threadId=syscall(SYS_gettid);

    sayac = sayacBul(oneThread->path);
    sizeOfWord = findSizeString(oneThread->kelime);
    /* arguman olarak girilen stringin boyu uzunlugunda tempArray olusturulur*/
    tempArray = malloc(sizeOfWord * sizeof (char));
    /* dosyadaki karakter sayisi kadar array icin malloc ile yer ayrilir*/
    array = malloc(sayac * sizeof (char));


    /*okudugumuz dosyadaki bilgiler malloc ile olusturdugumuz arraye yerlestirilir*/
    TakeToStringFromFile(oneThread->path, array);

    for (j = temporary; j <= sayac - sizeOfWord - 1; ++j, ++sutun) {

        gecici = j;
        /* \n gordukce satir arttir                                         */
        if (array[j] == '\n') {
            ++satir;
            sutun = 0;

        }
        for (i = 0; i < sizeOfWord; ++i) {

            tempArray[i] = array[gecici];
            gecici++;

            /* ilk karakter \n  bosluk veya tab ise es gec diger karakteri   */
            /* al tempArraye                                                 */
            /* tempArraydeki ilk karakter \n ise bu alt satira girilmis      */
            /* demektir sutunu sifirla satira dokunma satir kontrolu yukarda */
            if (tempArray[0] == '\n' || tempArray[0] == ' ' || tempArray[0] == '\t') {

                tempArray[i] = array[gecici];

                if (tempArray[0] == '\n') {
                    sutun = 0;
                }

            } else if ((i != 0 && (tempArray[i] == '\n' || tempArray[i] == ' ' || tempArray[i] == '\t'))) {

                tempArray[i] = array[gecici];
                gecici++;

                while (tempArray[i] == '\n' || tempArray[i] == ' ' || tempArray[i] == '\t') {

                    tempArray[i] = array[gecici];
                    gecici++;

                }
            }

        }
        /* tempArraye alinan karakterler girilen argumana esit oldukca      */
        /* eslenenHarf degiskenizimin sayisi artar                          */
        for (i = 0; i < sizeOfWord; ++i) {

            if (tempArray[i] ==oneThread->kelime[i]) {

                ++eslesenHarf;
            }
        }

        /* Eger eslenenHarf arguman olarak girilen stringin boyuna esitse   */
        /* bu eslenenKelime sayisini arttirmamiz gerektigini gosterir*/
        if (eslesenHarf == sizeOfWord) {

            ++eslesenKelime;
        
            pthread_mutex_lock(&myMutex);
            fprintf(inp, "%d - %d %s: [%d,%d] %s first character is found.\n",getpid(),threadId, oneThread->file, satir, sutun,oneThread->kelime);
            pthread_mutex_unlock(&myMutex);
        }

        temporary++;
        eslesenHarf = 0;
    }
     pthread_mutex_lock(&myMutex);

    fprintf(inpFor,"%d\n",satir);
    pthread_mutex_unlock(&myMutex);
    fclose(inpFor);
     free(tempArray);
    free(array);
  
    fclose(inp);
    
    okumaFonksiyonu(chooseFour);
    pthread_exit(NULL);
}

/*  Bu wait fonksiyonu ders kitabımızdan alinmistir */
/* Unix Systems Programming , Communications, Concurrency and Threads-Kay A.Robins-Steven Robins */

/* Program 3.3 r_wait.c */
pid_t r_wait(int *stat_loc) {
    int retval;
    while (((retval = wait(stat_loc)) == -1) && (errno == EINTR));
    return retval;
}

void okumaFonksiyonu(int secim){
	FILE* dosyaDegiskeni;
	int gecici=0;
	int a=0;
	char karakter='a';
	int toplamTread=0;
	int kontrol=0;

	if(secim==1){

		    inpForLine = fopen(FILE_OLUSAN, "r");

		    if (inpForLine == NULL) {
		        printf("dosya acilamadi");
		        
		        exit(ZERO);

		    }

		    a=fscanf(inpForLine,"%d",&gecici);

		    while (a>0) {
		       	   
		          toplamSatirSayisi +=gecici;
		          a=fscanf(inpForLine,"%d",&gecici);

		    }
		    fclose(inpForLine);
	}
	else if(secim==2){

		dosyaDegiskeni=fopen(TOGETHER_THREAD,"a");
		   if (dosyaDegiskeni == NULL) {
		        printf("dosya acilamadi");
		        
		        exit(ZERO);

		    }
		fprintf(dosyaDegiskeni,"%c ",'b');
		fclose(dosyaDegiskeni);


	}
	else if(secim==3){

		dosyaDegiskeni=fopen(TOGETHER_THREAD,"a");
		   if (dosyaDegiskeni == NULL) {
		        printf("dosya acilamadi");
		        
		        exit(ZERO);

		    }
		fprintf(dosyaDegiskeni,"%c ",'s');
		fclose(dosyaDegiskeni);

	}
	else if(secim==4){

		dosyaDegiskeni=fopen(TOGETHER_THREAD,"r");

		   if (dosyaDegiskeni == NULL) {
		        printf("dosya acilamadi");
		        
		        exit(ZERO);

		    }


		    kontrol=fscanf(dosyaDegiskeni,"%c",&karakter);

		    while(kontrol>0){


		    		if(karakter=='b'){

		    			ayniAndaThread=ayniAndaThread+1;

		    		}
		    		else if(karakter=='s'){


		    			ayniAndaThread=0;


		    		}

		    		if(ayniAndaThread>toplamTread){

		    			toplamTread=ayniAndaThread;



		    		}

		    	   kontrol=fscanf(dosyaDegiskeni,"%c",&karakter);



		    }

		    ayniAndaThread=toplamTread;
		    fclose(dosyaDegiskeni);

	}
	else if(secim==5){

		dosyaDegiskeni=fopen(FORCASCADE,"r");

		   if (dosyaDegiskeni == NULL) {
		        printf("dosya acilamadi");
		        
		        exit(ZERO);

		    }

 			a=fscanf(dosyaDegiskeni,"%d",&gecici);

		    while (a>0) {
		       	
		       	kontrol+=gecici;    
		        fprintf(stderr, "Number of cascade threads created: %d\n",gecici ); 
		        a=fscanf(dosyaDegiskeni,"%d",&gecici);

		    }

		    fprintf(stderr, "Number of cascade threads created: %d\n",dosyaSayisiBulan-kontrol ); 
		    fclose(dosyaDegiskeni);


	}
	else{


    inpForLine = fopen(FILE_OLUSAN, "r");

    if (inpForLine == NULL) {
        printf("dosya acilamadi");
        exit(ZERO);


    }

    karakter = fgetc(inpForLine);
    while (karakter != EOF) {
        if (karakter == '\n') {
            dosyaSayisiBulan=dosyaSayisiBulan+1;
        }
        karakter = fgetc(inpForLine);
    }
    fclose(inpForLine);




	}



}


int threadSayisiHesapla(){

	int threadSayisi=0;
	char karakter='a';
	char karakter2='b';
	inpForMaxThread=fopen(FILE_OLUSANTHREAD,"a");


	fprintf(inpForMaxThread,"%c\n",karakter);

	fclose(inpForMaxThread);

	inpForMaxThread=fopen(FILE_OLUSANTHREAD,"r");

    karakter2 = fgetc(inpForMaxThread);
    while (karakter2 != EOF) {
        if (karakter2 == '\n') {
          threadSayisi=threadSayisi+1;
        }
        karakter2 = fgetc(inpForMaxThread);
    }
    fclose(inpForMaxThread);

    return threadSayisi;

}