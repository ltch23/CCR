  /* Server code in C */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <vector>

int nbites = 4;

using namespace std;

void read2(int SocketFD, char *buffer)
{
	int n;
	for (;;)
	{
		bzero(buffer, 255);
		do
		{
			n = read(SocketFD, buffer, 4);
			n = read(SocketFD, buffer, atoi(buffer));
		} while (n == 0);
		printf("client(Cutire): %s\n", buffer);
	}
}

void write2(int SocketFD)
{
	string st,aux="";
	int dif=0;
	for (;;)
	{
		// cout << "server: ";
		getline(cin,st);
		aux = to_string(st.size());
		dif = nbites - int(aux.size());
		st = aux+st;
		for(int i=0;i<dif;i++)
			st="0"+st;
		write(SocketFD, st.c_str(), st.size());
	}
	// cout << "sali de write" << endl;
}

int main(void){
	struct sockaddr_in stSockAddr;
	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	char buffer[256];
	int n;

	if(-1 == SocketFD){
		perror("can not create socket");
		exit(EXIT_FAILURE);
	}

	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(1000);
	stSockAddr.sin_addr.s_addr = INADDR_ANY;

	if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in))){
		perror("error bind failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	if(-1 == listen(SocketFD, 10)){
		perror("error listen failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	int ConnectFD = accept(SocketFD, NULL, NULL);
	if(0 > ConnectFD){
		perror("error accept failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	/* perform read write operations ... */
	vector<thread> T;
	write(ConnectFD,"Conectado",9);
	bzero(buffer, 256);
	T.push_back(thread(write2, ConnectFD));
	T.push_back(thread(read2, ConnectFD, buffer));
	T[1].join();
	T[0].join();
	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);
	return 0;
}
