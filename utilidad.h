std::string fillZeros(int aux_size,int nroBytes){ // complete number with zeross =)
	std::string aux = std::to_string(aux_size);
	int dif = nroBytes - int(aux.size());
	for (int i = 0; i < dif; i++)
		aux = "0" + aux;
	return aux;
}

const char B64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		 "abcdefghijklmnopqrstuvwxyz"
		 "0123456789+/";

bool readFileContentToB64(std::string &filename, std::string &file){
	FILE *pFile;
	if(!(pFile=fopen(filename.c_str(),"rb"))){
		std::cout << "the file does not exist\n" << std::endl;
		return false;
	}
	long lSize;
	char * buffer;
	size_t result;
	fseek(pFile,0,SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	buffer = (char*) malloc (sizeof(char)*lSize);
	if (buffer == NULL) 
		return false;
	result = fread (buffer,1,lSize,pFile);
	if (result != lSize)
		return false;
	file="";
	for (int i=0;i<lSize/3;i++){
		file.push_back(B64[(buffer[i*3]&0xfc)>>2]);
		file.push_back(B64[((buffer[i*3]&0x03)<<4)|((buffer[i*3+1] & 0xf0) >> 4)]);
		file.push_back(B64[((buffer[i*3+1]&0x0f)<<2)|((buffer[i*3+2] & 0xc0) >> 6)]);
		file.push_back(B64[((buffer[i*3+2]&0x3f))]);
	}
	if(lSize%3==2){
		file.push_back(B64[(buffer[0]&0xfc)>>2]);
		file.push_back(B64[((buffer[0]&0x03)<<4)|((buffer[1] & 0xf0) >> 4)]);
		file.push_back(B64[((buffer[1]&0x0f)<<2)]);
		file.push_back('=');
	} else if(lSize%3==1){
		file.push_back(B64[(buffer[0]&0xfc)>>2]);
		file.push_back(B64[((buffer[0]&0x03)<<4)]);
		file.push_back('=');
		file.push_back('=');
	}

	fclose (pFile);
	free (buffer);
	return true;
}

int indB64(char c){
	if(c>='A' && c<='Z'){
		return c-'A';
	}
	if(c>='a' && c<='z'){
		return c-'a'+1+'Z'-'A';
	}
	if(c>='0' && c<='9'){
		return c-'0'+'z'-'a'+2+'Z'-'A';
	}
	if(c=='+'){
		return 62;
	}
	if(c=='/'){
		return 63;
	}
	return 255;
}

bool isInB64(char c){
	if(indB64(c)<64){
		return true;
	}
	return false;
}

std::string readB64Content(std::string &b64){
	std::string msg="";
	int i;
	for (i=0;i<b64.size() && b64[i]!='=' && isInB64(b64[i]) ;i++){
		if((i+1)%4==0){
			msg.push_back((indB64(b64[i-3])<<2)+((indB64(b64[i-2])&0x30)>>4));
			msg.push_back(((indB64(b64[i-2])&0xf)<<4)+((indB64(b64[i-1])&0x3c)>>2));
			msg.push_back(((indB64(b64[i-1])&0x3)<<6)+(indB64(b64[i])));
		}
	}
	if((i%4)){
		int j=i%4;
		int A[4];
		for (int k=0;k<j;k++){
			A[k]=indB64(b64[i-j+k]);//255;
		}
		msg.push_back((A[0]<<2)+((A[1]&0x30)>>4));
		msg.push_back(((A[1]&0xf)<<4)+((A[2]&0x3c)>>2));
	}
	return msg;
}

void writeArchiveB64(std::string &filename, std::string msg_file){
	FILE *newFile=fopen(filename.c_str(),"w");
	std::string b64=msg_file;
	std::string content=readB64Content(b64);
	for (int i=0;i<content.size();i++){
		fprintf(newFile, "%c", content[i]);
	}
	fclose(newFile);
}

