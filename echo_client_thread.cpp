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
    int n;
    char buf[MAXBUFLEN];
    pthread_detach(pthread_self());
    while (1)
    {
        n = read(sockfd, buf, MAXBUFLEN);
        if (n <= 0)
        {
            if (n == 0)
            {
                cout << "server closedd" << endl;
            }
            else
            {
                cout << "something wrong" << endl;
            }
            close(sockfd);
            // we directly exit the whole process.
            exit(1);
        }
        buf[n] = '\0';
        cout<<"-----------------------------ashche-----------------"<<endl;
        cout << buf << endl;
    }
}

//const unsigned serv_port = 5100;

int main(int argc, char **argv)
{
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

    //pthread_create(&tid, NULL, &process_connection, NULL);

    FD_ZERO(&orig_set);
    FD_SET(STDIN_FILENO, &orig_set);
    FD_SET(sockfd, &orig_set);
    if (sockfd > STDIN_FILENO) maxf = sockfd+1;
    else maxf = STDIN_FILENO+ 1;

    while (1)
    {
        rset = orig_set;
        select(maxf, &rset, NULL, NULL, NULL);
        if (FD_ISSET(sockfd, &rset))
        {
            if (read(sockfd, buf, MAXBUFLEN) == 0)
            {
                //printf("Server crashed.\n");
                close(sockfd);
                exit(0);
            }
            printf("Server response : %s\n", buf);
        }

        if (FD_ISSET(STDIN_FILENO, &rset))
        {
            if (fgets(buffer, MAXBUFLEN, stdin) == NULL)
                {
                    close(sockfd);
                    exit(0);
                }

            string oneline(buffer);
            if (oneline == "exit")
            {
                close(sockfd);
                return 0;
            }
            else
            {
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
                    write(sockfd, oneline.c_str(), oneline.length());
                    n = read(sockfd, buf, MAXBUFLEN);
                    if (n <= 0)
                    {
                        if (n == 0)
                        {
                            cout << "Server closed" << endl;
                        } else
                        {
                            cout << "something wrong" << endl;
                        }
                        close(sockfd);
                        exit(0);
                    }

                    buf[n] = '\0';
                    cout << buf << endl;
                    string command2;
                    cout<<"--------------------1"<<endl;

                    FD_ZERO(&orig_set);
                    FD_SET(STDIN_FILENO, &orig_set);
                    FD_SET(sockfd, &orig_set);
                    if (sockfd > STDIN_FILENO) maxf = sockfd+1;
                    else maxf = STDIN_FILENO+ 1;
                    cout<<"--------------------2"<<endl;
                    while (1)
                    {
                        cout<<"--------------------3"<<endl;
                        rset = orig_set;
                        select(maxf, &rset, NULL, NULL, NULL);
                        cout<<"--------------------4"<<endl;
                        if (FD_ISSET(sockfd, &rset))
                        {
                            if (read(sockfd, buf1, MAXBUFLEN) == 0)
                            {
                                //printf("Server crashed.\n");
                                close(sockfd);
                                exit(0);
                            }
                            printf("Server response : %s\n", buf);
                        }
                        else if (FD_ISSET(STDIN_FILENO, &rset))
                        {
                            cout<<"--------------------5"<<endl;
                            if (fgets(buffer1, MAXBUFLEN, stdin) == NULL)
                            {
                                close(sockfd);
                                exit(0);
                            }
                            string command2(buffer1);
                            string cmd_nxt;
                            cmd_nxt = command2;
                            cmd_nxt = cmd_nxt + " ";
                            string cmd1_nxt;
                            cmd1_nxt = "";
                            int pos_next = cmd_nxt.find(" ");
                            int len2 = cmd_nxt.length();
                            cmd1_nxt = cmd_nxt.erase(pos_next, len2);
                            cout<<"--------------------6"<<endl;
                            cout<<cmd1_nxt<<endl;

                            if(cmd1_nxt == "logout")
                            {
                                command2 = command2 + " " + cmd2;
                                write(sockfd, command2.c_str(), command2.length());
                                m = read(sockfd, output, MAXBUFLEN);
                                if (m <= 0)
                                {
                                    if (m == 0)
                                    {
                                        cout << "server closed" << endl;
                                    } else
                                    {
                                        cout << "something wrong" << endl;
                                    }
                                    close(sockfd);
                                    exit(0);
                                }

                                output[m] = '\0';
                                cout << output << endl;
                                break;
                            }
                            else
                            {
                                cout<<"doing well-----------"<<endl;
                            }
                        }
                    }
                }
                else
                {
                    cout<<"You need to log in first"<<endl;
                }
            }
        }

    }
    exit(0);
}

