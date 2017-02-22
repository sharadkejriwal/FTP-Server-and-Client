#include "server.h"

Server::Server()
{
	memset(&serv_cmd_addr,0,sizeof(serv_cmd_addr));
	
	serv_cmd_addr.sin_family = AF_INET;
	serv_cmd_addr.sin_port = htons(serv_cmd_port) ; 			
	serv_cmd_addr.sin_addr.s_addr = INADDR_ANY;	
	
	memset(&serv_data_addr,0,sizeof(serv_data_addr));
	
	serv_data_addr.sin_family = AF_INET;
	serv_data_addr.sin_port = htons(serv_data_port) ;
	serv_data_addr.sin_addr.s_addr = INADDR_ANY;
	
}

int Server::init()
{
	sercmdsockfd = socket(AF_INET,SOCK_STREAM,0);		//returns -1 on error
	
	if(sercmdsockfd==-1)
	{
		cout << "Unable to create command socket " << endl;
		exit(1);
	}
	else
	{
		cout << "Control socket created successfully" << endl;
	}
	
	serdatasockfd = socket(AF_INET,SOCK_STREAM,0);		//returns -1 on error
	
	if(serdatasockfd==-1)
	{
		cout << "Unable to create data socket " << endl;
		exit(1);
	}
	else
	{
		cout << "Data socket created successfully" << endl;
	}
	
	
	int bind_retval  = bind(sercmdsockfd , (struct sockaddr *) &serv_cmd_addr , sizeof(serv_cmd_addr));		//0 if success else -1
	
	if(bind_retval==-1)
	{
		cout << "Unable to bind to the command socket " << endl;
		exit(1);
	}
	else
	{
		cout << "Control Socket bind to address " << ntohl(serv_cmd_addr.sin_addr.s_addr) <<endl;
	}
	
	bind_retval  = bind(serdatasockfd , (struct sockaddr *) &serv_data_addr , sizeof(serv_data_addr));		//0 if success else -1
	
	if(bind_retval==-1)
	{
		cout << "Unable to bind to the data  socket " << endl;
		exit(1);
	}
	else
	{
		cout << "Data Socket bind to address " << ntohl(serv_data_addr.sin_addr.s_addr) <<endl;
	}
	
	
	int listen_retval = listen(sercmdsockfd,30);	//30 -> maximum no. of connections kernel  should queue for this socket.
											//0 on success else -1 on error
												
	if(listen_retval==-1)
	{
		cout << "Unable to listen at port no. " << serv_cmd_addr.sin_port <<endl;
		exit(1);
	}
	else
	{
		cout << "Server listening at port no. " << ntohs(serv_cmd_addr.sin_port) << endl;
	}
	
	listen_retval = listen(serdatasockfd,30);	//30 -> maximum no. of connections kernel  should queue for this socket.
											//0 on success else -1 on error
												
	if(listen_retval==-1)
	{
		cout << "Unable to listen at port no. " << serv_data_addr.sin_port <<endl;
		exit(1);
	}
	else
	{
		cout << "Server listening at port no. " << ntohs(serv_data_addr.sin_port) << endl;
	}
	
	
	while(1)
	{
		memset(&cli_cmd_addr,0,sizeof(cli_cmd_addr));
		socklen_t clicmdaddr_size  = sizeof(cli_cmd_addr);
		
		cmdsockfd = accept(sercmdsockfd, (struct sockaddr *)&cli_cmd_addr, &clicmdaddr_size);
		if(cmdsockfd!=-1)
			cout << "Server accepted the request of control channel from host:: " << inet_ntoa(cli_cmd_addr.sin_addr) << " and port :: " << htons(cli_cmd_addr.sin_port) << endl;
		else
			continue;
		
		memset(&cli_data_addr,0,sizeof(cli_data_addr));
		socklen_t clidataaddr_size  = sizeof(cli_data_addr);
		
		datasockfd = accept(serdatasockfd, (struct sockaddr *)&cli_data_addr, &clidataaddr_size);
		if(datasockfd!=-1)
			cout << "Server accepted the request of control channel from host:: " << inet_ntoa(cli_data_addr.sin_addr) << " and port :: " << htons(cli_data_addr.sin_port) << endl;
		else
			continue;
			
		cout << "Server started" << endl;
			
		//Fork returns 0 to child process and pid of 
		//child process to current process
		int pid = fork();
		
		if(pid<0)
		{
			cout << "Error on fork" << endl;
			continue;
		}
		
		if(pid==0)
		{
			cout << "This is the child process" << endl;
			close(sercmdsockfd);
			close(serdatasockfd);
			//cout << 
			this->start();
			this->stop();
			cout << "Child process stopped" << endl;
			break;
		}
		else
		{
			cout << "this is the parent process. Child process crearted with pid " << pid << endl;
			close(datasockfd);
			close(cmdsockfd);
		}
		
	}
	
}

bool Server::start()
{
	while(1)
	{
		
		/*
		* Setting timeout for recv function call on socket
		*/
		struct timeval timeout;
		timeout.tv_sec = this->SOCKET_READ_TIMEOUT_SEC;
		timeout.tv_usec = 0;
		setsockopt(datasockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		//setsockopt(cmdsockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));	Command should be blocking
		
		
		while(!user_login())
		{
			cout << "Failed login attempt" << endl;
		}
		if(!process_request())
		{
			//returns true on logout and false on exit
			break;
		}
	}
	
}

bool Server::user_login()
{
	char *str = ftp_common_obj.recv_cmd(cmdsockfd);
	
	if(!strcmp(str,USER))
	{
		if(!ftp_common_obj.send_cmd(cmdsockfd, this->success_response))
		{
			cout << this->SERVER_ERROR << this->success_response << endl;
			return false;
		}
		
		char *username = ftp_common_obj.recv_cmd(cmdsockfd);
		
		userName = username ;
		
		if(auth_obj.getUser(username) )
		{
			cout << "username matched "<<endl;
			if(!ftp_common_obj.send_cmd(cmdsockfd, this->USER_EXIST))
			{
				cout << this->SERVER_ERROR << this->USER_EXIST << endl;
				free(username);
				return false;
			}
			
		}
		else
		{
			cout << "username didn't match" << endl;
			if(!ftp_common_obj.send_cmd(cmdsockfd, this->USER_DOESNT_EXIST))
			{
				cout << this->SERVER_ERROR << this->USER_EXIST << endl;
			}
			return false;
		}
		
		
		char *pass = ftp_common_obj.recv_cmd(cmdsockfd);
		
		if(!strcmp(pass,PASS))
		{
			if(!ftp_common_obj.send_cmd(cmdsockfd, this->success_response))
			{
				cout << this->SERVER_ERROR << this->USER_EXIST << endl;
				free(username);
				return false;
			}
			
			char *password = ftp_common_obj.recv_cmd(cmdsockfd);
			
			if( auth_obj.getAuth(username, password) )
			{
				cout << "password matched" << endl;
				if(!ftp_common_obj.send_cmd(cmdsockfd, this->LOGIN_SUCCESSFULL))
				{
					cout << this->SERVER_ERROR << this->LOGIN_SUCCESSFULL << endl;
					return false;
				}
			}
			else
			{
				cout << "wrong password" << endl;
				if(!ftp_common_obj.send_cmd(cmdsockfd, this->PASSWORD_NOT_MATCHED))
				{
					cout << this->SERVER_ERROR << this->PASSWORD_NOT_MATCHED << endl;
				}
				return false;
			}
			
			cout << "Login successfull " << endl << endl;
			user.client_cmd_addr = cli_cmd_addr;
			user.client_data_addr = cli_data_addr;		
			return true;
		}
		else
		{
			if(!ftp_common_obj.send_cmd(cmdsockfd, this->failure_response))
			{
				cout << this->SERVER_ERROR << this->failure_response << endl;
			}
			cout << "PASS command expected but received " << pass << endl;
			return false; 
		}
	}
	else
	{
		if(!ftp_common_obj.send_cmd(cmdsockfd, this->failure_response))
		{
			cout << this->SERVER_ERROR << this->failure_response << endl;
		}
		cout << "USER command expected but received "<< str << endl;
		return false; 
	}
}

bool Server::process_request()
{
	while(1)
	{
		char *str = ftp_common_obj.recv_cmd(cmdsockfd);
		cout << "Command received " << str << endl;
		if(!ftp_common_obj.send_cmd(cmdsockfd, this->success_response))
		{
			cout << this->SERVER_ERROR << this->success_response << endl;
			continue;
		}
		
		if(!strcmp(str, PWD))
		{
			char *buffer = sys_obj.cmdinfo(str);
			if(!ftp_common_obj.function_send(cmdsockfd, buffer, strlen(buffer)))
			{
				cout << this->SERVER_ERROR << " pwd info" << endl;
			}
		}
		else if(!strcmp(str, LS))
		{
			char *buffer = sys_obj.cmdinfo(str);
			if(!ftp_common_obj.function_send(cmdsockfd, buffer, strlen(buffer)))
			{
				cout << this->SERVER_ERROR << " ls info" << endl;
			}
		}
		else if(!strcmp(str, CD))
		{
			char *dir = ftp_common_obj.recv_cmd(cmdsockfd);
			
			if(!sys_obj.changedir(dir))				//returns 0 for success
			{
				cout << "Change directory successfull " << endl;
				if(!ftp_common_obj.send_cmd(cmdsockfd, this->CD_SUCCESSFULL))
				{
					cout << this->SERVER_ERROR << this->CD_SUCCESSFULL << endl;
				}
			}
			else
			{
				cout << "No such file or directory" << endl;
				if(!ftp_common_obj.send_cmd(cmdsockfd, this->CD_FAILURE))
				{
					cout << this->SERVER_ERROR << this->CD_FAILURE << endl;
				}
			}
		}
		else if(!strcmp(str, GET))
		{
			char *fname = ftp_common_obj.recv_cmd(cmdsockfd);
			
			FILE *fp = sys_obj.openfile(fname);
			if(fp)
			{
				cout << "File to be sent with fname " << fname << endl;
				if(!ftp_common_obj.send_cmd(cmdsockfd, this->FILE_FOUND))
				{
					cout << this->SERVER_ERROR << this->FILE_FOUND << endl;
				}
				else
				{
					if(!ftp_common_obj.send_file(datasockfd, fname))
					{
						cout << "Error sending file" << endl;
					}
					else
					{
						cout << "File sent successfully" << endl;
					}
				}
			}
			else
			{
				cout << "File to be sent not found of fname " << fname << endl;
				if(!ftp_common_obj.send_cmd(cmdsockfd, this->FILE_NOT_FOUND))
				{
					cout << this->SERVER_ERROR << this->FILE_NOT_FOUND << endl;
				}
			}
		}
		else if(!strcmp(str, PUT))
		{
			char *fname = ftp_common_obj.recv_cmd(cmdsockfd);
			
			FILE *fp = sys_obj.openfile(fname);
			if(fp)
			{
				cout << "FIle to be received but file with fname already exist" << endl;
				if(!ftp_common_obj.send_cmd(cmdsockfd, this->FILE_ALREADY_EXIST))
				{
					cout << this->SERVER_ERROR << this->FILE_ALREADY_EXIST << endl;
				}
			}
			else
			{
				cout << "File to be recived with fname "<< fname << endl;
				if(!ftp_common_obj.send_cmd(cmdsockfd, FILE_DOESNT_EXIST))
				{
					cout << this->SERVER_ERROR << this->FILE_DOESNT_EXIST << endl;
				}
				else
				{
					long long file_length = ftp_common_obj.recv_file(datasockfd, fname);
					if(file_length>=0)
					{
						cout << "File successfully received of fname " << fname << " and of length " << (double)file_length/1024 << "KB. " << endl;
					}
					else
					{
						cout << "Error receiving file" << endl;
					}
				}
			}
		}
		else if(!strcmp(str,LOGOUT) || !strcmp(str, EXIT))
		{
			
			cout << "Logging out " << endl;			
			if(!auth_obj.removeActiveUser(userName, user))
			{
				if(!ftp_common_obj.send_cmd(cmdsockfd, this->LOGOUT_FAILURE))
				{
					cout << this->SERVER_ERROR << this->LOGOUT_FAILURE <<endl;
					continue;
				}
				cout << "Error logging out " << endl;
			}
			else
			{
				if(!ftp_common_obj.send_cmd(cmdsockfd, this->LOGOUT_SUCCESS))
				{
					cout << this->SERVER_ERROR << this->LOGOUT_SUCCESS << endl;
					continue;
				}
				cout << "User logged out " << endl;
				if(!strcmp(str,LOGOUT))
					return true;
				else
				{
					cout << "Exitting server" << endl;
					return false;
				}
			}
		}
		else
		{
			cout << "Unknown command received" << endl;
			if(!ftp_common_obj.send_cmd(cmdsockfd, this->failure_response))
			{
				cout << this->SERVER_ERROR << this->failure_response << endl;
			}
		}
		
	}
}

void Server::stop()
{
	//close returns 0 on success and -1 on failure 
	if(!close(cmdsockfd))
	{
		cout << "Error closing command socket" << endl;
	}
	else
	{
		cout << "Data socket closed " << endl;
	}
	if(!close(datasockfd))
	{
		cout << "Error closing data socket " << endl;
	}
	else
	{
		 cout << "Data socket closed" << endl;
	}
}
