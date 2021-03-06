#include<stdio.h>
#include<dirent.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include<sys/wait.h>
#define ZERO 0
#define PATH_UZUNLUK 255
#define DEBUG 0
#define FILE_NAME "log.txt"
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
void howManyTimesAndWhere(char *kelime, char *path, char *file);

void ReadStringFromDirectories(char *gelenString, char *directory);

/*  Bu wait fonksiyonu ders kitabımızdan alinmistir */
/* Unix Systems Programming , Communications, Concurrency and Threads-Kay A.Robins-Steven Robins */
/* Program 3.3 r_wait.c */
pid_t r_wait(int *stat_loc);

int main(int argc, char** argv) {
	FILE* inp;
    char *instruction1 = "./exe";
    char *instruction2 = "./listdir";
    int arttir = 0, i = 0;
    char karakter;
    int sayac = 0;
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
    ReadStringFromDirectories(argv[1], argv[2]);


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
    

    return 0;
}

void ReadStringFromDirectories(char *gelenString, char *directory) {



    int isimUzunlugu = 0;
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
                /*   Recursive cagrisi yapilir            */
                ReadStringFromDirectories(gelenString, yeniYolArray);

                exit(ZERO);

            }

        }            /* Eger dosya bulur ise bu kosulda islem yapacak */
            /* S_ISREG dosya olup olmadigini kontrol eder     */
            /* dogru ise 1 return eder */
        else if (S_ISREG(tempstat.st_mode) && (direntpoint->d_name[0] != '.' &&
                direntpoint->d_name[isimUzunlugu - 1] != '~'
                && strcmp(direntpoint->d_name, "..") != 0)) {

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
                howManyTimesAndWhere(gelenString, yeniYolArray, direntpoint->d_name);
                closedir(direc);
                exit(ZERO);
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

void howManyTimesAndWhere(char *kelime, char *path, char *file) {

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
}

/*  Bu wait fonksiyonu ders kitabımızdan alinmistir */
/* Unix Systems Programming , Communications, Concurrency and Threads-Kay A.Robins-Steven Robins */

/* Program 3.3 r_wait.c */
pid_t r_wait(int *stat_loc) {
    int retval;
    while (((retval = wait(stat_loc)) == -1) && (errno == EINTR));
    return retval;
}
