#include "server.h"


int main()
{
	auth auth_obj;		//for authentication
	auth_obj.setAuth("sharad","sharad");
	Server server;
	
	server.init();
	
	return 0;
}
