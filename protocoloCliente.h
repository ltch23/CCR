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
	    std::cout<< std::endl << "enter message: ";
	    std::cin.ignore();
	    std::string msg;
	    getline(std::cin,msg); //scan with spaces
            return    fillZeros(msg.size(),4)+    // size
		"C"+                // C
		msg;                // msg
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

typedef std::string (*pfunc)();

struct ClientProtocol{
	std::map<char, pfunc> F;
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
