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
//#include <conio.h>
#include <errno.h>
//#include <io.h>

std::vector<std::thread> T; // first,second;
struct sockaddr_in stSockAddr;
int Res;
int SocketFD ;
int n;
char buffer[255];

void fillZeros(std::string & st, int nroBytes,bool with_act=1){ // complete number with zeross =)
	std::string aux = std::to_string(st.size()-with_act);
	int dif = nroBytes - int(aux.size());
	st = aux + st;
	for (int i = 0; i < dif; i++)
		st = "0" + st;
}

std::string fillZeros(int aux_size,int nroBytes){ // complete number with zeross =)
	std::string aux = std::to_string(aux_size);
	int dif = nroBytes - int(aux.size());
	for (int i = 0; i < dif; i++)
		aux = "0" + aux;
	return aux;
}

void read2(int SocketFD, char *buffer)
{
	int n;
	for (;;)
	{
		bzero(buffer, 100);
		do
		{
			n = read(SocketFD, buffer, 4); // Reading first 4 bytes
			if(n==0 && T[1].joinable()==false){//FINALIZANDO CONEXION
				return;
			}
			int size_msg=atoi(buffer);
			bzero(buffer, 4); // Zeros for the 4 bytes that was reading   

			n = read(SocketFD, buffer, 1); //reading 1 bytes
			std::string action(buffer);
			bzero(buffer, 1); //equal to the before

			if (action == "R"){ // Responsive when is Printing or Chating or error in Login
				n = read(SocketFD, buffer, size_msg);
				printf ("%s\n", buffer);
				bzero(buffer,size_msg);
				/*if(std::string(buffer)==""){
					return;
				}*/
			} else if (action == "D"){//Responsive when is file
				n=read(SocketFD, buffer,2);
				int size_othername=atoi(buffer);
				bzero(buffer,2);
				n=read(SocketFD,buffer,size_othername);
				std::string othername(buffer);	//othername
				bzero(buffer, size_othername);
				//size_msg= size_txt;
				n=read(SocketFD,buffer,size_msg);
				std::string msg(buffer);	//filename
				bzero(buffer,size_msg);
				n=read(SocketFD, buffer, 4);
				//std::string str_size_file(buffer);
				int size_file=atoi(buffer);
				bzero(buffer,4);
				n=read(SocketFD, buffer,size_file);
				std::string msg_file(buffer);	//msg_file
				bzero(buffer,size_file);
				FILE *newFile=fopen(msg.c_str(),"w");
				for (int i=0;i<msg_file.size();i++){
					fprintf(newFile, "%c", msg_file[i]);
				}
				std::cout << othername << " te envió: " << msg << std::endl; 
				fclose(newFile);
			}

			// n = read(SocketFD, buffer, atoi(buffer));
			
		} while (n == 0);
	}
}

void write2(int  SocketFD) {
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
			//Protocolo:	
			msg = 	std::string("0000")+	// size of msg
				"P";	// P

		} else if (op == "L"){//protocolo for Login

			std::string nickname = "";
			std::cout << "enter nickname: ";
			std::cin.ignore(); 
			getline(std::cin, nickname); // scann with spaces
			//Protocolo:
			msg=	fillZeros(nickname.size(),4)+	// size of Nickname(4)
				"L"+				// L
				nickname;			// nickname

		} else if (op == "C")	{ //protocolo for Chat

			std::string nickname="";
			std::cout<<"enter nickname to chat: ";
			std::cin.ignore();
			getline(std::cin, nickname); //scan with spaces
			std::cout<<"enter message: ";
			//std::cin.ignore();
			getline(std::cin,msg); //scan with spaces
			//Protocolo:
			msg=	fillZeros(msg.size(),4)+	// size of msg(4)
				"C"+ 				// C
				fillZeros(nickname.size(),2)+	// nickname size(2)
				nickname+			// nickname
				msg;				// msg
			std::cout << msg << std::endl;

		} else if (op == "E"){ // protocolo for End
			//Protocolo:
			msg = 	std::string("0000") +			// size of msg(4)
				"E";				// E
		}
		else if (op == "F"){ // protocolo for File
			std::string file="",filename="",nickname="";
			std::cout << "enter nickname to send file: ";
			std::cin.ignore(); 
			getline(std::cin, nickname); // scann with spaces
			std::cout << "enter filename: ";
			getline(std::cin, filename); // scann with spaces
			if(FILE *file_content=fopen(filename.c_str(),"r+")){
				char c;
				while(!feof(file_content)){
					fscanf(file_content,"%c",&c);
					file+=c;
				}
				fclose(file_content);
				//Protocolo:
				msg=	fillZeros(filename.size(),4) +	// size of filename(4)
					"F" +				// F
					fillZeros(nickname.size(),2) +	// nickname size(2)
					nickname +			// nickaname
					filename +			// filename
					fillZeros(file.size(),4) +	// file's size(4)
					file;				// file
			} else {
				std::cout << "No existe el archivo" << std::endl;
				continue;
			}
		}
		else{ // this can be better =/
			msg = "0000P";std::cout << "error action no found, enter other\n ";
			continue;
		}
		//std::cout << msg <<std::endl;
		int nwrite = write(SocketFD, msg.c_str(), msg.size());
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
	stSockAddr.sin_port = htons(1101);
	Res = inet_pton(AF_INET, "192.168.160.240", &stSockAddr.sin_addr);

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
	T.resize(2);
	T[0]=(std::thread(read2, SocketFD, buffer));
	T[1]=(std::thread(write2, SocketFD));
	T[1].join();
	T[0].join();

	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);
	return 0;
}
