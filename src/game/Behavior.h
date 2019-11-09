#pragma once

#include <map>

typedef unsigned int flag;

enum statusIndicator: flag;
enum attackStatus: int;

extern std::map<statusIndicator,const std::string> statusMap;