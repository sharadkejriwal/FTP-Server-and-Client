#include <iostream>
#include <sys/param.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

class sys
{
	private:
		int buffer_len = 100000;
	public:
		int changedir(char *);
		char *cmdinfo(char *);
		FILE *openfile(char *);

};