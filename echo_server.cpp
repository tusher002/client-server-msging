#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <bits/stdc++.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define TRUE 1
#define FALSE 0


using namespace std;

map<string, int> user_info;
int port;
const unsigned MAXBUFLEN = 512;


int main(int argc , char *argv[])
{
    fstream my_file;
    my_file.open("configuration_file.txt", ios::in);
    if (!my_file)
    {
        cout << "File not found"<<endl<<endl;
    }
    else
    {
        regex reg("(port)(.*)");
        string line;
        while ( getline (my_file, line) )
        {
          if ( regex_match(line, reg) )
          {
            string delimiter = ":";
            size_t pos = 0;
            string token;
            while ((pos = line.find(delimiter)) != std::string::npos) {
                line.erase(0, pos + delimiter.length());
                port = stoi(line);
            }
          }
        }
        my_file.close();
    }
	int opt = TRUE;
	int master_socket , addrlen , new_socket , client_socket[300] ,
		max_clients = 300 , activity, i , valread , sockfd;
	int max_sd;
	struct sockaddr_in address;

	char buf[MAXBUFLEN]; //data buffer of 1K

	//set of socket descriptors
	fd_set readfds;

	//a message
	char *message = "ECHO Daemon v1.0 \r\n";

	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	//create a master socket
	master_socket = socket(AF_INET , SOCK_STREAM , 0);


	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	//bind the socket to localhost port 8888
	bind(master_socket, (struct sockaddr *)&address, sizeof(address));


	//accept the incoming connection
	addrlen = sizeof(address);

	while(1)
	{
		//clear the socket set
		FD_ZERO(&readfds);

		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++)
		{
			//socket descriptor
			sockfd = client_socket[i];

			//if valid socket descriptor then add to read list
			if(sockfd > 0)
				FD_SET( sockfd , &readfds);

			//highest file descriptor number, need it for the select function
			if(sockfd > max_sd)
				max_sd = sockfd;
		}

		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)

			cout << "remote client IP == " << inet_ntoa(address.sin_addr);
            cout << ", port == " << ntohs(address.sin_port) << endl;



			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					break;
				}
			}
		}

		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++)
		{
			sockfd = client_socket[i];

			if (FD_ISSET( sockfd , &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread = read( sockfd , buf, MAXBUFLEN)) == 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(sockfd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
					cout << "Client disconnected: IP == " << inet_ntoa(address.sin_addr);
                    cout << ", port == " << ntohs(address.sin_port) << endl;
					//Close the socket and mark as 0 in list for reuse
					close( sockfd );
					client_socket[i] = 0;
				}

				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					buf[valread] = '\0';
					string command(buf);
                    command = command + " ";
                    string cmd1, cmd2, cmd;
                    cmd = command;
                    cmd1 = "";
                    cmd2 = "";
                    int pos1 = command.find(" ");
                    command.pop_back();
                    int n = command.length();
                    cmd2 = command.erase(0, pos1+1);
                    cmd1 = cmd.erase(pos1, n);
                    if(cmd1 == "login")
                    {
                        user_info.insert(pair<string, int>(cmd2, sockfd));
                        cout << "Logged In User:"<<endl;
                        map<string, int>::iterator user_it = user_info.begin();
                        while(user_it != user_info.end())
                        {
                            cout << '\t' << user_it->first<< '\t' << user_it->second << '\n';
                            user_it++;
                        }
                        cout << endl;

                        string reply;
                        reply = "Welcome " + cmd2 + ", you have successfully logged in";
                        write(sockfd, reply.c_str(), reply.length());
                    }
                    else if(cmd1 == "logout")
                    {
                        string user;
                        map<string, int>::iterator user_it = user_info.begin();
                        while(user_it != user_info.end())
                        {
                            if(user_it->second == sockfd)
                            {
                                user = user_it->first;
                                break;
                            }
                            user_it++;
                        }
                        user_info.erase(user_info.find(user));
                        user_it = user_info.begin();
                        cout << "Logged In User:"<<endl;
                        while(user_it != user_info.end())
                        {
                            cout << '\t' << user_it->first<< '\t' << user_it->second << '\n';
                            user_it++;
                        }
                        cout << endl;

                        string reply;
                        reply = "You have successfully logged out";
                        write(sockfd, reply.c_str(), reply.length());
                    }
                    else if(cmd1 == "chat")
                    {
                        cmd2 = cmd2 + " ";
                        if(cmd2[0] == '@')
                        {
                            string msg;
                            int m_pos;
                            msg = cmd2;
                            m_pos = command.find(" ");
                            int m_len = cmd2.length();
                            msg = msg.erase(0, m_pos+1);
                            map<string, int>::iterator user_it = user_info.begin();
                            while(user_it != user_info.end())
                            {
                                if(user_it->second == sockfd)
                                {
                                    msg = user_it->first + " >> " + msg;
                                    break;
                                }
                                user_it++;
                            }
                            cmd2 = cmd2.erase(m_pos, m_len);
                            string user = cmd2.erase(0, 1);
                            map<string, int>::iterator it ;
                            it = user_info.find(user);
                            if(it == user_info.end())
                            {
                                string reply;
                                reply = "The person you have tried to send the message is unavailable right now";
                                write(sockfd, reply.c_str(), reply.length());
                            }
                            else
                            {
                                write(it->second, msg.c_str(), msg.length());
                            }
                        }
                        else
                        {
                            map<string, int>::iterator user_it = user_info.begin();
                            while(user_it != user_info.end())
                            {
                                if(user_it->second == sockfd)
                                {
                                    cmd2 = user_it->first + " >> " + cmd2;
                                    break;
                                }
                                user_it++;
                            }
                            user_it = user_info.begin();
                            while(user_it != user_info.end())
                            {
                                if(user_it->second != sockfd)
                                {
                                    write(user_it->second, cmd2.c_str(), cmd2.length());
                                }
                                user_it++;
                            }
                        }

                    }
                    memset(buf, 0, sizeof buf);
				}
			}
		}
	}

	return 0;
}
