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
int port;
const unsigned MAXBUFLEN = 512;

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
	int master_socket , addrlen , new_socket , client_socket[300] ,
		max_clients = 300 , activity, i , valread , sockfd;
	int max_sd;
	struct sockaddr_in address;

	char buffer[MAXBUFLEN]; //data buffer of 1K

	//set of socket descriptors
	fd_set readfds;


	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( port );

	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	cout<<"Server on port "<<port<<endl;

	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");

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

		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}

		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			//inform user of socket number - used in send and receive commands
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
				if ((valread = read( sockfd , buffer, MAXBUFLEN)) == 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(sockfd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
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
					buffer[valread] = '\0';
					send(sockfd , buffer , strlen(buffer) , 0 );
				}
			}
		}
	}

	return 0;
}
