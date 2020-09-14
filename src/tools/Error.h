#pragma once

#include <iostream>
#include <vector>

enum TBA_Error {
	ERR_DIV_BY_ZERO,
	WARN_RET_NULLPTR,
	ERR_INDEX,
	ERR_DEFAULT,
	WARN_DEFAULT,
};

void TBA_throw(TBA_Error,const std::string&,const std::string&);
void TBA_throw(TBA_Error,const std::string&);
