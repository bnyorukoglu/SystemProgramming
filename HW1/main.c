#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* fonksiyona yollanan kelimenin uzunlugunu bulur.                           */
int findSizeString(char** kelime);
/* bu fonksiyon icerisinde yollanan parametre adi ile dosya acilir           */
/* dosyadaki karakter sayisi bulunur ve karakter sayisi kadar uzunlugu olan  */
/* array olusturulur ve dosyadaki karakterler arraye yerlestirilir           */
void TakeToStringFromFile(const char fileName[], char *array);         
/* Bu Dosyadaki karakter sayısını bulur                                      */
int sayacBul(const char fileName[]);
/* konsoldan girilen stringin dosya icerisinde kac defa gectigini            */
/* ve başlangıc satir ve sutununu ekrana yazar                               */
void howManyTimesAndWhere(char *array, int sizeOfWord, char **argv, int sayac);

int main(int argc, char** argv) {
    int i, arttir = 0;
    char *myArray;
	/* executable name kontrolunu daha rahat yapabilmek icin olusturdugum   */
	/* arrayler                                                             */
    char *instruction1 = "./exe";
    char *instruction2 = "./list";


    int toplamHarf = 0;
    int wordSize = findSizeString(argv);
	/* arguman sayisi eksik ya da fazla girilirse ekrana hata basip 		*/
	/* programdan cikar                                                     */
    if (argc != 3) {

        printf("Usage: exe string filename\n ");
        return -1;
    }
	
	/* Dosyada karaakter yoksa                                              */
    if (findSizeString(argv) <= 0) {

        printf("String is empty.\n");
        return -1;
    }



	/* executable name ./exe veya ./list ise programa devam eder            */
	/* ancak bu ikisi disinde bir isim gelirse hata verip cikar             */
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
            printf("list ya da exe olmalı\n");
            return -1;
        }
    }

	/* arguman olarak girilen dosyadaki karakter sayisini olcer             */
    toplamHarf = sayacBul(argv[2]);
	/* olculen karakter sayisi uzunlugunda malloc ile array olusturulur     */
    myArray = malloc(toplamHarf * sizeof (char));
	/* Dosyadan okunan karakterler olusturulan arraye yerlestirilir         */
    TakeToStringFromFile(argv[2], myArray);
	/* Dosyada kac tane bulunması istenen stringin gectigini bulur          */
	/* Baslangıc adreslerini verir                                          */
    howManyTimesAndWhere(myArray, wordSize, argv, toplamHarf);
	/*alinan yerler serbest birakildi                                       */
	free(myArray);
    return 0;
}

/* fonksiyona yollanan kelimenin uzunlugunu bulur.                           */
int findSizeString(char** kelime) {
    int sayac;
    for (sayac = 0; kelime[1][sayac] != '\0'; ++sayac);

    return sayac;
}

int sayacBul(const char fileName[]) {
    int sayac = 0;
    char karakter;
    /* Dosya acilir                                                         */
    FILE *inp = fopen(fileName, "r");
    /* Eger dosya acilamadi ise hata verip cikar                            */
    if (inp == NULL) {
        printf("Cannot open file : %s\n", fileName);
        exit(1);
    }

    karakter = fgetc(inp);
    /* ilk karakter EOF ise bu dosya bos bir dosyadir ekrana hata basar     */
    /* Degilse karakter sayisini bul                                        */
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
void TakeToStringFromFile(const char fileName[], char *array) {
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
	/* Dosya acilir                                                         */
    inp = fopen(fileName, "r");

    karakter = fgetc(inp);
	/* while dongusu ile dosyadan okunan karakterler tek tek arraye 		*/
	/* yerlestirlir                                                         */
    while (karakter != EOF) {

        array[i] = karakter;



        karakter = fgetc(inp);
        ++i;
    }
	/* Acilan dosya kapatilir                                               */
    fclose(inp);
}

void howManyTimesAndWhere(char *array, int sizeOfWord, char **argv, int sayac) {

    char *tempArray = NULL;
    int satir = 1, sutun = 1, i=0, j=0;
    int eslesenKelime = 0, eslesenHarf = 0;
    int temporary = 0, gecici = 0;

	/* arguman olarak girilen stringin boyu uzunlugunda tempArray olusturulur*/
    tempArray = malloc(sizeOfWord * sizeof (char));

    for (j = temporary; j <= sayac - sizeOfWord-1; ++j, ++sutun) {

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

            if (tempArray[i] == argv[1][i]) {

                ++eslesenHarf;

            }
        }

		/* Eger eslenenHarf arguman olarak girilen stringin boyuna esitse   */
		/* bu eslenenKelime sayisini arttirmamiz gerektigini gosterir*/
        if (eslesenHarf == sizeOfWord) {

            ++eslesenKelime;
            printf("[%d,%d] konumunda ilk karakter bulundu. \n", satir, sutun);

        }

        temporary++;
        eslesenHarf = 0;
    }
    printf("%d adet ", eslesenKelime);
    for (i = 0; i < findSizeString(argv); ++i) {
        printf("%c", argv[1][i]);
    }
    printf(" bulundu.\n");
	/* malloc ile olusturulan tempArrayi free etmeliyiz                     */
    free(tempArray);
}
