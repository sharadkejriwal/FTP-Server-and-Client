#include "ftp_common.h"
#include "sys.h"

#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<cstring>
#include<string>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<cstdlib>
#include <unistd.h>
#include<time.h>
#include<fstream>
#include<map>


using namespace std;

class Client
{
		int cmdsockfd;				//File descriptor for control port connection
		int datasockfd;			//File descriptor for data port connection
		
		struct sockaddr_in serv_cmd_addr;
		struct sockaddr_in serv_data_addr;
		
		ftp_common ftp_common_obj;
		sys sys_obj;
		
		int SOCKET_READ_TIMEOUT_SEC = 3;
		
	public:
		Client(const char *, const char *, const char *);
		bool init();
		bool login();
		bool ftp_request();
		bool stop();
		void print_res(int );
		~Client();
		
};