#include "StringFuncs.h"
#include <iostream>

#include <string>
#include <algorithm>

int rfind(char c, const std::string& s) {

	for(int i=s.length()-1;i>0;i--) {
		if(s[i] == c) {return i;}
	}
	return -1;

}

//s is passed by reference 
bool autocomplete(std::string& s, const std::vector<std::string>& wordList) {

	if(s == "") {
		return false;
	}

	std::string testString = toLower(s);

	for(int i=0;i<wordList.size();i++) {
		if(startsWith(wordList.at(i),testString)) {
			s = wordList.at(i);
			return true;
		}
	}
	return false;
}

//toLower
std::string toLower(const std::string& s) {

	std::string lower = s;
	std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
	return lower;

}

//Replace
std::string replace(const std::string &s, char token, const std::string &sub) {

	std::string newString;

	for(int i=0;i<s.size();i++) {
		if(s[i] == token) {
			newString += sub;
		} else {
			newString += s[i];
		}
	}
	return newString;
}

//startWith
bool startsWith(const std::string& s, const std::string& token) {

	for(int i=0;i<token.size();i++) {
		if(token[i] != s[i]) {
			return false;
		}
	}
	return true;
}

//split
std::vector<std::string> split(char c, const std::string& s) {

	std::vector<std::string> v;
	std::string thisEntry = "";
	for(int i=0;i<s.size();i++) {
		if(s[i] == c) {
			v.push_back(thisEntry);
			thisEntry = "";
		} else {
			thisEntry += s[i];
		}
	}
	if(thisEntry != "") {
		v.push_back(thisEntry);
	}
	return v;
}

//rsplit (Retain split)
std::vector<std::string> rsplit(char c, const std::string& s) {

	std::vector<std::string> v;
	std::string thisEntry = "";
	for(int i=0;i<s.size();i++) {
		if(s[i] == c) {
			v.push_back(thisEntry+s[i]);
			thisEntry = "";
		} else {
			thisEntry += s[i];
		}
	}
	if(thisEntry != "") {
		v.push_back(thisEntry);
	}
	return v;
}

//extend
std::vector<std::string> extend(const std::vector<std::string>& v1, const std::vector<std::string>& v2) {

	std::vector<std::string> v3 = v1;

	for(int i=0;i<v2.size();i++) {
		v3.push_back(v2.at(i));
	}
	return v3;
}

//join
std::string join(char c, const std::vector<std::string>& v) {

	std::string newString = v.at(0);
	if(v.size() == 0) {
		return "";
	}

	for(int i=1;i<v.size();i++) {
		newString += c + v.at(i);
	}

	return newString;

}

float dist(point p1, point p2) {
	float x1 = std::get<0>(p1);
	float y1 = std::get<1>(p1);
	float x2 = std::get<0>(p2);
	float y2 = std::get<1>(p2);

	return sqrt(pow((y1-y2),2)+pow(x1-x2,2));
}
