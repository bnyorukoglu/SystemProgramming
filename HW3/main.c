#include<stdio.h>
#include<dirent.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include<sys/wait.h>
#include <fcntl.h>

#define ZERO 0
#define PATH_UZUNLUK 255
#define DEBUG 0
#define FILE_NAME "log.txt"




#define FIFO "myFifo"

int fifoSayac=0;


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
int howManyTimesAndWhere(char *kelime, char *path, char *file);

int ReadStringFromDirectories(char *gelenString, char *directory);
void fifoOLustur(int number);
/*  Bu wait fonksiyonu ders kitabımızdan alinmistir */
/* Unix Systems Programming , Communications, Concurrency and Threads-Kay A.Robins-Steven Robins */
/* Program 3.3 r_wait.c */
pid_t r_wait(int *stat_loc);
int readFifo(int *dirCheck, int fd);

int main(int argc, char** argv) {

    char *instruction1 = "./exe";
    char *instruction2 = "./withPipeandFIFO";
    int arttir = 0, i = 0;
    FILE*inp;

    int stringEslesme = 0;
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
    stringEslesme = ReadStringFromDirectories(argv[1], argv[2]);
    /*fprintf(stderr, "SON ESLESME: %d\n", stringEslesme);*/


    
    inp = fopen(FILE_NAME, "a");
    fprintf(inp, "%d %s were found in total.", stringEslesme, argv[1]);
    fclose(inp);


    return 0;
}

int ReadStringFromDirectories(char *gelenString, char *directory) {



    int isimUzunlugu = 0;
    DIR *direc = NULL; /* Klasör */
    /* yeni path olusturmak icin bir array tutariz */
    char yeniYolArray[PATH_UZUNLUK];

    struct dirent *direntpoint = NULL; /* Directory entry */

    struct stat tempstat; /* Stat fonksiyonu icin gerekli */

    pid_t childProcess;

    int myPipeArr[2];
    int eslesmeSayisi = 0;
    int pipeEslesme = 0;
    int checkPipe = 0;
    int toplamEslesme = 0;

    /*fifo file descriptor */
    int fd;

  
    int rdFifoCheck=0;
    /* fork kontrol*/
    int directoryCheck=0;
    
    
    int fifoWrite = 0;
    
    char myFifo[255];
    
    
    direc = opendir(directory);

    if (direc == NULL && errno == ENOTDIR) {

        printf("Klasör acilamadi!!\n ");

        exit(0);
    }

    /* Büyük temel  dongumuz recursive bu dongunun icinde yapilacak */


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


            sprintf(myFifo,"%d",fifoSayac);
            ++fifoSayac;
            mkfifo(myFifo, S_IRWXU);
            
            fd = open(myFifo, O_RDWR);



            childProcess = fork();
            ++directoryCheck;
            /*  Fork basarisiz ise                        */
            if (childProcess < 0) {
                printf("Hatali FORK \n");
                exit(ZERO);
            }/* cocuk process */
            
            
            else if (childProcess == 0) {

                /* olen child process konntrolu icin */
                if (DEBUG == 1) {
                    printf("File parent: %d, File child: %d\n", getppid(), getpid());
                }
                /*   Recursive cagrisi yapilir            */
                fifoWrite = ReadStringFromDirectories(gelenString, yeniYolArray);
                /* +1 OLACAK MI !!!!!*/
                write(fd, &fifoWrite, sizeof (int));
                close(fd);
                exit(ZERO);

            } else {

/*
                read(fd, &fifoRead, sizeof (int));
                toplamEslesme += fifoRead;
*/
                
                rdFifoCheck=readFifo(&directoryCheck,fd);
                toplamEslesme+=rdFifoCheck;
                unlink(myFifo);
                close(fd);

            }

        }/* Eger dosya bulur ise bu kosulda islem yapacak */
            /* S_ISREG dosya olup olmadigini kontrol eder     */
            /* dogru ise 1 return eder */
        else if (S_ISREG(tempstat.st_mode) && (direntpoint->d_name[0] != '.' &&
                direntpoint->d_name[isimUzunlugu - 1] != '~'
                && strcmp(direntpoint->d_name, "..") != 0)) {


            checkPipe = pipe(myPipeArr);
            if (checkPipe < 0) {

                printf("pipe olusmadi!!!\n");
                exit(ZERO);
            }
            childProcess = fork();
            if (childProcess < 0) {
                printf("Hatali FORK \n");
                exit(ZERO);
            }/* cocuk process            */
            else if (childProcess == 0) {
                /* olen child process konntrolu icin */
                if (DEBUG == 1) {
                    printf("File parent: %d, File child: %d\n", getppid(), getpid());
                }
                /* dosyanin icinde girilen stringin kac adet oldugu ve     */
                /* hangi konumda bulundugunu bulan gecen ödevde yazdıgımız */
                /* fonksiyon cagirilir                                     */
                eslesmeSayisi = howManyTimesAndWhere(gelenString, yeniYolArray, direntpoint->d_name);
                close(myPipeArr[0]);
                write(myPipeArr[1], &eslesmeSayisi, sizeof (int));



                exit(ZERO);
            } else {

                close(myPipeArr[1]);
                while (read(myPipeArr[0], &pipeEslesme, sizeof (int))>0) {
                    toplamEslesme += pipeEslesme;
                }
                /*fprintf(stderr, "matching:%d\n", toplamEslesme);*/
            }
        }

    } /* Dongu sonuu */

    /* Her dosya ve klasor icin fork ile olusturulan  processlerin olmesi 
     beklenir */
    while (r_wait(NULL) > 0);

    /* olen child process konntrolu icin */
    if (DEBUG == 1) {
        printf("File parent: %d, File child: %d\n", getppid(), getpid());
    }
    /* direc kapatilir*/
    closedir(direc);
    return toplamEslesme;
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
    if (karakter == EOF) {

        printf("Boş dosya");
        fclose(inp);
        exit(1);
    }
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

int howManyTimesAndWhere(char *kelime, char *path, char *file) {

    FILE *inp = fopen(FILE_NAME, "a");

    int sizeOfWord;
    char *tempArray = NULL;
    char *array = NULL;
    int sayac;
    int satir = 1, sutun = 1, i = 0, j = 0;
    int eslesenKelime = 0, eslesenHarf = 0;
    int temporary = 0, gecici = 0;

    sayac = sayacBul(path);
    sizeOfWord = findSizeString(kelime);
    /* arguman olarak girilen stringin boyu uzunlugunda tempArray olusturulur*/
    tempArray = malloc(sizeOfWord * sizeof (char));
    /* dosyadaki karakter sayisi kadar array icin malloc ile yer ayrilir*/
    array = malloc(sayac * sizeof (char));

    /*okudugumuz dosyadaki bilgiler malloc ile olusturdugumuz arraye yerlestirilir*/
    TakeToStringFromFile(path, array);

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

            if (tempArray[i] == kelime[i]) {

                ++eslesenHarf;
            }
        }

        /* Eger eslenenHarf arguman olarak girilen stringin boyuna esitse   */
        /* bu eslenenKelime sayisini arttirmamiz gerektigini gosterir*/
        if (eslesenHarf == sizeOfWord) {

            ++eslesenKelime;
            fprintf(inp, "%s: [%d,%d] %s first character is found.\n", file, satir, sutun, kelime);

        }

        temporary++;
        eslesenHarf = 0;
    }
    fclose(inp);
    free(tempArray);
    free(array);
    return eslesenKelime;
}

/*  Bu wait fonksiyonu ders kitabımızdan alinmistir */
/* Unix Systems Programming , Communications, Concurrency and Threads-Kay A.Robins-Steven Robins */

/* Program 3.3 r_wait.c */
pid_t r_wait(int *stat_loc) {
    int retval;
    while (((retval = wait(stat_loc)) == -1) && (errno == EINTR));
    return retval;
}

int readFifo(int *dirCheck, int fd){
    
    int i=0;
    int rdFifo=0;
    int returnVal=0;
    
    while(i<*dirCheck){
        
        if(read(fd,&rdFifo, sizeof(int))==-1){
            perror("dont read from fifo");
            exit(0);
            
        }
        else{
            returnVal+=rdFifo;
        }
        ++i;
    }
    --(*dirCheck);
    return returnVal;
    
}
