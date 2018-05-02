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
#include <errno.h>
#include <map>


#include <ncurses.h>
#include <chrono>         // std::chrono::seconds
#include <unistd.h>

#include "utilidad.h"
#include "protocoloCliente.h"
#include "modelocs.h"
#include "juego.h"

using namespace std;

/**Variables and FUnctions for Soscket cliente conection**********************/

void read2(int SocketFD);
std::string fillZeros(int aux_size,int nroBytes);
void write2(int  SocketFD);

std::vector<std::thread> T; //
/**Variables and FUnctions for Game**********************/

std::map<string,WIN> players;
std::map<std::string, WIN>::iterator it;

void create_boxs();
void create_box(string playerName, bool flag);
void create_bullet(int x, int y, int h);
void print_message(std::string msg);
bool playing=false;
std::string myNickname="";

/**Program Socket********************************************************************/

void Cl_bullet(int SocketFD, int size_msg){
	int n;
	char buffer[5];
	bzero(buffer,5);
	char msg[size_msg+1];
	n = read(SocketFD, msg, size_msg); //reading size_msg bytes
	msg[size_msg]=0;
	string playerName(msg);
	n = read(SocketFD, buffer,4); //reading 4 bytes
	int h=atoi(buffer);
	bzero(buffer,4); //Zeros for the 4 bytes that was reading
	if(playing){
	    //clear();
	    create_bullet(players[playerName].startx+8,players[playerName].starty-h,h);
	}
}

void Cl_game(int SocketFD, int size_msg){
	int n;
	char buffer[5];
	bzero(buffer,5);
		
	char msg[size_msg+1];
	n = read(SocketFD, msg, size_msg); //reading size_msg bytes
	msg[size_msg]=0;
	string playerName(msg);
	n = read(SocketFD, buffer,4); //reading 4 bytes
	string xnum(buffer);
	bzero(buffer, 4); // Zeros for the 4 bytes that was reading   
	while(xnum.size()>1 && xnum[0]=='0'){
		xnum.erase(xnum.begin());
	}
	int x=atoi(xnum.c_str());
	n = read(SocketFD, buffer,4); //reading 4 bytes
	string ynum(buffer);
	bzero(buffer, 4); // Zeros for the 4 bytes that was reading   
	while(ynum.size()>1 && ynum[0]=='0'){
		ynum.erase(ynum.begin());
	}
	int y=atoi(ynum.c_str());
	n = read(SocketFD, buffer,2); //reading 2 bytess 
	int lives=atoi(buffer);
	bzero(buffer,2);//Zeros for the 2 bytes that was raeading

	if(lives==0) {
		if( players.find(playerName)!=players.end()){
			players.erase(players.find(playerName));
		}
	} else {
		WIN newPlayer(x,y,lives);
		players[playerName]=newPlayer;
	}
	if(playing){
		clear();  
		create_boxs();
		refresh();
	}
}

void read2(int SocketFD) {
	int n;
	char buffer[5]; //
	for (;;){
		bzero(buffer, 5);
		do {
			n = read(SocketFD, buffer, 4); // Reading first 4 bytes
			if(n==0){//interrupted connection
				if(playing)
					endwin();
				cout << "Server: The connection was interrupted" << endl;
				return;
			}
			int size_msg=atoi(buffer);
			bzero(buffer, 4); // Zeros for the 4 bytes that was reading   

			n = read(SocketFD, buffer, 1); //reading 1 bytes
			std::string action(buffer);
			bzero(buffer, 1); //equal to the before

			if (action == "G"){ // Responsive when exist movement in Game
				Cl_game(SocketFD,size_msg);
			} else if(action=="O"){
				Cl_bullet(SocketFD,size_msg);
			} else if(action=="R"){
				std::string msg=Cl_ReadMsg(SocketFD, size_msg);
				print_message(msg);
			} else if(action=="D"){
				if(!playing){
					std::cout << "Downloading" << std::endl;
				}
				std::string msg=Cl_Download(SocketFD, size_msg);
				print_message(msg);
			}

		} while (n == 0);
	}
}

void write2(int  SocketFD) {
	std::string msg , aux = "", op = "";
	int dif = 0;
	ClientProtocol CP;
	while (op!="E") {
		CP.printMenu();
		std::cin >> op;
		if(op.size()==1 && CP.getMsg(op[0],msg) && msg!=""){
			int nwrite = write(SocketFD, msg.c_str(), msg.size());
		} else {

			if (op == "G") { //protocolo for Game
				playing=true;
				//Start curses mode        
				initscr(); 
				// Start the color functionality 
				start_color();                  
				cbreak();
				// I need that nifty F1 
				keypad(stdscr, TRUE);           
				noecho();
				// Line buffering disabled, Pass on
				init_pair(1, COLOR_CYAN, COLOR_BLACK);
				attron(COLOR_PAIR(1));
				refresh();  
				attroff(COLOR_PAIR(1));
				std::string movement=std::to_string(int('N'));
				movement=fillZeros(movement.size(),4)+"G"+movement
					+fillZeros(LINES,4)+fillZeros(COLS,4);
				int nwrite = write(SocketFD, movement.c_str(), movement.size());

				while (1) {
					int ch = getch();
					movement=std::to_string(ch) ;
					movement=fillZeros(movement.size(),4)+"G"+movement;
					int nwrite = write(SocketFD, movement.c_str(), movement.size());
					if(ch==KEY_ESC){
						endwin();
						playing=false;
						break;
					}
				}
				print_message("End Game");
			} else {
				std::cout << "Error: Action does not work.\n ";
				continue;
			}
		}
	}

	return ;
}

void create_bullet(int x, int y, int h){
	for (int i=0;i<h;i++){
		move(y+i,x); addstr("o");
	}
	refresh();
}

void print_message(std::string msg){
	if(playing){
		move(0,0);clrtoeol();
		move(0,0);addstr(msg.c_str());
		refresh();
	} else {
		std::cout << msg << endl;
	}
}

void create_boxs(){
	for (it=players.begin();it!=players.end();it++){
		create_box(it->first,false);
		create_box(it->first,true);
	}
}


void create_box(std::string playerName, bool flag){
	int x, y, w, h, v;
	x = players[playerName].startx;
	y = players[playerName].starty;
	w = players[playerName].width;
	h = players[playerName].height;
	std::string playerLive = std::to_string(players[playerName].lives);
	while(playerName.size()<8){
		playerName+="#";
	}
	playerName="#USER: "+playerName.substr(0,8)+"#";
	playerLive="# #    "+playerLive+"     # #";
	string mons[8];
	mons[0]="  ##        ##  ";
	mons[1]="    #      #    ";
	mons[2]="  ############  ";
	mons[3]=" ###  ####  ### ";
	mons[4]=playerName.c_str();
	mons[5]="# ############ #";
	mons[6]=playerLive.c_str();
	mons[7]="   ##      ##   ";
	if(flag == TRUE) {
		for (int i=0;i<h;i++)
			for (int j=0;j<w;j++)
				if(x+j<=COLS && x+j>=0){
					if(y+i>=0 && y+i<=LINES){
						mvaddch(y+i,x+j,mons[i][j]);
					}
				}
	} else {
		for(int j = y; j < y + h; ++j)
			for(int i = x; i < x + w; ++i)
				if(x+j<=COLS && x>=0){
					if(y+i>=0 && y+i<=LINES){
						mvaddch(j, i, ' ');
					}
				}
	}
}

/*MAIN*******************************************/
int main(){
	int port;
	cout << "Port: ";
	cin >> port;
	MainConnection MC("10.0.1.3",port);
	ClientServerModel CSM(&MC);
	CSM.run(read2,write2);
	return 0;
}
