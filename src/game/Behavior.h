#pragma once

#include <map>

typedef unsigned long int flag;

enum StatusIndicator: flag;
enum attackStatus: int;

extern std::map<StatusIndicator,const std::string> statusMap;