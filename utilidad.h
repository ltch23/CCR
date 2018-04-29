std::string fillZeros(int aux_size,int nroBytes){ // complete number with zeross =)
	std::string aux = std::to_string(aux_size);
	int dif = nroBytes - int(aux.size());
	for (int i = 0; i < dif; i++)
		aux = "0" + aux;
	return aux;
}
