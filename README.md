														CPDP Project 2: A Simple Chat Application
															Date of Submission: 04 - 08 - 2021

Name: Tusher Chandra Mondol
FSUID: TM20DU



Introduction:
This chat application has two components: a chat server and a chat clients. Chat server maintains the user account information,
and facilitate the chat functionality. Chat client sends either broadcast or targeted messages to other clients.

Project Description:
In this project I have developed a client-server chatting application which supports the following commands:
		1. login
		2. logout
		3. chat
		4. exit
Signal SIGINT, generated when a user types Ctrl-C has been handled properly.
There are 6 types of files.
	1. server_select.cpp: This is the messenger server program. It can handle multiple clients parallely. I have used select() to support I/O multiplexing.
	2. server_configuration_file.txt: This text file contains necessay configuration information for server. Such as, port number. It occupies one line with the following format:
	port: value
	3. client_thread.cpp: This is the messenger client program. I have used Pthread to support I/O multiplexing here.
	4. client_configuration_file.txt: This text file contains necessay configuration information for client. Such as, server port number. With the following format:
	servhost: value
	servport: value
	5. Makefile: Makefile tells make what to do.
	6. README.md: All the description of the project is written there.
		
Execution:
At first type 'make'.
Server must be runned before Client.
To run the Server, type './s.out'.
To run the Client, type './c.out'.

Then run the commands you want.

1. Login: logs into the server using command "login" in the format of "login username".
2. Logout: logs out from the server, in the format "logout". Note that "logout" command can only be used after the user has logged into the server.
3. Exit: quits the client program using command "exit".
4. Chat: sends a chat message in the format of "chat [@username] some_message", where @username is optional parameter of the chat command. If @username is not present in the command,
	the corresponding some_message is a broadcast message, the server sends to all the users who are currently logged in. If @username is present in the chat command, the 	some_message
	is only forwarded to the corresponding "username". Note that "chat" command can only be used after the user has logged into the server.