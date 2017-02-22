#include "client.h"


Client::Client(const char *serv_ip,const char *control_port,const char *data_port)
{
	//Passive FTP as client initiates both data and
	//control/command port channels
	//Other one is Active FTP in which client first initiates 
	//connection on control port and then server initiates 
	//connection on data port
	//Here is passive FTP implementation
	
	
	int serv_port_cmd = atoi(control_port);
	
	memset(&serv_cmd_addr,0,sizeof(serv_cmd_addr));
	
	serv_cmd_addr.sin_family = AF_INET;
	serv_cmd_addr.sin_port =  htons(serv_port_cmd);
	serv_cmd_addr.sin_addr.s_addr = inet_addr(serv_ip);
	
	int serv_port_data = atoi(data_port);
	
	memset(&serv_data_addr,0,sizeof(serv_data_addr));
	
	serv_data_addr.sin_family = AF_INET;
	serv_data_addr.sin_port = htons(serv_port_data);
	serv_data_addr.sin_addr.s_addr = inet_addr(serv_ip);
	
}



bool Client::init()
{
	//Create Socket
	cmdsockfd = socket(AF_INET,SOCK_STREAM,0);

	if(cmdsockfd==-1)
	{
		cout << "Unable to create control socket " << endl;
		return false;
	}
	
	cout << "Control socket created" <<endl;
	
	datasockfd = socket(AF_INET,SOCK_STREAM,0);
	
	if(datasockfd == -1)
	{
		cout << "Unable to create data socket " << endl;
		return false;
	}
	
	cout << "Data Socket created" <<endl;
	
	//Connect on control/command port
	int connect_retval = connect(cmdsockfd,(struct sockaddr *) &serv_cmd_addr,sizeof(serv_cmd_addr));
	
	if(connect_retval==0)
	{
		cout << "Succesfully connected to the server on control port" <<endl;
		//cout << "ftp> " << endl;
	}
	else 
	{
		cout << "Unable to connect to the server on control port: " << endl;
		return false;
	}
	
	//Connect on data port
	connect_retval = connect(datasockfd,(struct sockaddr *) &serv_data_addr,sizeof(serv_data_addr));
	
	if(connect_retval==0)
	{
		cout << "Succesfully connected to the server on data port" <<endl;
		//cout << "ftp> " << endl;
	}
	else 
	{
		cout << "Unable to connect to the server on data port: " << endl;
		return false;
	}
	
	
	/*
	* Setting timeout for recv function call on socket
	*/
	struct timeval timeout;
	timeout.tv_sec = this->SOCKET_READ_TIMEOUT_SEC;
	timeout.tv_usec = 0;
	setsockopt(datasockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	
	bool retval = this->login();
	while(!retval)
	{
		retval = this->login();
	}
	if(retval)
	{
		if(!this->stop())
		{
			return false;
		}
		else
			return true;
	}
	
}


bool Client::login()
{
	int retval;
	cout << endl;
	
	
	char *buffer = (char *)malloc(20*sizeof(char));
	//User command to be send to the server
	cout << "Enter username" <<endl;
	cin >> buffer;			//username
	retval = ftp_common_obj.send_cmd(cmdsockfd,"user");
	if(retval==0)
	{
		cout << "Unable to send User command to server" <<endl;
		this->login();
	}
	int response = ftp_common_obj.recv_response(cmdsockfd);
	print_res(response);
	if(response<0 || response%2==0)
	{
		free(buffer);
		return false;
	}
	
	retval = ftp_common_obj.send_cmd(cmdsockfd,buffer);
	if(retval==0)
	{
		free(buffer);
		cout << "Unable to send User name to server" <<endl;
		return false;
	}
	response = ftp_common_obj.recv_response(cmdsockfd);
	print_res(response);
	if(response<0 || response%2==0)
	{
		free(buffer);
		return false;
	}
	
	
	char *buffer2 = (char *)malloc(20*sizeof(char));
	cout << "Enter password" <<endl;
	cin >> buffer2;				//password
	retval = ftp_common_obj.send_cmd(cmdsockfd,"pass");
	if(retval==0)
	{
		free(buffer2);
		cout << "Unable to send Pass command to server" <<endl;
		return false;
	}
	response = ftp_common_obj.recv_response(cmdsockfd);
	print_res(response);
	if(response<0 || response%2==0)
	{
		free(buffer2);
		return false;
	}
		
	retval = ftp_common_obj.send_cmd(cmdsockfd,buffer2);
	if(retval==0)
	{
		free(buffer2);
		cout << "Unable to send password to server" <<endl;
		return false;
	}
	else
	{
		response = ftp_common_obj.recv_response(cmdsockfd);
		print_res(response);
		if(response<0 || response%2==0)
		{
			free(buffer2);
			return false;
		}
	}
	
	cout << "Login Successful" << endl;
	cout << endl <<endl;
	
	return (ftp_request());

}

bool Client::ftp_request()
{
	int retval;
	string str;
	string dir,fname;
	int response;
	
	
	
	//Loop for receiving commands
	while(1)
	{
		char *buffer = (char *)malloc(10*sizeof(char));
		char *buffer2 = (char *)malloc(100*sizeof(char));
		int response;
		cout << endl << "ftp> ";
		cin >> buffer;			//command
		string str(buffer);		//for comparison
		//cout << buffer << strlen(buffer) << endl;
		
		/* Valid Commands are
			show -> to show a list of available Commands
			pwd -> present working directory in the server
			ls -> list files and folders
			cd dir -> change directory to dir
			get fname -> receive file from server with filename as fname
			put fname -> send file to server at pwd with filename as fname
			logout -> logout from current user session
			exit -> exit ftp
		*/
		if(str=="show")
		{
			cout << "show, pwd, ls, cd dir, get fname, put fname, exit, logout " <<endl <<endl;
			continue;
		}
		if(str == "put")
		{
			cin >> buffer2;
			FILE *fp = sys_obj.openfile(buffer2);
			if(fp)
			{
				cout << "File exist" << endl;
			}
			else
			{
				cout << "File with fname "  << buffer2 << " doesnt exist" << endl;
				continue;
 			}
		}
		
		if(!ftp_common_obj.send_cmd(cmdsockfd,buffer))
		{
			cout << "Unable to send command " << str << " to the server" << endl;
			continue;
		}
		else
		{
			//receive acknowledgement
			response = ftp_common_obj.recv_response(cmdsockfd);
			print_res(response);
			if(response<0 || response%2 == 0)
				continue;
		}
		
		
		if(str=="pwd"||str=="ls")
		{
			if(ftp_common_obj.recv_cmd_description(cmdsockfd)==-1)
			{
				cout << "Error receiving command description" << endl;
			}
		}
		else if(str=="cd")
		{
			cin >> buffer2;					//directory name
			retval = ftp_common_obj.send_cmd(cmdsockfd, buffer2);
			
			if(retval == 0)
				cout << "Unable to send directory name to the server" << endl;
			else 
			{
				response = ftp_common_obj.recv_response(cmdsockfd);
				print_res(response);
				if(response%2==0)
				{
					cout << "No such file or directory" << endl;
				}
			}
		}
		else if(str == "get")
		{
			cin >> buffer2;				//file name
			retval = ftp_common_obj.send_cmd(cmdsockfd, buffer2);
			if(retval == 0)
				cout << "Unable to send file name to the server" << endl;
			else
			{
				response = ftp_common_obj.recv_response(cmdsockfd);
				print_res(response);
				if(response%2 != 0 && response > 0)
				{
					long long file_length = ftp_common_obj.recv_file(datasockfd, buffer2);
					cout << file_length << endl;
					if(file_length<0)
						cout << "Error receiving file" << endl;
					else
						cout << "File with file name " << buffer2 << " successfully received of length " << (double)file_length/1024 << "KB" << endl;
				}
				else if(response > 0)
				{
					cout << "No such file exist" << endl;
				}
			}
		}
		else if(str == "put")
		{
			//cin >> buffer2; 			already done before
			retval = ftp_common_obj.send_cmd(cmdsockfd, buffer2);
			if(retval == 0)
				cout << "Unable to send file name to the server" << endl;
			else
			{
				response = ftp_common_obj.recv_response(cmdsockfd);
				print_res(response);
				if(response%2 != 0 && response > 0)
				{
					retval = ftp_common_obj.send_file(datasockfd, buffer2);
					if(retval == 0)
						cout << "Error sending file" << endl;
					else
						cout << "File successfully sent" << endl;
				}
				else if(response > 0)
				{
					cout << "No such file exist" << endl;
				}
			}
			
		}
		else if(str == "logout")
		{
			response = ftp_common_obj.recv_response(cmdsockfd);
			print_res(response);
			if(response<0 || response%2==0)
			{
				cout << "Error logging out" << endl;
				continue;
			}
			free(buffer);
			free(buffer2);
			cout << "Logging out" <<endl;
			return false;
		}
		else if(str == "exit")
		{
			response = ftp_common_obj.recv_response(cmdsockfd);
			print_res(response);
			if(response <0 || response%2==0)
			{
				cout << "Error exitting" << endl;
				continue;
			}
			free(buffer);
			free(buffer2);
			cout << "Exitting server" << endl;
			return true;
		}
		else 
		{
			cout << "Command not recognized" << endl;
		}
		free(buffer);
		free(buffer2);
	}
}

void Client::print_res(int response)
{
	if(response<0)
	{
		cout << "Error receiving response" << endl;
				
	}
	else if(response%2==0)					//Even response for unmatch
	{
		cout << "Response " << response << " received." <<endl; 
		cout << "False credentials/input/error" <<endl;		
	}	
	else									//Odd one for proper response
	{	
		cout << "Response " << response << " received." <<endl;
		//cout << "Successfull" <<endl;	
	}	
}

bool Client::stop()
{
	int ret_val = close(datasockfd);
	if(!ret_val)
		cout << "Closed data port" << endl;
	else
	{
		cout << "Error closing data port" << endl;
		return false;
	}
		
	ret_val = close(cmdsockfd);	
	if(!ret_val)
		cout << "Closed command port" << endl;
	else
	{
		cout << "Error closing command port " << endl;
		return false;
	}
	return true;
}



Client::~Client()
{
	cout << "Exiting ftp" << endl;
	exit(1);
}

