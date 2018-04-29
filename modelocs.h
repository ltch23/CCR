#include "conneccion.h"

struct ClientServerModel{
	std::thread read;
	std::thread write;
	MainConnection* clientConnection;
	ClientServerModel(){}
	ClientServerModel(MainConnection* MC){
		clientConnection=MC;
	}
	bool run(void (*readF)(int),void (*writeF)(int)){
		read=std::thread(readF,clientConnection->socketFD);
		write=std::thread(writeF,clientConnection->socketFD);
		read.join();
		read.std::thread::~thread();
		write.detach();
		write.std::thread::~thread();
		clientConnection->endConnection();
		return true;
	}
};
