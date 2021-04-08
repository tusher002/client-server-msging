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
#include <gtkmm.h>

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
        string reply(buf);
        if(reply == "Server Crashed")
        {
            cout << "server closed" << endl;
            close(sockfd);
            exit(1);
        }
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

        buf[n] = '\0';
        cout << buf << endl;
        memset(buf, 0, sizeof buf);
    }
}

void signalHandler(int signum)
{
    close(sockfd);
    exit(1);
}

//const unsigned serv_port = 5100;

int main(int argc, char* argv[])
{
    Gtk:Main kit(argc, argv);
    Gtk::Window window;
    window.set_default_size(600, 360);
    window.set_title("Chatting App");
    window.set_position(Gtk::WIN_POS_CENTER);
    window.set_border_width(10);
    Gtk::Main::run(window);
    string ip, port;
    fstream my_file;
    my_file.open("chat_configuration_file.txt", ios::in);
    if (!my_file)
    {
        cout << "File not found"<<endl<<endl;
    }
    else
    {
        regex p_reg("(servport)(.*)");
        regex i_reg("(servhost)(.*)");
        string line;
        while ( getline (my_file, line) )
        {
            if ( regex_match(line, i_reg) )
            {
                string delimiter = ":";
                size_t pos = 0;
                string token;
                while ((pos = line.find(delimiter)) != std::string::npos)
                {
                    ip = line.erase(0, pos + delimiter.length());
                }
            }
            if ( regex_match(line, p_reg) )
            {
                string delimiter = ":";
                size_t pos = 0;
                string token;
                while ((pos = line.find(delimiter)) != std::string::npos)
                {
                    port = line.erase(0, pos + delimiter.length());
                }
            }
        }
        my_file.close();
    }
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

    cout << ip << " " << port << endl;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(ip.c_str(), port.c_str(), &hints, &res)) != 0)
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

    signal(SIGINT, signalHandler);

    pthread_create(&tid, NULL, &process_connection, NULL);

    string oneline;
    while (getline(cin, oneline))
    {
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
                logged = 1;
            }
            else if(cmd1 == "logout" && logged == 1)
            {
                write(sockfd, oneline.c_str(), oneline.length());
                logged = 0;
            }
            else if(cmd1 == "chat" && logged == 1)
            {
                write(sockfd, oneline.c_str(), oneline.length());
            }
            else if((cmd1 != "chat" && cmd1 == "logout") && logged == 1)
            {
                cout<<"Invalid command"<<endl;
            }
            else
            {
                cout<<"You need to log in first"<<endl;
            }
        }
    }
    exit(0);
}

