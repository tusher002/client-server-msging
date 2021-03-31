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
#include <bits/stdc++.h>

using namespace std;

//const unsigned serv_port = 5100;
const unsigned MAXBUFLEN = 512;

map<int, int> gquiz1;

int main(int argc, char **argv)
{
	int sockfd, rv, flag;
	ssize_t n, m;
	char buf[MAXBUFLEN];
	char output[MAXBUFLEN];
	struct addrinfo hints, *res, *ressave;

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
	string oneline;


	/* Does this program have problems? Yes.
	 * The client may be blocked forever after the server crashes or closes
	 * the connection. FOREVER: until user types a line. Otherwise, the client
	 * program is blocked on getline().
	 */
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
                n = read(sockfd, buf, MAXBUFLEN);
                if (n <= 0)
                {
                    if (n == 0)
                    {
                        cout << "server closed" << endl;
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
                while (getline(cin, command2))
                {
                    string cmd_nxt;
                    cmd_nxt = command2;
                    cmd_nxt = cmd_nxt + " ";
                    string cmd1_nxt;
                    cmd1_nxt = "";
                    int pos_next = cmd_nxt.find(" ");
                    int len2 = cmd_nxt.length();
                    cmd1_nxt = cmd_nxt.erase(pos_next, len2);

                    if(cmd1_nxt == "logout")
                    {
                        cout<<"---------"<<endl<<cmd2<<endl;
                        command2 = command2 + " " + cmd2;
                        write(sockfd, command2.c_str(), command2.length());
                        m = read(sockfd, output, MAXBUFLEN);
                        if (m <= 0)
                        {
                            if (m == 0)
                            {
                                cout << "server closedd" << endl;
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
                }
            }
            else
            {
                cout<<"You need to log in first"<<endl;
            }
		}
	}
}

