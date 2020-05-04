#include "Shared.h"

void unpack(int &index,std::string &val,const std::string &data,int len) {
	val = data.substr(index,len);
	index += len;
}