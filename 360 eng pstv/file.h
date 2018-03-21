#ifndef __FILE_H__
#define __FILE_H__

#include <psp2/io/dirent.h> 
#include <psp2/io/fcntl.h> 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <psp2/net/http.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#define printf psvDebugScreenPrintf

#define TRANSFER_SIZE 64 * 1024 //64kb
#define MAX_PATH_LENGTH 1024
int doesFileExist(const char* path);
int doesDirExist(const char* path);

int copyFile(char *src_path, char *dst_path);
int getFileSize(const char *file);
int createEmptyFile(char *path);

int makePath(const char *dir);
int removePath(char *path);

#endif
