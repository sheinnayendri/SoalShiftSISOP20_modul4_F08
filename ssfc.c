#define FUSE_USE_VERSION 28
#include<fuse.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<errno.h>
#include<sys/time.h>

static const char *dirpath = "/home/sheinna/Documents";

char ext[100000] = "\0";
int id = 0;

void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   while (c < l) 
   {
      sub[c] = s[p + c];
      c++;
   }
   sub[c] = '\0';
}

char key[90] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
int ges = 10;
void encr(char* str)
{
	int i, j;
	if(!strcmp(str, ".") || !strcmp(str, "..")) return;
	for(i = 0; i < strlen(str); i++)
	{
		for(j = 0; j < 87; j++){
			// printf("%c",key[j]);
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
		for(j = 0; j < 87; j++){
			// printf("%c",key[j]);
			if(str[i] == key[j]){
				str[i] = key[(j + 87 - ges) % 87];
				break;
			}
		}
	}
}

int check_ext(char* file)
{
	id = 0;
	while(id < strlen(file) && file[id] != '.') id++;
	memset(ext, 0, sizeof(ext));
	strcpy(ext, file + id);
	return id;
}

void listFilesRecursivelyEnc1(char *basePath)
{
    char path[1024];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

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
			sprintf(foldername, "%s%s", basePath, newname);
			
			pid_t cid;
			cid = fork();
			if(cid == 0)
			{
				char *argv[] = {"cp", basePath, foldername, NULL};
				execv("/bin/cp", argv);
				exit(1);
			}
			else wait(NULL);
			
            printf("%s\n", foldername);
            
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            listFilesRecursivelyEnc1(path);
        }
    }
    closedir(dir);
}

void listFilesRecursivelyDec1(char *basePath)
{
    char path[1024];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

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
			
        	decr(namafile);
        	sprintf(newname, "%s.%s", namafile, ext);
        	memset(foldername, '\0', sizeof(foldername));
			sprintf(foldername, "%s%s", basePath, newname);
			
			pid_t cid;
			cid = fork();
			if(cid == 0)
			{
				//gtw dia tau atau ngga kl file mana yg udah diencrypt atau blm
				//soalnya dimove ke tempat yg sama (cuma rename)
				//jadi loop-nya bs jalan trs
				char *argv[] = {"cp", basePath, foldername, NULL};
				execv("/bin/cp", argv);
				exit(1);
			}
			else wait(NULL);
			
            printf("%s\n", foldername);
            
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            listFilesRecursivelyDec1(path);
        }
    }
    closedir(dir);
}

void listFilesRecursivelyEnc2(char *basePath)
{
    char path[1024];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if(!dir) return;

    while((dp = readdir(dir)) != NULL)
    {
        if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {	
			char finalPath[1024];
        	memset(finalPath, '\0', sizeof(finalPath));
        	char ch = '.';
    		strncat(finalPath, &ch, 1);
			pid_t cid;
			cid = fork();
			if(cid == 0)
			{
//				split -d 3 -b 1024K [nama_file] [nama_file].
//				csplit -n 3 domainlist 
				char *argv[] = {"split", "-d", "3", "-b", "1024K", basePath, finalPath, NULL};
				execv("/bin/split", argv);
				exit(1);
			}
			else wait(NULL);
			
            printf("%s\n", basePath);
            
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            listFilesRecursivelyEnc2(path);
        }
    }
    closedir(dir);
}

void listFilesRecursivelyDec2(char *basePath)
{
    char path[1024];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

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
				sprintf(comb, "%s/",basePath);
				strncat(comb, dp->d_name, strlen(dp->d_name) - l);
				struct stat st;
				if(stat(comb, &st) < 0) creat(comb, 0777);
				FILE* combined;
				combined = fopen(comb, "a+");
				FILE* need;
				char this[1000];
				sprintf(this, "%s/%s", basePath, dp->d_name);
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
		listFilesRecursivelyDec2(path);
    }
    closedir(dir);
}

void encrypt1(char *name)
{
	listFilesRecursivelyEnc1(name);
}

void decrypt1(char *name)
{
	listFilesRecursivelyDec1(name);
}

void encrypt2(char *name)
{
	listFilesRecursivelyEnc2(name);
}

void decrypt2(char *name)
{
	listFilesRecursivelyDec2(name);
}

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

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	writeI("LS", path);
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_access(const char *path, int mask)
{
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = access(fpath, mask);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = readlink(fpath, buf, size - 1);
	if (res == -1) return -errno;
	buf[res] = '\0';
	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
    writeI("CD", path);
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
	
	DIR *dp;
	struct dirent *de;
	(void) offset;
	(void) fi;
	dp = opendir(fpath);
	if (dp == NULL) return -errno;
	
	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;		
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
		if(res!=0) break;
	}
	closedir(dp);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    writeI("CREATE", path);
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res;
	
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0) res = close(res);
	} else if (S_ISFIFO(mode)) res = mkfifo(fpath, mode);
	else res = mknod(fpath, mode, rdev);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    writeI("MKDIR", path);
    
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
	int res;


	res = mkdir(fpath, mode);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_unlink(const char *path)
{
    writeW("REMOVE", path);
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = unlink(fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_rmdir(const char *path)
{
    writeW("RMDIR", path);
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = rmdir(fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
    writeI("MOVE", from);
	int res;

	res = rename(from, to);
	
	char cek_substr[10], cek2[10];
    substring(to, cek_substr, 0, 6);
    substring(from, cek2, 0, 6);
	if(strcpy(cek_substr, "encv1_") == 0) //folder encrypt1
	{
		encrypt1(to);	
	}
	else if(strcpy(cek_substr, "encv2_") == 0) //folder encrypt2
	{
		encrypt2(to);
	}
	else if(strcpy(cek2, "encv1_") == 0) //folder decrypt1
	{
		decrypt1(from);
	}
	else if(strcpy(cek2, "encv2_") == 0) //folder decrypt2
	{
		decrypt2(from);
	}
	
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = chmod(fpath, mode);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = lchown(fpath, uid, gid);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
    writeI("TRUNCATE", path);
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = truncate(fpath, size);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(fpath, tv);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
    writeI("OPEN", path);
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = open(fpath, fi->flags);
	if (res == -1) return -errno;
	close(res);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
    writeI("CAT", path);
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int fd = 0;
	int res = 0;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1) return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1) res = -errno;

	close(fd);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
    writeI("WRITE", path);
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1) return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1) res = -errno;

	close(fd);
	return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = statvfs(fpath, stbuf);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) 
{
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;	
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
    (void) fi;

    int res;
    res = creat(fpath, mode);
    if(res == -1) return -errno;
    close(res);
    return 0;
}

static int xmp_release(const char *path, struct fuse_file_info *fi)
{
	(void) path;
	(void) fi;
	return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create     = xmp_create,
	.release	= xmp_release,
	.fsync		= xmp_fsync,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
