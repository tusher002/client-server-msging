#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <bits/stdc++.h>

using namespace std;

<<<<<<< HEAD
const unsigned port;
=======
const unsigned port = 28109;
>>>>>>> a0c61d4c4d58d79d61a5548826d9abb3f17625d5
const unsigned MAXBUFLEN = 512;

map<string, int> user_info;

string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
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
	int serv_sockfd, cli_sockfd;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t sock_len;
	pid_t child_pid;
	ssize_t n;
	char buf[MAXBUFLEN];

	cout << "port = " << port << endl;
	serv_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero((void*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	bind(serv_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	listen(serv_sockfd, 5);

	for (; ;)
    {
		sock_len = sizeof(cli_addr);
		cli_sockfd = accept(serv_sockfd, (struct sockaddr *)&cli_addr, &sock_len);

		close(serv_sockfd); // close the listen socket in child process
        while ((n = read(cli_sockfd, buf, MAXBUFLEN)) > 0)
        {
            buf[n] = '\0';
            //cout << buf << endl;
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
                user_info.insert(pair<string, int>(cmd2, cli_sockfd));
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
                write(cli_sockfd, reply.c_str(), reply.length());
            }
            else if(cmd1 == "logout")
            {
                cout<<"=========="<<cmd2<<endl;
                user_info.erase(user_info.find(cmd2));
                map<string, int>::iterator user_it = user_info.begin();
                cout << "Logged In User:"<<endl;
                while(user_it != user_info.end())
                {
                    cout << '\t' << user_it->first<< '\t' << user_it->second << '\n';
                    user_it++;
                }
                cout << endl;

                string reply;
                reply = cmd2 + ", you have successfully logged out";
                write(cli_sockfd, reply.c_str(), reply.length());
            }
            memset(buf, 0, sizeof buf);
        }

        if (n == 0)
            cout << "client closed" << endl;
        else
            cout << "something wrong" << endl;

        close(cli_sockfd);
	}
}
