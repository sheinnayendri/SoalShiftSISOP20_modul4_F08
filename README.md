# SoalShiftSISOP20_modul4_F08
Soal Shift Sistem Operasi 2020
#
1. Sheinna Yendri (05111840000038)
2. Muhammad Rafi Yudhistira (05111840000115)
#
1. [Soal1](#soal1)
2. [Soal2](#soal2)
3. [Soal3](#soal3)
4. [Soal4](#soal4)
#

Soal ini meminta kami membuat File System seperti layaknya linux, dengan beberapa ketentuan dan mount source (root) filesystem adalah direktori ```/home/[user]/Documents```.

## Soal1
Soal 1 meminta kami melakukan enkripsi dengan Caeshar Cipher key ```9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO``` dan dengan ```key=10```. Oleh karena itu, kami buat fungsi tersendiri untuk melakukan enkripsi dan dekripsi ketika dibutuhkan seperti sebagai berikut:
```c
char key[90] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
int ges = 10;
void encr(char* str)
{
	int i, j;
	if(!strcmp(str, ".") || !strcmp(str, "..")) return;
	for(i = 0; i < strlen(str); i++)
	{
		for(j = 0; j < 87; j++)
		{
			if(str[i] == key[j]){
				str[i] = key[(j + ges) % 87];
				break;
			}
		}
	}
}

void decr(char* str)
{
	int i, j;
	if(!strcmp(str, ".") || !strcmp(str, "..")) return;
	for(i = 0 ; i < strlen(str); i++)
	{
		for(j = 0; j < 87; j++)
		{
			if(str[i] == key[j]){
				str[i] = key[(j + 87 - ges) % 87];
				break;
			}
		}
	}
}
```
Source code di atas berfungsi untuk melakukan enkripsi dan dekripsi, tanpa melakukan enkripsi untuk tanda ```/``` seperti permintaan soal, kemudian untuk memisahkan ekstensi file dengan nama file, kami gunakan fungsi berikut:
```c
char ext[100000] = "\0";
int id = 0;

int check_ext(char* file)
{
	id = 0;
	while(id < strlen(file) && file[id] != '.') id++;
	memset(ext, 0, sizeof(ext));
	strcpy(ext, file + id);
	return id;
}
```
Di mana nantinya untuk mengetahui posisi karakter start ekstensi ada pada variabel ```id```, sedangkan untuk string ekstensinya sendiri tersimpan pada variabel ```ext```.

Dikarenakan enkripsi hanya berlangsung pada folder yang memiliki nama folder diawali dengan ```encv1_```, maka kami akan mengambil substring dari tiap nama file yang ada dalam folder secara rekursif, dengan kode sebagai berikut:
```c
void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   while (c < l) 
   {
      sub[c] = s[p + c];
      c++;
   }
   sub[c] = '\0';
}

void listFilesRecursivelyEnc1(char *basePath)
{
	char fpath[1000];
	if(strcmp(basePath,"/") == 0)
	{
		basePath=dirpath;	
		sprintf(fpath,"%s",basePath);
	}
	else sprintf(fpath, "%s%s",dirpath,basePath);

    char path[1024];
    struct dirent *dp;
    DIR *dir = opendir(fpath);

    if(!dir) return;

    while((dp = readdir(dir)) != NULL)
    {
        if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
        	char name[1024], newname[1024], foldername[1024];
        	strcpy(name, dp->d_name);
            check_ext(name);
			char namafile[100000];
			memset(namafile, 0, strlen(namafile));
			strncpy(namafile, name, id);
			
        	encr(namafile);
        	sprintf(newname, "/log%s.%s", namafile, ext);
        	memset(foldername, '\0', sizeof(foldername));
			sprintf(foldername, "%s%s", fpath, newname);
			
			pid_t cid;
			cid = fork();
			if(cid == 0)
			{
				char *argv[] = {"cp", fpath, foldername, NULL};
				execv("/bin/cp", argv);
				exit(1);
			}
			else wait(NULL);
			
            printf("%s\n", foldername);
            
            strcpy(path, fpath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            listFilesRecursivelyEnc1(path);
        }
    }
    closedir(dir);
}

void encrypt1(char *name)
{
	listFilesRecursivelyEnc1(name);
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	printf("MKDIR\n");
    
    char cek_substr[10];
    substring(path, cek_substr, 0, 6);
	printf("%s -- %s", path, cek_substr);
	if(strcpy(cek_substr, "encv1_") == 0) //folder encrypt1
	{
		encrypt1(path);	
	}
	else if(strcpy(cek_substr, "encv2_") == 0) //folder encrypt2
	{
		encrypt2(path);
	}
	
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
    writeI("MKDIR", fpath);
	int res;


	res = mkdir(fpath, mode);
	if (res == -1) return -errno;
	return 0;
}
```
Terlihat bahwa fungsi di atas melakukan looping terhadap direktori pada path yang dipassing melalui parameter, kemudian looping dilakukan secara rekursif, ditandai dengan fungsi ```listFilesRecursivelyEnc1(path);``` dipanggil dalam loop folder, agar tiap folder dalam folder juga dicek dan dienskripsi.

Kemudian setiap akan memanggil fungsi enkripsi, maka harus memanggil fungsi ```substring``` yang telah kami declare sendiri untuk mendapatkan namafilenya saja, tanpa ekstensi, kemudian setelah namafile sudah dienkripsi, baru digabungkan kembali dengan ```sprintf```.
#

## Soal2
Sama seperti soal2, ketika ada folder berawalan ```encv2_``` maka akan dilakukan enkripsi mode 2, di mana suatu file akan dipecah-pecah menjadi 1024KB. Di sini kami menggunakan bantuan fungsi split utuk memisah file dengan command:
```c
split -d 3 -b 1024K [nama_file] [nama_file].
```
Di mana perintah ini berarti memecah nama_file menjadi 1024KB untuk setiap filenya, kemudian dengan format nama seperti nama_file itu sendiri diconcat dengan titik dan kemudian nomor ID, dengan command ```-d 3``` yang berarti 3 digit.

Kemudian untuk menggabungkannya, sama halnya, dilakukan looping pada setiap folder untuk mengecek apakah ada file yang memiliki pecah-pecahan, agar dapat digabung dengan menuliskan pada suatu file baru, dengan source code sebagai berikut:
```c
void listFilesRecursivelyDec2(char *basePath)
{
	char fpath[1000];
	if(strcmp(basePath,"/") == 0)
	{
		basePath=dirpath;	
		sprintf(fpath,"%s",basePath);
	}
	else sprintf(fpath, "%s%s",dirpath,basePath);

    char path[1024];
    struct dirent *dp;
    DIR *dir = opendir(fpath);

    if(!dir) return;

    while((dp = readdir(dir)) != NULL)
    {
        if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
        	char name[1024];
        	strcpy(name, dp->d_name);
            check_ext(name);
            int l = strlen(ext) + 1;
            
			if(strlen(dp->d_name) > 7 && strncmp(dp->d_name + strlen(dp->d_name) - l, ext, 1) == 0)
			{
				char comb[1000];
				sprintf(comb, "%s/",fpath);
				strncat(comb, dp->d_name, strlen(dp->d_name) - l);
				struct stat st;
				if(stat(comb, &st) < 0) creat(comb, 0777);
				FILE* combined;
				combined = fopen(comb, "a+");
				FILE* need;
				char this[1000];
				sprintf(this, "%s/%s", fpath, dp->d_name);
				need = fopen(this, "r");
				int c;
				c = fgetc(need);
				while(c != EOF) fprintf(combined, "%c", c);
				fclose(need);
				fclose(combined);
				memset(this, 0, sizeof(this));
				memset(comb, 0, sizeof(comb));
			}
		}
		strcpy(path, fpath);
		strcat(path, "/");
		strcat(path, dp->d_name);

		listFilesRecursivelyDec2(path);
    }
    closedir(dir);
}
```
#

## Soal3

#

## Soal4
Soal ini meminta kami untuk membuat file log yang berisi perintah-perintah apa saja yang dilakukan pada file system dengan format:
```[LEVEL]::[yy][mm][dd]-[HH]:[MM]:[SS]::[CMD]::[DESC ...]```

Dengan keterangan:
```
LEVEL    : Level logging
yy   	 : Tahun dua digit
mm    	 : Bulan dua digit
dd    	 : Hari dua digit
HH    	 : Jam dua digit
MM    	 : Menit dua digit
SS    	 : Detik dua digit
CMD     	 : System call yang terpanggil
DESC      : Deskripsi tambahan (bisa lebih dari satu, dipisahkan dengan ::)
```

Contoh isi file log:
```
INFO::200419-18:29:28::MKDIR::/iz1
INFO::200419-18:29:33::CREAT::/iz1/yena.jpg
INFO::200419-18:29:33::RENAME::/iz1/yena.jpg::/iz1/yena.jpeg
```
Di mana LEVEL ada 2 yaitu ```INFO``` dan ```WARNING```, di mana ```WARNING``` hanya untuk perintah ```rmdir``` ataupun ```unlink```.

Untuk mencatat log tersebut, kami membuat fungsi ```WriteI``` dan ```WriteW``` untuk menangani setiap pencatatan ke file log pada path ```/home/[user]/fs.log```, di mana fungsi tersebut memiliki parameter pointer ke string command yang sedang dijalankan:
```c
void writeI(char *text, char* path)
{
    char* info = "INFO";
	char curtime[30];
    time_t t = time(NULL);
    struct tm* p1 = localtime(&t);
	strftime(curtime, 30, "%y%m%d-%H:%M:%S", p1);
    char log[1000];
    sprintf(log, "%s::%s::%s::%s", info, curtime, text, path);
	FILE *out = fopen("/home/sheinna/fs.log", "a");  
    fprintf(out, "%s\n", log);  
    fclose(out);  
    return 0;
}

void writeW(char *text, char* path)
{
    char* info = "WARNING";
    char curtime[30];
    time_t t = time(NULL);
    struct tm* p1 = localtime(&t);
	strftime(curtime, 30, "%y%m%d-%H:%M:%S", p1);
    char log[1000];
    sprintf(log, "%s::%s::%s::%s", info, curtime, text, path);
	FILE *out = fopen("/home/sheinna/fs.log", "a");  
    fprintf(out, "%s\n", log);  
    fclose(out);  
    return 0;
}
```
#
