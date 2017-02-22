#include "auth.h"
#include "ftp_common.h"
#include "sys.h"

#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<cstring>
#include<cstdlib>
#include<string>
#include<fstream>


using namespace std;

class Server
{
	private:
		int serdatasockfd;				//Data Socket File Descriptor
		int sercmdsockfd;				//Cmd Socket File Descriptor
		int cmdsockfd;			//Client command/control socket file descriptor
		int datasockfd; 			//Client data socket file descriptor
		struct sockaddr_in	 serv_cmd_addr;
		struct sockaddr_in 	 serv_data_addr;
		struct sockaddr_in 	 cli_cmd_addr;
		struct sockaddr_in   cli_data_addr;
		
		char *userName;
		struct User user;
		
		int serv_cmd_port = 7000;	
		int serv_data_port = 7001;
		
		int SOCKET_READ_TIMEOUT_SEC = 3;	
		
		
		
		char *USER = "user";
		char *PASS = "pass";
		//char *SHOW = "show";		//NO communictin with server for this
		char *PWD = "pwd";
		char *LS = "ls";
		char *CD = "cd";
		char *GET = "get";
		char *PUT = "put";
		char *LOGOUT = "logout";
		char *EXIT = "exit";
		
		char *success_response = "31";
		char *failure_response = "30";
		
		char *USER_DOESNT_EXIST = "60";
		char *USER_EXIST = "61";
		char *PASSWORD_NOT_MATCHED = "62";
		char *LOGIN_SUCCESSFULL = "63";
		
		char *CD_FAILURE = "64";
		char *CD_SUCCESSFULL = "65";
		
		char *FILE_NOT_FOUND = "66";
		char *FILE_FOUND = "67";
		
		char *FILE_ALREADY_EXIST = "68";
		char *FILE_DOESNT_EXIST = "69";
		
		char *BAD_FILE = "72";
		
		char *LOGOUT_FAILURE = "70";
		char *LOGOUT_SUCCESS = "71";
				
		
		char *SERVER_ERROR = "Internal Server Error: Unable to send command response ";
		
		
		auth auth_obj;
		sys sys_obj;
		ftp_common ftp_common_obj;
		
	public:
		Server();
		int init();
		bool start();
		bool user_login();
		bool process_request();
		void stop();
		
};