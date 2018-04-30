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

 
using namespace std;

/**Variables and FUnctions for Soscket cliente conection**********************/

void read2(int SocketFD, char *buffer);
std::string fillZeros(int aux_size,int nroBytes);
void write2(int  SocketFD);

std::vector<std::thread> T; //
struct sockaddr_in stSockAddr; //
int Res; //
int SocketFD ; //
char buffer[20]; 
std::string nickname="";

/**Variables and FUnctions for Game**********************/

typedef struct _win_border_struct {
        chtype  ls, rs, ts, bs,
                tl, tr, bl, br;
}WIN_BORDER;

typedef struct _WIN_struct {
        int startx, starty;
        int height, width;
        WIN_BORDER border;
}WIN;

map<string, WIN> players;

void init_win_params(WIN & p_win);
void print_win_params(WIN & p_win);
void create_box(WIN & win, bool flag);
// int move_user1();
void move_user2(WIN & win2,int ch);
void bullet(WIN & p_win);
// void bullet(WIN  & p_win,WIN  & p_win2);

void delete_box(WIN & p_win);


/**Program Socket********************************************************************/

std::string fillZeros(int aux_size,int nroBytes){ // complete number with zeross =)
    std::string aux = std::to_string(aux_size);
    int dif = nroBytes - int(aux.size());
    for (int i = 0; i < dif; i++)
        aux = "0" + aux;
    return aux;
}
// void findWIN(std::string st, WIN & p_win){//return nickanme found their number socket  

//     std::map<std::string, WIN>::iterator it;
//     for (it = players.begin(); it != players.end(); it++)
//         if(it->first == st){
//             p_win=(it->second);
//             std::cout<<"founded"<<std::endl;
//         }
// }

bool existUser(std::string msg){
    std::map<std::string, WIN>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
        if(msg==it->first)
            return true;
    return false;

}

void read2(int SocketFD, char *buffer) {
    int n;
    for (;;){
        bzero(buffer, 5);
        do {
            n = read(SocketFD, buffer, 4); // Reading first 4 bytes
            if(n==0 /*&& T[1].joinable()==false*/){//FINALIZANDO CONEXION
                return;
            }
            int size_msg=atoi(buffer);
            bzero(buffer, 4); // Zeros for the 4 bytes that was reading   

            n = read(SocketFD, buffer, 1); //reading 1 bytes
            std::string action(buffer);
            bzero(buffer, 1); //equal to the before

            if (action == "R"){ // Responsive when is Printing or Chating or error in Login
                char msg[size_msg+1];
                n = read(SocketFD, msg, size_msg);
                msg[size_msg]=0;
                printf ("[%s]\n", msg);
            } else if (action == "D"){//Responsive when is file
                n=read(SocketFD, buffer,2);
                int size_othername=atoi(buffer);
                bzero(buffer,2);
                char othernameBuffer[size_othername+1];
                othernameBuffer[size_othername]=0;
                n=read(SocketFD,othernameBuffer,size_othername);
                std::string othername(othernameBuffer); //othername
                char filenameBuffer[size_msg+1];
                filenameBuffer[size_msg]=0;
                n=read(SocketFD,filenameBuffer,size_msg);
                std::string msg(filenameBuffer);    //filename
                std::cout << othername << " te envió: " << msg << std::endl; 
                n=read(SocketFD, buffer, 4);
                int size_file=atoi(buffer);
                bzero(buffer,4);
                char msg_file[size_file];
                n=read(SocketFD,msg_file,size_file);
                FILE *newFile=fopen(msg.c_str(),"w");
                for (int i=0;i<size_file;i++){
                    fprintf(newFile, "%c", msg_file[i]);
                }
                fclose(newFile);
            }else if(action=="N"){

                char msg[size_msg+1];
                n = read(SocketFD, msg, size_msg);
                msg[size_msg]=0;
                

                if(existUser(string(msg))==false){
                    players[string(msg)]=  WIN();
                }

                // std::cout<<"read"<<std::endl;
                std::map<std::string, WIN>::iterator it;
                // std::cout<<std::endl;
                for (it = players.begin(); it != players.end(); it++){
                       init_win_params(it->second);
                        print_win_params(it->second);
                    }


                // std::cout<<std::endl;


            } else if (action == "H"){ 

                n = read(SocketFD, buffer, 2); //reading 1 bytes
                int size_user=atoi(buffer);
                bzero(buffer, 2); //equal to the before

                char user[size_user+1];
                n = read(SocketFD, user, size_user);
                user[size_user]=0;
                // std::cout<<"user: "<<user<<std::endl;
                // findWIN(string(user), win2);
                char msg[size_msg+1];
                n = read(SocketFD, msg, size_msg);
                msg[size_msg]=0;
                int a = atoi(msg);
                // std::cout<<"msg: "<<msg<<std::endl;
                // printf ("[%s]\n"1, msg);
                // move_user2(win2,a);

                std::map<std::string, WIN>::iterator it;
                for (it = players.begin(); it != players.end(); it++){
                    // std::cout<<it->first<<"-"<<&it->second<<std::endl;  

                    if(it->first == string(user)){
                        
                        // std::cout<<"founded"<<std::endl;
                        move_user2(it->second,a);
                    }
                }
                            // std::cout<<"w: "<<tmp<<std::endl;

            }

            // n = read(SocketFD, buffer, atoi(buffer));
            
        } while (n == 0);
    }
}

void write2(int  SocketFD) {
    std::string msg , aux = "", op = "";
    int dif = 0;

    while (1) {
        msg="";
        std::cout << "------Menu (action)-----\n"
             << "P -> Print list of user on the chat \n"
             << "L -> Login to the char\n"
             << "C -> Send a msg to a user on the chat\n"
             << "E -> End chat or logout from chat\n"
             << "F -> Send a file from a user to another user\n"
             << "G -> G\n"
             << "----------------------------\n"
             << std::endl;
        std::cin >> op;

        if (op == "P") {// protocolo for Print

            //Protocolo:    
            msg =   std::string("0000")+    // size of msg
                "P";            // P
        int nwrite = write(SocketFD, msg.c_str(), msg.size());


        } else if (op == "L"){//protocolo for Login

            nickname = "";
            std::cout << "enter nickname: ";
            std::cin.ignore(); 
            getline(std::cin, nickname); // scann with spaces
            //Protocolo:
            msg=    fillZeros(nickname.size(),4)+   // size of Nickname(4)
                "L"+                // L
                nickname;           // nickname
            // this_user=nickname;
            
             int nwrite = write(SocketFD, msg.c_str(), msg.size());



        } else if (op == "C") { //protocolo for Chat

            std::string nickname="";
            std::cout<<"enter nickname to chat: ";
            std::cin.ignore();
            getline(std::cin, nickname); //scan with spaces
            std::cout<<"enter message: ";
            //std::cin.ignore();
            getline(std::cin,msg); //scan with spaces
            //Protocolo:
            msg=    fillZeros(msg.size(),4)+    // size of msg(4)
                "C"+                // C
                fillZeros(nickname.size(),2)+   // nickname size(2)
                nickname+           // nickname
                msg; // msg
            int nwrite = write(SocketFD, msg.c_str(), msg.size());


        } else if (op == "G")   { //protocolo for Chat


            initscr();                      /* Start curses mode            */
            start_color();                  /* Start the color functionality */
            cbreak();
            keypad(stdscr, TRUE);           /* I need that nifty F1         */
            noecho();
            init_pair(1, COLOR_CYAN, COLOR_BLACK); /* Line buffering disabled, Pass on*/
            // create_box(players[nickname], TRUE);

            attron(COLOR_PAIR(1));
            refresh();  
            attroff(COLOR_PAIR(1));

            // std::cout<<"write"<<std::endl; 
            std::map<std::string, WIN>::iterator it;
            // std::cout<<std::endl;
            for (it = players.begin(); it != players.end(); it++){
                
                   init_win_params(it->second);
                    print_win_params(it->second);

                

            }

          

                std:: string movement;
            while (1){
             // for(int i=100;i<105;i++){   
                int ch=getch();
                // int ch=i;
                // std::cout<<"entre"<<std::endl;
                // int ch = getch();
                // if(ch==27) break;
                movement= std::to_string(ch);
                movement= fillZeros(movement.size(),4)+"G"+movement;
                int nwrite = write(SocketFD, movement.c_str(), movement.size());
            
            }
            // endwin();

        }else if (op == "E"){ // protocolo for End

            //Protocolo:
            msg =   std::string("0000") +       // size of msg(4)
                "E";                // E

        }
        else{ // this can be better =/
            msg = "0000P";
            std::cout << "error action no found, enter other\n ";
            int nwrite = write(SocketFD, msg.c_str(), msg.size());

            continue;
        }
        // int nwrite = write(SocketFD, msg.c_str(), msg.size());
        if(op=="E"){
            return;
        }

}
}
/**Program Game********************************************************************/
void bullet(WIN  & p_win,WIN  & p_win2)
// void bullet(WIN  & p_win)
{
    int k,m,count=0;
    int x, y, w, h;
    int x2_inicial, y2_inicial, x2_final;
    
    x = p_win.startx;
    y = p_win.starty;
    w = p_win.width;
    h = p_win.height;
    
    x2_inicial = p_win2.startx;
    y2_inicial = p_win2.starty;

    x2_final = x2_inicial + w;
    m=x+w/2;

    for (k=y-1;k>0;k--){
        move (k,m ); addstr("o");
            count++;
        if(k == y2_inicial){
            for(int i=x2_inicial; i<x2_final; i++)
                if(m==i){
                    delete_box(p_win2);
                    break;
                }
        }
    }
    sleep(2);

    for (k=y-1;k>y-1-count;k--){
    // std::this_thread::sleep_for(std::chrono::milliseconds(50));
        move( k,m );addstr(" ");
    }    

}
// int move_user1(){

//     int ch = getch();
//     return ch;
// }

void move_user2(WIN & win2,int ch){

    // cout<<ch<<endl;
    // create_box(win2, TRUE);
    // while((ch = getch()) != 122)
    // {
        
        switch(ch)
        {   case KEY_LEFT:
                    create_box(win2, FALSE);
                    --win2.startx;
                    create_box(win2, TRUE);
                    break;
            case KEY_RIGHT:
                    create_box(win2, FALSE);
                    ++win2.startx;
                    create_box(win2, TRUE);
                    break;
            case KEY_UP:
                    create_box(win2, FALSE);
                    --win2.starty;
                    create_box(win2, TRUE);
                    break;
            case KEY_DOWN:
                    create_box(win2, FALSE);
                    ++win2.starty;
                    create_box(win2, TRUE);
                    break;
            case 111:{
                    std::map<std::string, WIN>::iterator it;
                    for (it = players.begin(); it != players.end(); it++)
                        if( &(it->second) != &win2)
                            bullet(win2,it->second);
                        
                    // bullet(win2);
                    break;}
            case 27:
                    endwin();
                    break;
        }       

    // std::cout<<" "<<ch<<std::endl;
    // }
}


void init_win_params(WIN & p_win){

        p_win.height = 8;
        p_win.width = 16;
        p_win.starty = (LINES - p_win.height)/2;
        p_win.startx = (COLS - p_win.width)/2;
        p_win.border.ls = '|';
        p_win.border.rs = '|';
        p_win.border.ts = '-';
        p_win.border.bs = '-';
        p_win.border.tl = '+';
        p_win.border.tr = '+';
        p_win.border.bl = '+';
        p_win.border.br = '+';
}
void print_win_params(WIN & p_win){
}

void create_box(WIN & p_win, bool flag){
        
        int i, j;
        int x, y, w, h;
        x = p_win.startx;
        y = p_win.starty;
        w = p_win.width;
        h = p_win.height;
        if(flag == TRUE)
        {
                move( y+0,x ); addstr("  ##        ##  ");
                move( y+1,x ); addstr("    #      #    ");
                move( y+2,x ); addstr("  ############  ");
                move( y+3,x ); addstr(" ###  ####  ### ");
                move( y+4,x ); addstr("################");
                move( y+5,x ); addstr("# ############ #");
                move( y+6,x ); addstr("# #          # #");
                move( y+7,x ); addstr("   ##      ##   ");
        }
        else
               for(j = y; j <= y + h; ++j)
                        for(i = x; i <= x + w; ++i)
                                mvaddch(j, i, ' ');
        refresh();
}

void delete_box(WIN  & p_win){
        int i, j;
        int x, y, w, h;
        x = p_win.startx;
        y = p_win.starty;
        w = p_win.width;
        h = p_win.height;
        move( y+0,x ); addstr("                ");
        move( y+1,x ); addstr("                ");
        move( y+2,x ); addstr("                ");
        move( y+3,x ); addstr("                ");
        move( y+4,x ); addstr("                ");
        move( y+5,x ); addstr("                ");
        move( y+6,x ); addstr("                ");
        move( y+7,x ); addstr("                ");

        refresh();
}


/*MAIN*******************************************/
int main(){
    int n;
    
    SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (-1 == SocketFD)
    {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1201);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);

    if (0 > Res) {
        perror("error: first parameter is not a valid address family");
        close(SocketFD);
        exit(EXIT_FAILURE);
    } else if (0 == Res) {
        perror("char string (second parameter does not contain valid ipaddress");
        close(SocketFD);
        exit(EXIT_FAILURE);
    } if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in))) {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
       

    T.resize(2);
    T[0]=(std::thread(read2, SocketFD, buffer));
    T[1]=(std::thread(write2, SocketFD));
    T[0].join();
    T[1].join();
    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    if(T[1].joinable()){
        perror("server is close");
        exit(EXIT_FAILURE);
    }
    return 0;
}





