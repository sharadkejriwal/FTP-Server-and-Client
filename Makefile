all:

	g++ server_main.cpp server.cpp ftp_common.cpp auth.cpp sys.cpp -std=c++11 -o server 
	g++ client_main.cpp client.cpp ftp_common.cpp sys.cpp -std=c++11 -o client