#define KEY_BULU 111
#define KEY_ESC 27
#define INF 100000

typedef struct _win_border_struct{
	chtype ls, rs, ts, bs,
		tl, tr, bl, br;
}WIN_BORDER;

typedef struct _WIN_struct{
	int lives;
	int startx, starty;
	int height, width;
	WIN_BORDER border;
	_WIN_struct(){

		height=8;
		width=16;
		border.ls = '|';
		border.rs = '|';
		border.ts = '-';
		border.bs = '-';
		border.tl = '+';
		border.tr = '+';
		border.bl = '+';
		border.br = '+';

	}
	_WIN_struct(int x,int y){
		startx=x;
		starty=y;
		height=8;
		width=16;
		lives=4;
		border.ls = '|';
		border.rs = '|';
		border.ts = '-';
		border.bs = '-';
		border.tl = '+';
		border.tr = '+';
		border.bl = '+';
		border.br = '+';

	}
	_WIN_struct(int x,int y,int l){
		startx=x;
		starty=y;
		lives=l;
		height=8;
		width=16;
		border.ls = '|';
		border.rs = '|';
		border.ts = '-';
		border.bs = '-';
		border.tl = '+';
		border.tr = '+';
		border.bl = '+';
		border.br = '+';

	}
	int coincidence(_WIN_struct& B){
		int c=0;
		if(startx>B.startx+B.width){//0001 -> Derecha
			c+=1;
		} 
		if(startx+width<B.startx){//0010 <- Izquierda
			c+=2;
		}
		if(starty>B.starty+B.height){//0100 v Abajo
			c+=4;
		}
		if(starty+height<B.starty){//1000 ^ Arriba
			c+=8;
		}
		return c;
	}
} WIN;

struct game{

	std::map<std::string, WIN> players;
	std::map<std::string,WIN>::iterator it;
	int height, width;
	int maxy;
	game(){
		height=8;
		width=16;
	}

	bool existConflicts(std::string P){
		for (it=players.begin();it!=players.end();it++){
			if(it->first!=P){
				std::cout << it->first << std::endl;
				if(players[P].coincidence(it->second)==0){
					return true;
				}
			}
		}
		return false;
	}

	bool newPlayer(std::string P, int L, int C){
		int W=(C-width)/2;
		int H=(L-height)/2;
		WIN p_win(W,H);
		players[P]=p_win;
		bool newposition=!existConflicts(P);
		int moveposition=1;
		while(newposition==false){
			players[P].startx=W-2*moveposition;
			for (int i=0;i<3;i++){
				players[P].startx+=moveposition;
				players[P].starty=H-2*moveposition;
				for (int j=0;j<3;j++){
					players[P].starty+=moveposition;
					if(!existConflicts(P)){
						return true;
					}
				}
			}
			moveposition++;
		}
		return true;
	}

	bool existVictim(std::string& P, std::string& B){
		maxy=-INF;
		for (it=players.begin();it!=players.end();it++){
			if(it->first!=P){
				int c=players[P].coincidence(it->second);
				if ((c&1)==0 && (c&2)==0 && (c&4)){
					if(it->second.startx<=players[P].startx+8 && it->second.startx+width>players[P].startx+8){
						if(maxy<(it->second).starty+8){
							B=it->first;
							maxy=(it->second).starty+8;
						}
					}
				}
			}
		}
		return (maxy!=-INF);
	}

	bool move_user2(std::string P, int action, std::string& affect){
		affect=P;
		switch(action){
			case KEY_LEFT:
				players[P].startx--;
				if(existConflicts(P)){
					players[P].startx++;
					return false;
				}
				return true;
			case KEY_RIGHT:
				players[P].startx++;
				if(existConflicts(P)){
					players[P].startx--;
					return false;
				}
				return true;
			case KEY_DOWN:
				players[P].starty++;
				if(existConflicts(P)){
					players[P].starty--;
					return false;
				}
				return true;
			case KEY_UP:
				players[P].starty--;
				if(existConflicts(P)){
					players[P].starty++;
					return false;
					break;
				}
				return true;
			/*case KEY_BULL:
				break;
			case KEY_BULR:
				break;
			case KEY_BULD:
				break;*/
			case KEY_BULU:
				std::cout << "disparando" << std::endl;
				if(existVictim(P,affect)){
					std::cout << affect << std::endl;
					players[affect].lives--;
					return true;
				}
				return false;
			case KEY_ESC:
				players[P].lives=0;
				return true;
		}
		return false;
	}
};
