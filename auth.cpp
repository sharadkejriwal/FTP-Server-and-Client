#include "auth.h"

map<string, string> auth::authn;

map<string, list< struct User> > auth::activeusers;

void auth::setAuth(string username, string password)
{
	if(authn.find(username)!=authn.end())
	{
		cout << "User already exist " << endl;
	}
	else
	{
		authn[username] = password;			//insert into map
	}
}

//
bool auth::getAuth(string username, string password)
{
	if(authn.find(username)!=authn.end())
	{
		if(authn[username]!=password)
		{
			cout << "Password do not match" << endl;
			return false;
		}
		cout << "User authenticated" << endl;
		return true;
	}
	else
	{
		cout << "User do not exist" << endl;
		return false;
	}
}

//checks if user exist or not
bool auth::getUser(string username)
{
	if(authn.find(username)!=authn.end())
	{
		cout << "User exist" << endl;
		return true;
	}
	else
	{
		cout << "User do not exist" << endl;
		return false;
	}
}

bool auth::setActiveUser(string username, struct User user_data)
{
	list<struct User> userlist;
	if(activeusers.find(username)!=activeusers.end())
	{
		userlist = activeusers[username];	
	}
	
	userlist.push_back(user_data);	
	activeusers[username] = userlist ;
	return true;
}

bool auth::removeActiveUser(string username, struct User user_data)
{
	if(activeusers.find(username)!=activeusers.end())
	{
		list<struct User> userlist = activeusers[username];
		
		list<struct User> :: iterator it = userlist.begin();
		
		struct User user;
		while(it!=userlist.end())
		{
			user = *it;
			/*if(user==user_data)
			{
				userlist.erase(it);
				break;
			}*/
			it++;
		}
		if(it==userlist.end())
			return false;
		else
			return true;
	}
	else
	{
		cout << "User is not active" << endl;
		return false;
	}
}

list<struct User> auth::getActiveUsers(string username)
{
	if(activeusers.find(username)!=activeusers.end())
	{
		return activeusers[username];
	}
	else
	{
		cout << "User is not active" << endl;
		list<struct User> userlist;
		return userlist;
	}
}