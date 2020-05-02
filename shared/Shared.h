#pragma once
#include <string>

enum NetworkDataType: int {
	DATA_WORLD = 1,
	DATA_CHAR = 2,
};

const std::string DATA_BEGIN= "###DATA_BEGIN###";
const std::string DATA_TERM = "###DATA_END###";
const int DATA_BEGIN_LEN = DATA_BEGIN.size();
const int DATA_TERM_LEN = DATA_TERM.size();