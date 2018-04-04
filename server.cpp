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
#include <map>
#include <chrono>


int nbites_4 = 4;
struct sockaddr_in stSockAddr;
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
char buffer[256];
int n;
std::map<std::string,int> clients;

void acceptClient(int ConnectFD);
void write2(int ConnectFD,std::string prnt, std::string act);
void read2(int ConnectFD, char *buffer);

void fillZeros(std::string &st, int nroBytes){ // complete number with zeross  =)
	std::string aux = std::to_string(st.size());
	int dif = nroBytes - int(aux.size());
	st = aux + st;
	for (int i = 0; i < dif; i++)
		st = "0" + st;
}

bool find_nick(std::string st){ //find  a  nickname is equal to st

	std::map<std::string, int>::iterator it;
	for (it = clients.begin(); it != clients.end(); it++)
		if(it->first == st){
			return true;
		}
	return false;
}

void find_str(int id,std::string & st){//return nickanme found their number socket  

	std::map<std::string, int>::iterator it;
	for (it = clients.begin(); it != clients.end(); it++)
		if(it->second == id){
			st=it->first;
		}
}


void read2(int ConnectFD, char *buffer){

	int n;

	for (;;){
		bzero(buffer, 255);
		do{
			n = read(ConnectFD, buffer, 4);
			int size_txt=atoi(buffer);
			bzero(buffer, 4);

			n = read(ConnectFD, buffer, 1);
			std::string action(buffer);
			bzero(buffer, 1);

			if (action == "P"){ //Protocolo for Printing
				
				std::string prnt="";
				std::map<std::string, int>::iterator it;
				for (it = clients.begin(); it != clients.end(); it++){
					prnt+="username: "+it->first
					+" value: " + std::to_string(it->second)+"\n";
				}
				std::cout<<"Print: "<<prnt<<std::endl; // print has all clients 
				write2(ConnectFD,prnt,action); 
			}

			else if (action == "L"){//protocolo for Login
				
				if(find_nick(std::string(buffer)) == true){ // find  a new nickname is equal to other already exists
					std::string err="nickname already exists, enter other\n";
					write2(ConnectFD,err.c_str(),action);
				}
				n = read(ConnectFD, buffer, size_txt);

				clients[buffer] = ConnectFD; //adding a newclient
				std::cout << "Login: " << buffer << std::endl;
			}

			else if (action == "C"){ //protocolo for chating

				std::string username = "";
				find_str(ConnectFD,username); //username has nickname who send to mssg 

				n = read(ConnectFD, buffer, 2); //reading a size of the other client
				int size_othername=atoi(buffer);
				bzero(buffer, 2);

				n = read(ConnectFD, buffer, size_othername); //reading a nickname the other client
				std::string othername(buffer);
				bzero(buffer, size_othername);
				if(find_nick(othername)==false){ //check if othername exists
					std::string err = "nickname not found, enter other\n";
					write2(ConnectFD, err.c_str(), action);
					break;
				}	

				int size_msg= size_txt-3-othername.size();// size has the size the real mssg
				// cout << "size_msg: " << size_msg<<endl;
				n = read(ConnectFD, buffer, size_msg);
				std::string msg(buffer);
				bzero(buffer,size_msg);
				
				int otherConnectFD = clients.find(othername)->second; //finding socket number the other client for send to mssg 
				if (otherConnectFD < 0){
					perror("error in nickname");
				}
				
				msg = username+": "+msg; //msg final
				std::cout<<msg+" -> "+othername<<std::endl;
				write2(otherConnectFD, msg, action);
			}
			else if (action == "E"){//protocol for End
				std::cout<<"End\n";
				std::vector<std::string> V;
				for (auto it=clients.begin();it!=clients.end();it++){
					if(it->second==ConnectFD){
						V.push_back(it->first);
					}
				}
				for (int i=0;i<V.size();i++){
					clients.erase(V[i]);
				}
				write2(ConnectFD,"Te has retirado del chat\nYa no te podran enviar mensajes","C");
				close(ConnectFD);
				return;
			}
			else if (action == "F"){//protocol for File
			//here file
						
			}
			else // this is can be better, you can do it =)
			std::cout << "error in action, msg bad\n";

		} while (n == 0);
		// printf("client: %s\n", buffer);
	}
}

void write2(int ConnectFD, std::string mssg, std::string act)
{

	if (act == "P" or act == "C" or act == "L") // L is when a nickname is repeat 
	{
		mssg = "R" +mssg;
		fillZeros(mssg,4);
		write(ConnectFD, mssg.c_str(), mssg.size());
	}

}
void acceptClient(int ConnectFD)
{
	if (0 > ConnectFD)
	{
		perror("error accept failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	write(ConnectFD, "Conectado", 9);
	bzero(buffer, 256);
	//changing to detach();
	std::thread(read2, ConnectFD,buffer).detach();

	std::this_thread::sleep_for(std::chrono::seconds(100));
}

int main(void){

	if(-1 == SocketFD){
		perror("can not create socket");
		exit(EXIT_FAILURE);
	}

	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(1100);
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
	
	while(true){
		int ConnectFD = accept(SocketFD, NULL, NULL);
		std::cout << "ConnectFD: " << ConnectFD << std::endl;
		std::thread(acceptClient,ConnectFD).detach();
	}

	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);
	return 0;
}
