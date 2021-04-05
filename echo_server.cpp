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
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <bits/stdc++.h>


using namespace std;

map<string, int> user_info;
int port, client_socket[300];
const unsigned MAXBUFLEN = 512;

void signalHandler( int signum )
{
    map<string, int>::iterator user_it = user_info.begin();
    string reply;
    reply = "Server Crashed";
    for(int j; j<=300; j++)
    {
        if(client_socket[j] != 0)
        send(client_socket[j], reply.c_str(), reply.length(), 0);
        user_it++;
    }
    sleep(1);
    exit(0);
}

int main()
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
	int opt = 1;
	int master_socket, addrlen, new_socket, max_clients = 300, activity, i, valread, sockfd;
	int max_sd;
	struct sockaddr_in address;

	char buf[MAXBUFLEN];
	fd_set readfds;

	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( port );

	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	cout<<"Server on port "<<port<<endl;

	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	addrlen = sizeof(address);
	puts("Waiting for connections ...");

	while(1)
	{
	    signal(SIGINT, signalHandler);
		FD_ZERO(&readfds);

		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		for ( i = 0 ; i < max_clients ; i++)
		{
			sockfd = client_socket[i];

			if(sockfd > 0)
				FD_SET( sockfd , &readfds);

			if(sockfd > max_sd)
				max_sd = sockfd;
		}

		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}

		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			cout << "remote client IP == " << inet_ntoa(address.sin_addr);
            cout << ", port == " << ntohs(address.sin_port) << endl;

			for (i = 0; i < max_clients; i++)
			{
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					break;
				}
			}
		}

		for (i = 0; i < max_clients; i++)
		{
			sockfd = client_socket[i];

			if (FD_ISSET( sockfd , &readfds))
			{
				if ((valread = read( sockfd , buf, MAXBUFLEN)) == 0)
				{
					getpeername(sockfd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					cout << "Client disconnected: IP == " << inet_ntoa(address.sin_addr);
                    cout << ", port == " << ntohs(address.sin_port) << endl;

					close( sockfd );
					client_socket[i] = 0;
				}

				else
				{
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
                        send(sockfd, reply.c_str(), reply.length(), 0);
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
                        send(sockfd, reply.c_str(), reply.length(), 0);
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
                                send(sockfd, reply.c_str(), reply.length(), 0);
                            }
                            else
                            {
                                send(it->second, msg.c_str(), msg.length(), 0);
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
                                    send(user_it->second, cmd2.c_str(), cmd2.length(), 0);
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
