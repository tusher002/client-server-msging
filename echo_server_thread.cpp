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

using namespace std;

map<string, int> user_info;
int port;
const unsigned MAXBUFLEN = 512;

void *process_connection(void *arg)
{
    int sockfd;
    ssize_t n;
    char buf[MAXBUFLEN];

    sockfd = *((int *)arg);
    free(arg);

    pthread_detach(pthread_self());
    while ((n = read(sockfd, buf, MAXBUFLEN)) > 0)
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
            reply = "Welcome " + cmd2 + "You have successfully logged in";
            write(sockfd, reply.c_str(), reply.length());
        }
        else if(cmd1 == "logout")
        {
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
            reply = "You have successfully logged in";
            write(sockfd, reply.c_str(), reply.length());
        }
        memset(buf, 0, sizeof buf);
    }
    if (n == 0)
    {
        cout << "client closed" << endl;
    }
    else
    {
        cout << "something wrong" << endl;
    }
    close(sockfd);
    return(NULL);
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
    int serv_sockfd, cli_sockfd, *sock_ptr;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t sock_len;
    pthread_t tid;

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

        cout << "remote client IP == " << inet_ntoa(cli_addr.sin_addr);
        cout << ", port == " << ntohs(cli_addr.sin_port) << endl;

        sock_ptr = (int *)malloc(sizeof(int));
        *sock_ptr = cli_sockfd;

        pthread_create(&tid, NULL, &process_connection, (void
                                 *)sock_ptr);
    }
}
