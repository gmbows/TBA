#pragma once
#include <iostream>
#include <fstream>

struct Log {

	std::string path;
	std::string filename;
	char timestamp[32];
	void write(const std::string&);
	void write_nts(const std::string&); //write without timestamp
	void writeln(const std::string&);
	std::string get_timestamp();
	
	std::ofstream logfile;

	Log(const std::string&);
	~Log() {
		this->logfile.close();
	}

};