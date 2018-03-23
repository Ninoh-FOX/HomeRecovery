#include "file.h"
#include "graphics.h"

int WriteFile(char *file, void *buf, int size) {
        buf = malloc(0x100);
	SceUID fd = sceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}

int mount(int argc, char *argv[]) {return 0;}

int doesFileExist(const char* path) { 
	SceUID dir = sceIoOpen(path, SCE_O_RDONLY, 0777); 
 	if (dir >= 0) { 
 		sceIoClose(dir); 
 		return 1; 
 	} else { 
 		return 0; 
 	} 
} 

int doesDirExist(const char* path) { 
	SceUID dir = sceIoDopen(path); 
 	if (dir >= 0) { 
 		sceIoDclose(dir); 
 		return 1; 
 	} else { 
 		return 0; 
 	} 
} 

int getFileSize(const char *file) {  //thx Flow VitaShell
	SceUID fd = sceIoOpen(file, SCE_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	int fileSize = sceIoLseek(fd, 0, SCE_SEEK_END);
	
	sceIoClose(fd);
	return fileSize;
}

int copyFile(char *src_path, char *dst_path) { //thx Flow VitaShell
	// The source and destination paths are identical
	if (strcmp(src_path, dst_path) == 0) return -1;

	// The destination is a subfolder of the source folder
	int len = strlen(src_path);
	if (strncmp(src_path, dst_path, len) == 0 && dst_path[len] == '/') return -1;

	SceUID fdsrc = sceIoOpen(src_path, SCE_O_RDONLY, 0);
	if (fdsrc < 0) return fdsrc;

	SceUID fddst = sceIoOpen(dst_path, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0777);
	if (fddst < 0) {
		sceIoClose(fdsrc);
		return fddst;
	}

	void *buf = malloc(TRANSFER_SIZE);

	int read;
	while ((read = sceIoRead(fdsrc, buf, TRANSFER_SIZE)) > 0) {
		sceIoWrite(fddst, buf, read);
	}

	free(buf);

	sceIoClose(fddst);
	sceIoClose(fdsrc);

	return 0;
}

int createEmptyFile(char *path) {
	FILE *file = fopen(path, "w");
	
	if (file == NULL){
		return -1;
		
	} else {
		//fprintf(file, "");
	}
	fclose(file);
	return 0;
}

int makePath(const char *dir) { //thx molecule
	char dir_copy[0x400] = {0};
	snprintf(dir_copy, sizeof(dir_copy) - 2, "%s", dir);
	dir_copy[strlen(dir_copy)] = '/';
	char *c;
	
	for (c = dir_copy; *c; ++c) {
		if (*c == '/') {
			*c = '\0';
			sceIoMkdir(dir_copy, 0777);
			*c = '/';
		}
	}
	
	//test
	SceUID test = sceIoDopen(dir); 
 	if (test >= 0) { 
 		sceIoDclose(test); 
 		return 0; //success
 	} else { 
 		return 1; 
 	} 
}

int removePath(char *path) {  //thx Flow VitaShell
	SceUID dfd = sceIoDopen(path);
	if (dfd >= 0) {
		int res = 0;

		do {
			SceIoDirent dir;
			memset(&dir, 0, sizeof(SceIoDirent));

			res = sceIoDread(dfd, &dir);
			if (res > 0) {
				if (strcmp(dir.d_name, ".") == 0 || strcmp(dir.d_name, "..") == 0)
					continue;

				char *new_path = malloc(strlen(path) + strlen(dir.d_name) + 2);
				snprintf(new_path, MAX_PATH_LENGTH, "%s/%s", path, dir.d_name);

				if (SCE_S_ISDIR(dir.d_stat.st_mode)) {
					int ret = removePath(new_path);
					if (ret <= 0) {
						free(new_path);
						sceIoDclose(dfd);
						return ret;
					}
				} else {
					int ret = sceIoRemove(new_path);
					if (ret < 0) {
						free(new_path);
						sceIoDclose(dfd);
						return ret;
					}
				}

				free(new_path);
			}
		} while (res > 0);

		sceIoDclose(dfd);

		int ret = sceIoRmdir(path);
		if (ret < 0)
			return ret;

	} else {
		int ret = sceIoRemove(path);
		if (ret < 0)
			return ret;
	}

	return 1;
}
