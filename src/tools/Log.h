#pragma once
#include <iostream>
#include <fstream>

struct Log {

	std::string path;
	std::string filename;
	char timestamp[32];
	void write(const std::string&);
	void writeln(const std::string&);
	void get_timestamp();

	Log(const std::string&);
	~Log() {}

};