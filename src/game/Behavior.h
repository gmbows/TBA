#pragma once

#include <map>

typedef unsigned long int flag;

enum statusIndicator: flag;
enum attackStatus: int;

extern std::map<statusIndicator,const std::string> statusMap;