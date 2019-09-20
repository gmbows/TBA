#include "Log.h"
#include <iostream>
#include <fstream>

#include <ctime>

Log::Log(const std::string& _filename) {
	this->filename = _filename;
	this->path = "log/"+_filename;
	std::ofstream logfile;
	logfile.open(this->path,std::ofstream::out | std::ofstream::trunc);
	if(!logfile) {
		std::cerr << "ERROR opening "<< this->filename << " at path " << this->path << std::endl;
		return;
	} else {
		this->writeln(this->filename+" initialized");
		logfile.close();
	}
}

void Log::write(const std::string& s) {

	std::ofstream logfile;
	logfile.open(this->path,std::ios_base::app);

	if(!logfile) {
		std::cerr << "ERROR writing to "<< this->filename << " at path " << this->path << std::endl;
		return;
	} else {
		this->get_timestamp();
		logfile << this->timestamp << " : " << s;
		logfile.close();
	}

}

void Log::writeln(const std::string& s) {

	std::ofstream logfile;
	logfile.open(this->path,std::ios_base::app);

	if(!logfile) {
		std::cerr << "ERROR writing to "<< this->filename << " at path " << this->path << std::endl;
		return;
	} else {
		this->get_timestamp();
		logfile << this->timestamp << " : " << s << std::endl;
		logfile.close();
	}

}

void Log::get_timestamp() {

	//Get UTC and convert to tm struct
	time_t now = time(0);
	tm *local_time = localtime(&now);

	//Get format using local_time struct and copy 32 characters into char array
	strftime(this->timestamp,32,"%b/%d/%Y(%a)%H:%M:%S",local_time);

}