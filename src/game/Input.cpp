#include "Input.h"

#include "../tools/StringFuncs.h"
#include "../common/Common.h"
#include "Command.h"

#include <string>
#include <random>
#include <ctime>

std::string processCommand(const std::string& s) {

	if(s.size() == 0) {
		return "";
	}

	std::string commandString;

	//Split command string into word list
	std::vector<std::string> commandVector  = split(' ',s);
	std::vector<std::string> args;

	//Command is the first word in the wordlist
	commandString = commandVector.at(0);

	if(commandVector.size() == 1) {
		args = {};
	} else {
		//Arguments are the rest of the wordlist
		args = subVec(commandVector,1,commandVector.size());
	}

	
	
	Command* testCommand;

	for(int i=0;i<TBAGame->commandList.size();i++) {

		//Check known commands against input command
		testCommand = TBAGame->commandList.at(i);

		if(testCommand->name == commandString) {
			//If input command matches, call command with arguments
			return testCommand->call(args);

		} else {
			//Otherwise, check against known command aliases
			for(int j=0;j<testCommand->aliases.size();j++) {
				if(testCommand->aliases.at(j) == commandString) {
					return testCommand->call(args);
				}
			}
		}
	}

	//If input command doesn't match any known commands or aliases, return error string
	return "\n"+commandString+": command not found";

}