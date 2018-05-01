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
#include <errno.h>
#include <ncurses.h>
#include "utilidad.h"
#include "juego.h"

/*VARIABLES Y FUNCIONES*/

struct sockaddr_in stSockAddr;
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP),n;
std::map<std::string,int> clients;
std::map<std::string, int>::iterator it;
game Game;

void acceptClient(int ConnectFD);
bool write2(int ConnectFD,std::string prnt, std::string act);
void read2(int ConnectFD);

/***********************/

bool find_nick(std::string st){ //find  a  nickname is equal to st

	for (it = clients.begin(); it != clients.end(); it++)
		if(it->first == st){
			return true;
		}
	return false;
}

void find_str(int id,std::string & st){//return nickanme found their number socket  

	for (it = clients.begin(); it != clients.end(); it++)
		if(it->second == id){
			st=it->first;
		}
}

std::string protocoloGame(std::string affect){
	return fillZeros(affect.size(),4)+"G"+affect+
		fillZeros(Game.players[affect].startx,4)+
		fillZeros(Game.players[affect].starty,4)+
		fillZeros(Game.players[affect].lives,2);
}

void read2(int ConnectFD){
	char buffer[256];

	int n;
	bool login=false;

	for (;;){
		bzero(buffer, 255);
		do{
			n = read(ConnectFD, buffer, 4);
			if(n==0){
				std::vector<std::string> V;
				for (auto it=clients.begin();it!=clients.end();it++){
					if(it->second==ConnectFD){
						V.push_back(it->first);
						if(Game.players.find(it->first)!=Game.players.end()){
							Game.players[it->first].lives=0;
						}
					}									
				}
				for (int i=0;i<V.size();i++){
					if(Game.players.find(V[i])!=Game.players.end()){
						std::string msg=protocoloGame(V[i]);
						std::cout << msg << "\n";
						for (auto it=clients.begin();it!=clients.end();it++){
							if(it->second!=ConnectFD){
								int nwrite=write(it->second,msg.c_str(),msg.size());
							}
						}
					}
					clients.erase(V[i]);
					Game.players.erase(Game.players.find(V[i]));
				}
				close(ConnectFD);
				return;
			}
			std::string sendFile(buffer);
			int size_txt=atoi(buffer);
			bzero(buffer, 4);

			n = read(ConnectFD, buffer, 1);
			std::string action(buffer);
			bzero(buffer, 1);

			if (action == "P"){ //Protocolo for Printing
				std::string prnt="";
				for (it = clients.begin(); it != clients.end(); it++){
					prnt+="username: "+it->first
					+" value: " + std::to_string(it->second) + "\n";
				}
				std::cout<<"Print:\n"<<prnt<<std::endl; // print has all clients 
				if(login){
					write2(ConnectFD,prnt,action); 
				} else {
					prnt="no estas logueado";
					write2(ConnectFD,prnt,action); 
				}
			} else if (action == "L"){//protocolo for Login
				login=true;
				
				n = read(ConnectFD, buffer, size_txt);
				if(find_nick(std::string(buffer)) == true){ // find  a new nickname is equal to other already exists
					std::string err="nickname already exists, enter other";
					write2(ConnectFD,err.c_str(),action);
					continue;
				}

				clients[buffer] = ConnectFD; //adding a newclient
				std::cout << "Login: " << buffer << std::endl;
				for (Game.it = Game.players.begin(); Game.it != Game.players.end(); Game.it++){
					std::string msg=protocoloGame(Game.it->first);
					int nwrite= write(ConnectFD, msg.c_str(), msg.size());
					std::cout<<msg+" -> "+it->first<<std::endl;
				}
			} else if (action == "C"){ //protocolo for chating
				std::string username = "";
				find_str(ConnectFD,username); //username has nickname who send to mssg 

				n = read(ConnectFD, buffer, 2); //reading a size of the other client
				int size_othername=atoi(buffer);
				bzero(buffer, 2);

				n = read(ConnectFD, buffer, size_othername); //reading a nickname the other client
				std::string othername(buffer);
				bzero(buffer, size_othername);

				int size_msg= size_txt;// size has the size the real mssg
				// cout << "size_msg: " << size_msg<<endl;
				n = read(ConnectFD, buffer, size_msg);
				std::string msg(buffer);
				bzero(buffer,size_msg);
				if(find_nick(othername)==false){ //check if othername exists
					std::string err = "nickname not found, enter other\n";
					write2(ConnectFD, err.c_str(), action);
					continue;
				}	
				
				int otherConnectFD = clients.find(othername)->second; //finding socket number the other client for send to mssg 
				if (otherConnectFD < 0){
					perror("error in nickname");
				}
				
				msg = username+": "+msg; //msg final
				std::cout<<msg+" -> "+othername<<std::endl;
				if(login){
				write2(otherConnectFD, msg, action);
				} else {
					msg="no estas conectado\n";
					write2(otherConnectFD, msg, action);
				}

			} else if (action == "G"){ //protocolo for chating
				std::string username = "";

				find_str(ConnectFD,username);

				int size_msg= size_txt;// size has the size the real mssg
				n = read(ConnectFD, buffer, size_msg);
				int msg=atoi(buffer);
				bzero(buffer,size_msg);
				std::cout << "size: " << char(msg) << std::endl;
				std::string affect=username;
				bool propagate=true;
				if(msg=='N'){
					n = read(ConnectFD, buffer, 4);
					int lines=atoi(buffer);
					bzero(buffer,4);
					n = read(ConnectFD, buffer, 4);
					int cols=atoi(buffer);
					bzero(buffer,4);
					Game.newPlayer(username,lines,cols);
					std::cout << username << " join to the game\n";
				} else if(!Game.move_user2(username,msg,affect)){
						propagate=false;
				}
				if(propagate){
					if(login){
						std::string nmsg=protocoloGame(affect),nmsg2;
						if(msg=='o'){
							nmsg2= fillZeros(username.size(),4)+
								"O"+
								username+
								fillZeros(Game.players[username].starty-Game.maxy,4);
						}
						for (it = clients.begin(); it != clients.end(); it++){
							int nwrite= write(it->second, nmsg.c_str(), nmsg.size());
							nwrite= write(it->second, nmsg2.c_str(), nmsg2.size());
							std::cout<<nmsg+" -> "+it->first<<std::endl;
							std::cout<<nmsg2+" -> "+it->first<<std::endl;
						}
					}
					if(Game.players[affect].lives==0){
						Game.players.erase(Game.players.find(affect));
					}
				}
				
				/*msg =fillZeros(msg.size(),4)+"N"+fillZeros(username.size(),2)+username+msg; //msg final*/

			} else if (action == "E"){//protocol for End
				std::vector<std::string> V;
				for (auto it=clients.begin();it!=clients.end();it++){
					if(it->second==ConnectFD){
						V.push_back(it->first);
					}
				}
				for (int i=0;i<V.size();i++){
					clients.erase(V[i]);
				}
				close(ConnectFD);
				return;
			} else if (action == "F"){
				sendFile+="D";
				std::cout << sendFile << "\n";
				std::string username = "";
				find_str(ConnectFD,username); //username has nickname who send to mssg 

				n = read(ConnectFD, buffer, 2); //reading a size of the other client
				std:: cout << std::string(buffer) << "\n";
				int size_othername=atoi(buffer);
				bzero(buffer, 2);

				n = read(ConnectFD, buffer, size_othername); //reading a nickname the other client
				std::string othername(buffer);
				std:: cout << std::string(buffer) << "\n";
				bzero(buffer, size_othername);
				int size_msg= size_txt;// size has the size the real mssg

				n = read(ConnectFD, buffer, size_msg);
				std::string msg(buffer);
				std::cout << msg << "\n";
				bzero(buffer,size_msg);
				n = read(ConnectFD, buffer, 4);
				std::string str_size_file(buffer);
				std::cout << str_size_file << "\n";
				int size_file=atoi(buffer);
				bzero(buffer,4);
				char msg_file[size_file];
				for (int i=0;i<size_file;i++){
					n=read(ConnectFD,buffer,1);
					msg_file[i]=buffer[0];
					bzero(buffer,1);
				}
				if(find_nick(othername)==false){ //check if othername exists
					//std::cout << "PASE" << std::endl;
					std::string err = "nickname not found, enter other\n";
					write2(ConnectFD, err.c_str(), action);
					continue;
				}	
				sendFile+=fillZeros(username.size(),2)+username+msg+str_size_file;
				std::cout << sendFile << std::endl;
				int otherConnectFD = clients.find(othername)->second; //finding socket number the other client for send to mssg 
				if (otherConnectFD < 0){
					perror("error in nickname");
					continue;
				}
				std::cout<< msg << " -> "+othername<<std::endl;
				if(login){
					write2(otherConnectFD, sendFile, "D");
					write(otherConnectFD,msg_file,size_file);
				} else {
					sendFile="no estas logueado\n";
					write2(otherConnectFD, sendFile, "C");
				}
				
			//here file
			} else {// this is can be better, you can do it =)
				std::cout << "error in action, msg bad\n";
			}

		} while (n == 0);
		// printf("client: %s\n", buffer);
	}
}

bool write2(int ConnectFD, std::string mssg, std::string act){

	if (act == "P" or act == "C" or act == "L") { // L is when a nickname is repeat 
		mssg = fillZeros(mssg.size(),4)+"R" +mssg;
		int nwrite= write(ConnectFD, mssg.c_str(), mssg.size());//std::cout << nwrite << "\n";
		return true;
	} else if (act=="D"){
		int nwrite= write(ConnectFD, mssg.c_str(),mssg.size());//std::cout << nwrite << "\n";
		return true;
	} else if(act=="G"){
		mssg = fillZeros(mssg.size(),4)+"N" +mssg;
		int nwrite= write(ConnectFD, mssg.c_str(), mssg.size());//std::cout << nwrite << "\n";
		return true;	
	}
	return false;
}

void acceptClient(int ConnectFD) {
	if (0 > ConnectFD) {
		perror("error accept failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	write2(ConnectFD,"Welcome","C");
	std::thread(read2, ConnectFD).detach();
	std::this_thread::sleep_for(std::chrono::seconds(100));
}

bool startConnection(int port){
	stSockAddr.sin_port = htons(port);
	if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in))){
		/*perror("error bind failed");
		close(SocketFD);
		exit(EXIT_FAILURE);*/
		return false;
	}
	if(-1 == listen(SocketFD, 10)){
		/*perror("error listen failed");
		close(SocketFD);
		exit(EXIT_FAILURE);*/
		return false;
	}
	std::cout << "Port " << port << ": " << "connection success" << std::endl;
	return true;
}

int main(void){

	if(-1 == SocketFD){
		perror("can not create socket");
		exit(EXIT_FAILURE);
	}

	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_addr.s_addr = INADDR_ANY;
	for (int i=1200;startConnection(i)==false;i++){
		std::cout << "Port " << i << ": " << "bind or listen error" << std::endl;
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
