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
const unsigned MAXBUFLEN = 512;
int sockfd;

void *process_connection(void *arg)
{
    cout<<"-----------------------10"<<endl;
    int n;
    char buf[MAXBUFLEN];
    pthread_detach(pthread_self());
    while (1)
    {
        cout<<"-----------------------11"<<endl;
        n = read(sockfd, buf, MAXBUFLEN);
        if (n <= 0)
        {
            if (n == 0)
            {
                cout << "server closed" << endl;
            }
            else
            {
                cout << "something wrong" << endl;
            }
            close(sockfd);
            // we directly exit the whole process.
            exit(1);
        }
        cout<<"-----------------------12"<<endl;
        buf[n] = '\0';
        cout << buf << endl;
        memset(buf, 0, sizeof buf);
        cout<<"-----------------------13"<<endl;
    }
}

//const unsigned serv_port = 5100;

int main(int argc, char **argv)
{
    bool logged = 0;
    char buf[MAXBUFLEN];
	char buf1[MAXBUFLEN];
	char buffer[MAXBUFLEN];
	char buffer1[MAXBUFLEN];
	char output[MAXBUFLEN];
    int maxf, maxf1;
	fd_set  rset, orig_set;
	fd_set  rset1, orig_set1;
	ssize_t n, m;
    int rv, flag;

    struct addrinfo hints, *res, *ressave;
    pthread_t tid;

    if (argc != 3)
    {
        cout << "echo_client server_name_or_ip port" << endl;
        exit(1);
    }

    cout << argv[1] << " " << argv[2] << endl;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0)
    {
        cout << "getaddrinfo wrong: " << gai_strerror(rv) << endl;
        exit(1);
    }

    ressave = res;
    flag = 0;
    do
    {
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0)
	    continue;
	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0) {
	    flag = 1;
	    break;
	}
	close(sockfd);
    }
    while ((res = res->ai_next) != NULL);
    freeaddrinfo(ressave);

    if (flag == 0)
    {
        fprintf(stderr, "cannot connect\n");
        exit(1);
    }

    pthread_create(&tid, NULL, &process_connection, NULL);

    string oneline;
    while (getline(cin, oneline))
    {
        cout<<"-----------------------1"<<endl;
        if (oneline == "exit")
        {
            close(sockfd);
            return 0;
        }
        else
        {
            cout<<"-----------------------2"<<endl;
            string command;
            command = oneline;
            command = command + " ";
            string cmd1, cmd, cmd2;
            cmd = command;
            cmd1 = "";
            cmd2 = "";
            int pos1 = command.find(" ");
            command.pop_back();
            int len = command.length();
            cmd1 = cmd.erase(pos1, len);
            cmd2 = command.erase(0, pos1+1);

            if(cmd1 == "login")
            {
                cout<<"-----------------------3"<<endl;
                write(sockfd, oneline.c_str(), oneline.length());
                logged = 1;
            }
            cout<<"-----------------------4"<<endl;
            else if(cmd1 == "logout" && logged == 1)
            {
                write(sockfd, oneline.c_str(), oneline.length());
                logged = 0;
            }
            else if(cmd1 == "chat" && logged == 1)
            {
                cout<<"-----------------------5"<<endl;
                write(sockfd, oneline.c_str(), oneline.length());
            }
            cout<<"-----------------------6"<<endl;
            else
            {
                cout<<"You need to log in first"<<endl;
            }
        }
    }
    exit(0);
}

