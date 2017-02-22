FTP Server and Clien

Server listens from client on control port and data port 7000 & 7001 respectively and can handle multiple connections simultaneously.

To compile, go to src and run the command 
$ make

To run server 
$ ./server

To run client
$ ./client server_ip 7000 7001					// (For servers running on same system, server_ip is 127.0.0.1)

enter username - sharad
enter password - sharad
all set to go .. 
ftp> appears

Use commands -

show -> to show a list of available Commands
pwd -> present working directory in the server
ls -> list files and folders
cd dir -> change directory to dir
get fname -> receive file from server with filename as fname
put fname -> send file to server at pwd with filename as fname
logout -> logout from current user session
exit -> exit ftp






