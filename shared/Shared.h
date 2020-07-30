#pragma once
#include <string>

#define PACKET_SIZE 1024


enum NetworkDataType: int {
	SERV_MSG,
	
	DATA_WORLD,
	DATA_CHAR,
	DATA_OBJS,
	
	EVENT_PROJ_CREATE,
	EVENT_PROJ_COLLIDE,
	
	EVENT_PATH_TURN,
	
};

const std::string DATA_BEGIN= "";//"##DATA_BEGIN##";
const std::string DATA_TERM = "";//"###DATA_END###";
const int STREAM_INFO_LEN = DATA_BEGIN.size();

const int PAD_BOOL = 1;
const int PAD_SHORT = 2;
const int PAD_INT = 6;
const int PAD_STR = 64;
const int PAD_FLOAT= 8;
const int PAD_LONG= 10;

void unpack(int&,std::string&,const std::string&,int);