#include "sys.h"

int sys::changedir(char *dir)
{
	int r = chdir(dir);
	return r;
}

//cmd can be "ls" or "pwd"
char *sys::cmdinfo(char *cmd)
{
	//if(cmd == "ls")
	//	strcat(cmd,"- l");
	FILE *fp = popen(cmd, "r");
	
	char *buffer = (char *)malloc(buffer_len * sizeof(char));
	
	fread(buffer, 1, ftell(fp), fp);
	
	return buffer;
	
}

FILE *sys::openfile(char *fname)
{
	return fopen(fname,"r");			//return FILE pointer on success or null on error
}