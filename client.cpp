/* Client code in C */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <vector>

using namespace std;

vector<thread> T; // first,second;
struct sockaddr_in stSockAddr;
int Res;
int SocketFD ;
int n;
char buffer[255];

void fillZeros(string & st, int nroBytes){
	string aux = to_string(st.size());
	int dif = nroBytes - int(aux.size());
	st = aux + st;
	for (int i = 0; i < dif; i++)
		st = "0" + st;
}

void read2(int SocketFD, char *buffer)
{
	int n;
	for (;;)
	{
		bzero(buffer, 100);
		do
		{
			// cout<<"Reading\n";
			n = read(SocketFD, buffer, 4);
			string size_msg(buffer);
			bzero(buffer, 4);

			n = read(SocketFD, buffer, 1);
			string action(buffer);
			bzero(buffer, 1);

			if (action == "R"){
				n = read(SocketFD, buffer, atoi(size_msg.c_str()));
			}
			
			else if (action == "D"){

			}

			

			// n = read(SocketFD, buffer, atoi(buffer));
		} while (n == 0);
		cout << buffer << endl;
	}
}

void write2(int  SocketFD)
{
	string msg = "", aux = "", op = "";
	int dif = 0;

	while (1)
	{
		cout << "------Menu (action)-----\n"
			 << "P -> Print list of user on the chat \n"
			 << "L -> Login to the char\n"
			 << "C -> Send a msg to a user on the chat\n"
			 << "E -> End chat or logout from chat\n"
			 << "F -> Send a file from a user to another user\n"
			 << "----------------------------\n"
			 << endl;
		cin >> op;

		if (op == "P")
		{
			msg = "0000P";
		}

		else if (op == "L")
		{
			string username = "";
			cout << "enter nickname: ";
			// getline(cin, username);
			cin >> username;
			msg="L"+username;
			fillZeros(msg,4);

		}
		else if (op == "C"){
			string othername="";
			cout<<"enter nickname to chat: ";
			cin>>othername;
			fillZeros(othername,2);
			cout<<"enter message: ";
			cin.ignore();
			getline(cin,msg);
			msg="C"+othername+msg;
			fillZeros(msg,4);
		}
		else if (op == "E"){
			msg = "0000E";
		}
		else if (op == "F"){
		}
		else{
			cout << "error action no find\n";
			break;
		}
		write(SocketFD, msg.c_str(), msg.size());
	}

	// cout << "sali de write" << endl;
}

int main(){
	
	SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (-1 == SocketFD)
	{
		perror("cannot create socket");
		exit(EXIT_FAILURE);
	}

	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(1100);
	Res = inet_pton(AF_INET, "192.168.0.6", &stSockAddr.sin_addr);

	if (0 > Res)
	{
		perror("error: first parameter is not a valid address family");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	else if (0 == Res)
	{
		perror("char string (second parameter does not contain valid ipaddress");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
	{
		perror("connect failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	n = read(SocketFD, buffer, 9);
	printf("%s\n", buffer);
	T.push_back(thread(read2, SocketFD, buffer));
	T.push_back(thread(write2, SocketFD));
	T[0].join();
	T[1].join();

	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);
	return 0;
}
