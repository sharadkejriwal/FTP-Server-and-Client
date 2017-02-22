#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<cstring>
#include<cstdlib>
#include<string>
#include<fstream>
#include<map>
#include<list>

using namespace std;

class ftp_common
{
	private:
		int buffer_len = 1234;
		int cmd_buffer_len = 10000;
		
	public:
	
		char *FAIL = "Failed Receving command";
		int send_file(int, char *);
		int send_cmd(int, char *);
		int function_send(int, char*, int );
		
		
		long long recv_file(int, char *);
		
		char* recv_cmd(int );
		
		int recv_response(int );			//-1 on error receiving response 
											//odd on success
											//even on invalid credintials
		
		int recv_cmd_description(int );
		
		char *changebufferlen(char *,int );
		
		
};