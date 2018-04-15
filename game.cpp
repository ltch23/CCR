#include <ncurses.h>
#include <iostream>       // std::cout
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <errno.h>

 
using namespace std;


typedef struct _win_border_struct {
        chtype  ls, rs, ts, bs,
                tl, tr, bl, br;
}WIN_BORDER;

typedef struct _WIN_struct {
        int startx, starty;
        int height, width;
        WIN_BORDER border;
}WIN;


void init_win_params(WIN & p_win);
void print_win_params(WIN & p_win);
void create_box(WIN & win, bool flag);
void move_user1(WIN & win, WIN & win2);
void move_user2(WIN & win, WIN & win2);
void bullet(WIN & p_win);
void delete_box(WIN & p_win);

std::vector<std::thread> T; //
struct sockaddr_in stSockAddr; //
int Res; //
int SocketFD ; //
char buffer[5]; //



void bullet(WIN  & p_win,WIN  & p_win2)
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

    x2_final = x2_inicial+w;
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


    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    for (k=y-1;k>y-1-count;k--){
        move( k,m );addstr(" ");
    }

}
void move_user1(WIN & win, WIN & win2){

    int ch;
    create_box(win, TRUE);
    while((ch = getch()) != 122)
    {
        switch(ch)
        {   case KEY_LEFT:
                    create_box(win, FALSE);
                    --win.startx;
                    create_box(win, TRUE);
                    break;
            case KEY_RIGHT:
                    create_box(win, FALSE);
                    ++win.startx;
                    create_box(win, TRUE);
                    break;
            case KEY_UP:
                    create_box(win, FALSE);
                    --win.starty;
                    create_box(win, TRUE);
                    break;
            case KEY_DOWN:
                    create_box(win, FALSE);
                    ++win.starty;
                    create_box(win, TRUE);
                    break;
            case 111:
                    bullet(win,win2);
                    break;
            }
    }
}

void move_user2(WIN & win, WIN & win2){

    int ch;
    create_box(win, TRUE);
    while((ch = getch()) != 122)
    {
        switch(ch)
        {   case 97:
                    create_box(win, FALSE);
                    --win.startx;
                    create_box(win, TRUE);
                    break;
            case 100:
                    create_box(win, FALSE);
                    ++win.startx;
                    create_box(win, TRUE);
                    break;
            case 119:
                    create_box(win, FALSE);
                    --win.starty;
                    create_box(win, TRUE);
                    break;
            case 115:
                    create_box(win, FALSE);
                    ++win.starty;
                    create_box(win, TRUE);
                    break;
            case 112:
                    bullet(win,win2);
                    break;
        }
    }
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

void create_box(WIN  & p_win, bool flag){
        
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


int main(int argc, char *argv[])
{       

        



        WIN win;
        WIN win2;

        initscr();                      /* Start curses mode            */
        start_color();                  /* Start the color functionality */
        cbreak();                       /* Line buffering disabled, Pass on
                                         * everty thing to me           */
        keypad(stdscr, TRUE);           /* I need that nifty F1         */
        noecho();
        init_pair(1, COLOR_CYAN, COLOR_BLACK);
        


        /* Initialize the window parameters */
        init_win_params(win);
        print_win_params(win);
        /* Initialize the window parameters */
        init_win_params(win2);
        print_win_params(win2);
        

        attron(COLOR_PAIR(1));
        refresh();
        
        attroff(COLOR_PAIR(1));


         //create_player_1
        // --win2.startx;
        // --win2.starty;
        
        // create_box(win2, TRUE); //create_player_2

       
        thread t1= thread(move_user1, std::ref(win),std::ref(win2)); 
        thread t2= thread(move_user2, std::ref(win2),std::ref(win)); 
        // move_user(&win2); 
        t1.join();
        t2.join();

        endwin();                       /* End curses mode                */
        


       return 0;
}

