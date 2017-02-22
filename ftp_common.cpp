#include "ftp_common.h"

int ftp_common::send_file(int sockfd, char *fname)
{
	char *buffer = (char *)malloc(buffer_len*sizeof(char));
	int data_len;
	int retval;
	fstream data_file;
	data_file.open(fname);
	if(data_file.is_open())
	{
		streampos begin,end,size;
		
		begin = data_file.tellg();
		data_file.seekg(0,ios::end);
		end = data_file.tellg();
		size = end-begin ; 						// Gives the size of the file
		cout << size << endl;
		
		data_file.seekg(0,ios::beg);			//put the file pointer to the beginning again .
		
		streampos curr = begin;
		int data_len = buffer_len ; 			
		while((size-curr)>buffer_len)
		{
		   	data_file.read(buffer,data_len);	//data read into buffer
			curr = data_file.tellg();
			retval = function_send(sockfd,buffer,data_len);
			if(retval == 0)
			{
				data_file.close();
				break;
			}
		}
		if(retval!=0)
		{
			data_len = (int)(size - curr);
			//cout << data_len;
			data_file.read(buffer,data_len);			
			retval = function_send(sockfd,buffer,data_len);
			data_file.close();
			if(retval!=0)
				return 1;
			else 
				return 0;
		}
		else
		{
			cout << "Error" << endl;
			return 0;
		}
		
	}
	else 
	{
		cout << "Unable to open the file" << endl;
		return 0;
	}
}

int ftp_common::send_cmd(int sockfd, char *command)
{
	int data_len = strlen(command);
	
	int retval = function_send(sockfd, command, data_len);
	
	return retval;
}


int ftp_common::function_send(int sockfd, char *buffer, int data_len)
{
	int flag;			
	int send_retval;						//returns no. of bytes send else -1 on error
	
	//xcout << buffer <<endl;
	send_retval = send(sockfd,buffer,data_len,0);
	if(send_retval==data_len)
	 	return 1;
	else
		return 0;
}

long long ftp_common::recv_file(int sockfd, char *fname)
{
	char *buffer = (char *)malloc(buffer_len*sizeof(char));
	ofstream file;
	file.open(fname);
	int recv_retval;
	long long len = 0;			//File length can be greater than a GB
	
	
	//fcntl(sockfd, F_SETFL, O_NONBLOCK);
	//recv returns no. of bytes read into buffer else -1 on error
	while(1)
	{
		recv_retval = recv(sockfd,buffer,buffer_len,0);
		if(recv_retval<=0)
			break;
		len += recv_retval;
		cout << recv_retval << " " << len << endl;
		file.write(buffer,recv_retval);

	}
	file.close();
	if(recv_retval<0)
	{
		if(errno != EAGAIN && errno != EWOULDBLOCK)			//Timeout reached
			return len;
		else
		{
			cout << "Error occured " <<endl;
			return -1;
		}
	}
	else
	{
		return len;
	}
}

char *ftp_common::recv_cmd(int sockfd)
{
	char *buffer = (char *)malloc((cmd_buffer_len)*sizeof(char));
	
	int recv_retval = recv(sockfd,buffer,cmd_buffer_len,0);
	
	if(recv_retval<0)
	{
		return this->FAIL;
	}
	else
	{
		return buffer;
		//return changebufferlen(buffer, recv_retval);
	}
}



int ftp_common::recv_response(int sockfd)
{
	int response;
	char *res= this->recv_cmd(sockfd);
	if(res != this->FAIL)
	{
		response = atoi(res);
		return response;
	}
	else
		return -1;
}

int ftp_common::recv_cmd_description(int sockfd)
{
	char *buffer = (char *)malloc((buffer_len)*sizeof(char));
	int recv_retval;
	int len = 0;
	if((recv_retval = recv(sockfd,buffer,buffer_len,0))>0)
	{
		len += recv_retval;
		cout << buffer;
	}
	if(recv_retval<0)
		return -1;
	else
		return len;
}


char* ftp_common::changebufferlen(char *buffer, int data_len)
{
	char *buffer2 = (char *)malloc(data_len*sizeof(char));
	strncpy(buffer2, buffer, data_len);
	return buffer2;
}