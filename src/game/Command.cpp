#include "Command.h"

#include "../tools/StringFuncs.h"

#include <vector>
#include <string>

Command::Command(const std::string& _name,const std::vector<std::string>& _alias,int _argCount,

		//Primary and EC functions
		std::string (*_function)(Command* , const std::vector<std::string>&),
		bool (*_ECfunction)(Command*, const std::vector<std::string>&)):
		
	name(_name), aliases(_alias), argCount(_argCount), function(_function), ECfunction(_ECfunction) {}

std::string Command::call(const std::vector<std::string>& args) {

	//If command has no errorchecking, run primary function
	if(this->ECfunction == nullptr) {
		return this->function(this,args);
	} else {
		if(this->ECfunction(this,args)) {
			//If errorcheck is passed, run primary function
			return this->function(this,args);
		} else {
			//If Errorcheck is failed, return error value set by errorcheck function
			return "\n"+this->error;
		}
	}
}