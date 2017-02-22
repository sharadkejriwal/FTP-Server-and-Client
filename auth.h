#include <iostream>
#include <sys/socket.h>
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

struct User
{
	struct sockaddr_in client_cmd_addr;
	struct sockaddr_in client_data_addr;
};

class auth 
{
	private:
		static map<string, string> authn;			//username and password key value pair
		static map<string, list< struct User > > activeusers;
		
	public:
		void setAuth(string, string );
		bool getAuth(string, string);
		bool getUser(string);
		bool setActiveUser(string, struct User);
		bool removeActiveUser(string, struct User);
		
		list<struct User> getActiveUsers(string );
		
};