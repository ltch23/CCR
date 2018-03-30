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

int nbites =4;
vector<thread> T; // first,second;
void read2(int SocketFD, char * buffer ){
	int n;
	for (;;){
		bzero(buffer,100);
		do{
			n= read(SocketFD,buffer,4);
			n = read(SocketFD, buffer, atoi(buffer));
		} while (n==0);
		printf("client: %s\n",buffer);
	}
}

void write2(int SocketFD)
{
	string st="", aux = "";
	int dif = 0;
	while (1)
	{
		// cout << "server: ";
		getline(cin, st);
		if(st=="exit") {
			// T[0].join();
			break;
		}
		aux = to_string(st.size());
		dif = nbites - int(aux.size());
		st = aux + st;
		for (int i = 0; i < dif; i++)
			st = "0" + st;
		write(SocketFD, st.c_str(), st.size());
	}


	// cout << "sali de write" << endl;
}

int main(void){
	struct sockaddr_in stSockAddr;
	int Res;
	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	int n;

	if (-1 == SocketFD){
		perror("cannot create socket");
		exit(EXIT_FAILURE);
	}

	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(1000);
	Res = inet_pton(AF_INET, "192.168.0.6", &stSockAddr.sin_addr);

	if (0 > Res){
		perror("error: first parameter is not a valid address family");
		close(SocketFD);
		exit(EXIT_FAILURE);
	} else if (0 == Res){
		perror("char string (second parameter does not contain valid ipaddress");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in))){
		perror("connect failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	char buffer[255];
	//n = write(SocketFD,"Hi, whore",18);
	/* perform read write operations ... */
	n=read(SocketFD,buffer,9);
	printf("%s\n",buffer);
	T.push_back (thread(read2,SocketFD,buffer));
	T.push_back (thread(write2,SocketFD));
	T[0].join();
	T[1].join();
	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);
	return 0;
}
