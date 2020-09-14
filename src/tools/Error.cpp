#include "Error.h"
#include "Utility.h"

void TBA_throw(TBA_Error type, const std::string &funcname, const std::string &err) {
	std::string perr;
	switch(type) {
		case ERR_INDEX:
			perr = "ERROR ("+funcname+"): Index out of range";
			break;
		case ERR_DIV_BY_ZERO:
			perr = "ERROR ("+funcname+"): Division by zero";
			break;
		case WARN_RET_NULLPTR:
			perr = "WARNING ("+funcname+"): Returning nullpointer";
			break;
		case ERR_DEFAULT:
			perr ="ERROR ("+funcname+"): "+err;
			break;
		case WARN_DEFAULT:
			perr = "WARNING ("+funcname+"): "+err;
			break;
		default:
			perr = "ERR_TYPE_UNDEF ("+funcname+"): "+err;
			break;
	}
	debug(perr);
}
void TBA_throw(TBA_Error type, const std::string &funcname) {
	TBA_throw(type,funcname,"");
}
