struct MainConnection{
	int socketFD ;
	int Res;
	struct sockaddr_in stSockAddr;
	MainConnection(){}
	MainConnection(const char* ip, int port=1200){
		createSocket();
		initConnection(ip,port);
	}
	void createSocket(){
		socketFD=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (-1 == socketFD) {
			perror("cannot create socket");
			exit(EXIT_FAILURE);
		}
	}
	void initConnection(const char* ip, int port){
		memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
		stSockAddr.sin_family = AF_INET;
		stSockAddr.sin_port = htons(port);
		Res = inet_pton(AF_INET, ip, &stSockAddr.sin_addr);

		if (0 > Res) {
			perror("error: first parameter is not a valid address family");
			close(socketFD);
			exit(EXIT_FAILURE);
		} else if (0 == Res) {
			perror("char string (second parameter does not contain valid ipaddress");
			close(socketFD);
			exit(EXIT_FAILURE);
		} if (-1 == connect(socketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in))) {
			perror("connect failed");
			close(socketFD);
			exit(EXIT_FAILURE);
		}
	}
	void endConnection(){
		shutdown(socketFD, SHUT_RDWR);
		close(socketFD);
	}
};
