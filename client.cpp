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

std::vector<std::thread> T; // first,second;
struct sockaddr_in stSockAddr;
int Res;
int SocketFD ;
int n;
char buffer[255];

void fillZeros(std::string & st, int nroBytes){ // complete number with zeross =)
	std::string aux = std::to_string(st.size());
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
			n = read(SocketFD, buffer, 4); // Reading first 4 bytes
			std::string size_msg(buffer);
			bzero(buffer, 4); // Zeros for the 4 bytes that was reading   

			n = read(SocketFD, buffer, 1); //reading 1 bytes
			std::string action(buffer);
			bzero(buffer, 1); //equal to the before

			if (action == "R"){ // Responsive when is Printing or Chating or error in Login
				n = read(SocketFD, buffer, atoi(size_msg.c_str()));
				if(std::string(buffer)==""){
					return;
				}
			}
			
			else if (action == "D"){//Responsive when is file
				//here file

			}

			// n = read(SocketFD, buffer, atoi(buffer));
			
		} while (n == 0);
	}
}

void write2(int  SocketFD)
{
	std::string msg = "", aux = "", op = "";
	int dif = 0;

	while (1)
	{
		std::cout << "------Menu (action)-----\n"
			 << "P -> Print list of user on the chat \n"
			 << "L -> Login to the char\n"
			 << "C -> Send a msg to a user on the chat\n"
			 << "E -> End chat or logout from chat\n"
			 << "F -> Send a file from a user to another user\n"
			 << "----------------------------\n"
			 << std::endl;
		std::cin >> op;

		if (op == "P") {// protocolo for Print
			
			msg = "0000P";
		}

		else if (op == "L"){//protocolo for Login

			std::string username = "";
			std::cout << "enter nickname: ";
			std::cin.ignore(); 
			getline(std::cin, username); // scann with spaces
			// cin >> username;
			msg="L"+username;  // msg final
			fillZeros(msg,4);

		}
		else if (op == "C")	{ //protocolo for Chat

			std::string othername="";
			std::cout<<"enter nickname to chat: ";
			std::cin>>othername;
			fillZeros(othername,2);  
			
			std::cout<<"enter message: ";
			std::cin.ignore();
			getline(std::cin,msg); //scan with spaces
			msg="C"+othername+msg;
			fillZeros(msg,4);
		}
		else if (op == "E"){ // protocolo for End
			msg = "0000E";
		}
		else if (op == "F"){ // protocolo for File
			msg = "0000P";
		//here file
		}
		else{ // this can be better =/
			continue;
			std::cout << "error action no found, enter other\n ";
		}
		write(SocketFD, msg.c_str(), msg.size());
		if(op=="E"){
			std::cout << "End of Write\n";
			return;
		}
	}

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
	Res = inet_pton(AF_INET, "192.168.161.253", &stSockAddr.sin_addr);

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
	T.push_back(std::thread(read2, SocketFD, buffer));
	T.push_back(std::thread(write2, SocketFD));
	T[0].join();
	T[1].join();

	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);
	return 0;
}
