typedef std::string (*pfunc)();

	std::string Cl_printUsers(){
		    return std::string("0000")+    // size of msg
			"P";            // P
	}

	std::string Cl_login(){
		    std::string nickname = "";
		    std::cout << "enter nickname: ";
		    std::cin.ignore(); 
		    getline(std::cin, nickname); // scann with spaces
		    return fillZeros(nickname.size(),4)+   // size of Nickname(4)
			"L"+                // L
			nickname;           // nickname
		    // this_user=nickname;
	}

	std::string Cl_chat(){//MENSAJES SIN SALTOS DE LINEA
		std::string nickname="";
		std::cout<<"enter nickname to chat: ";
		std::cin.ignore();
		getline(std::cin, nickname); //scan with spaces
		std::cout<<"enter message: ";
		std::string msg;
		//std::cin.ignore();
		getline(std::cin,msg); //scan with spaces
		//Protocolo:
		return	fillZeros(msg.size(),4)+	// size of msg(4)
			"C"+ 				// C
			fillZeros(nickname.size(),2)+	// nickname size(2)
			nickname+			// nickname
			msg;				// msg
	}

	std::string Cl_file(){

		std::string file="",filename="",nickname="";
		std::cout << "enter nickname to send file: ";
		std::cin.ignore(); 
		getline(std::cin, nickname); // scann with spaces
		std::cout << "enter filename: ";
		getline(std::cin, filename); // scann with spaces
		if(!readFileContent(filename,file)){
			std::cout << "the file doesn't exist\n";
			return "";
		}
		if(file.size()>9999){
			std::cout << "the file is very large(>9999)\n";
			return "";
		}
		//std::cout << file << std::endl;
		std::cout << readB64Content(file) << std::endl;
		return	fillZeros(filename.size(),4) +	// size of filename(4)
			"F" +				// F
			fillZeros(nickname.size(),2) +	// nickname size(2)
			nickname +			// nickaname
			filename +			// filename
			fillZeros(file.size(),4) +	// file's size(4)
			file;				// file
	}

	std::string Cl_endConnection(){
		    return  std::string("0000") +       // size of msg(4)
			"E";                // E
	}

void Cl_Download(int SocketFD,int size_msg){
	int n;
	char buffer[5];
	n=read(SocketFD, buffer,2);
	int size_othername=atoi(buffer);
	bzero(buffer,2);
	char othernameBuffer[size_othername+1];
	othernameBuffer[size_othername]=0;
	n=read(SocketFD,othernameBuffer,size_othername);
	std::string othername(othernameBuffer);	//othername
	char filenameBuffer[size_msg+1];
	filenameBuffer[size_msg]=0;
	n=read(SocketFD,filenameBuffer,size_msg);
	std::string msg(filenameBuffer);	//filename
	n=read(SocketFD, buffer, 4);
	int size_file=atoi(buffer);
	bzero(buffer,4);
	char msg_file[size_file];
	n=read(SocketFD,msg_file,size_file);
	writeArchiveB64(msg,msg_file);
	std::cout << othername << " send you: " << msg << std::endl; 
}

void Cl_ReadMsg(int SocketFD,int size_msg){
	int n;
	char msg[size_msg+1];
	n = read(SocketFD, msg, size_msg);
	msg[size_msg]=0;
	printf ("[%s]\n", msg);
}

struct ClientProtocol{
	std::map<char, pfunc> F;
	void printMenu(){
		std::cout << "\n------Menu (action)-----\n"
		     << "P -> Print list of user on the chat \n"
		     << "L -> Login to the char\n"
		     << "C -> Send a msg to a user on the chat\n"
		     << "E -> End chat or logout from chat\n"
		     << "F -> Send a file from a user to another user\n"
		     << "G -> Joint to the game\n"
		     << "----------------------------\n"
		     << std::endl;
	}


	ClientProtocol(){
		F['P']=Cl_printUsers;
		F['L']=Cl_login;
		F['C']=Cl_chat;
		F['E']=Cl_endConnection;
		F['F']=Cl_file;
	}
	bool getMsg(char operation, std::string& msg){
		auto it=F.find(operation);
		if(it==F.end()){
			return false;
		}
		msg= (*(it->second))();
		return true;
	}
};
