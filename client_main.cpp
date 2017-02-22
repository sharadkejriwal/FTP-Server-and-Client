#include "client.h"

#include <iostream>

int main(int argc, char **argv )
{
	if(argc!=4)
	{
		cout << "3 arguements server ip, control port and data port is needed" << endl;
		exit(0);
	}
	
	const char *serv_ip = argv[1];
	const char *control_port = argv[2];
	const char *data_port = argv[3];
	
	Client client(serv_ip, control_port, data_port);
	
	bool res = client.init();
	
	if(!res)
	{
		cout << "Some error occured" << endl;
	}
	
	
	return 0;	
}