#pragma once

#include <vector>
#include <string>

struct Command {

	//Primary string identifier
	//const std::string name;

	//Alternative ID strings
	const std::vector<std::string> aliases;

	//Minimum arg count
	int argCount;

	//Error check string for information acquired during failed errochecking
	std::string error;

	//Auxiliary information vector for information acquired during passed errorchecking
	std::string aux;

	//Handler
	std::string call(const std::vector<std::string>& args);

	//Primary function
	std::string (*function)(Command*, const std::vector<std::string>&);

	//Errorchecking function
	bool (*ECfunction)(Command*, const std::vector<std::string>&);

	//Command(const std::string&,const std::vector<std::string>&,int,std::string (*_function)(const std::vector<std::string>&));
	Command(
		const std::vector<std::string>&,
		int,
		std::string (*_function)(Command*, const std::vector<std::string>&),
		bool (*_ECfunction)(Command*, const std::vector<std::string>&) = nullptr
	);

};