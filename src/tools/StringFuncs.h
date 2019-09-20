#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <math.h>

typedef std::tuple<float,float> point;

//=========
//--VECTORS--
//=========

//dumpvec
template <class T>
void dumpVec(const std::vector<T>& v) {

	std::cout << "--DUMPVEC BEGIN--" << std::endl;
	for(int i=0;i<v.size();i++) {
		std::cout << ">>" << v.at(i) << "<<" << std::endl;
	}
	std::cout << "--DUMPVEC END--" << std::endl;

}

//split
std::vector<std::string> split(char, const std::string&);

//join
std::string join(char, const std::vector<std::string>&);

//rsplit
//split by token but retain token
std::vector<std::string> rsplit(char, const std::string&);

//extend
//extends lines from second vector to end of first
std::vector<std::string> extend(const std::vector<std::string>&, const std::vector<std::string>&);

//swap
//swaps 2 entries in a vector
template<class T>
void swap(std::vector<T> &v, int i ,int j) {
	T temp;
	temp = v.at(i);
	v.at(i) = v.at(j);
	v.at(j) = temp;
}

//subVec
//Like substring, but for vectors
template <class T>
std::vector<T> subVec(const std::vector<T> &v,int start,int end) {

	if(abs(start) >= v.size()) {
		start = 0;
	}
	if(abs(end) >= v.size()) {
		end = v.size();
	}

	if(start < 0) {
		start = v.size()+start;
	}
	if(end < 0) {
		end = v.size()+end;
	}

	std::vector<T> newVec;

	for(int i=start;i<end;i++) {
		newVec.push_back(v.at(i));
	}
	return newVec;
}

//contains
//Check if vector contains token
template <class T>
bool contains(const std::vector<T> v,T c) {
	for(int i=0;i<v.size();i++) {
		if(v.at(i) == c) {return true;}
	}
	return false;
}

template <class T>
int count(const std::vector<T> &v, T token) {
	int count = 0;
	for(int i=0;i<v.size();i++) {
		if(v.at(i) == token){ 
			count++;
		}
	}
	return count;
}

//========
//--STRINGS--
//========

//autocomplete
bool autocomplete(std::string&, const std::vector<std::string>&);

//tolower
std::string toLower(const std::string& s);

//startswith
bool startsWith(const std::string&, const std::string&);

template <class T>
int find(T o,const std::vector<T> v) {
	for(int i=0;i<v.size();i++) {
		if((char*)o == (char*)v.at(i)) {
			return i;
		}
	}
	return -1;
}

//rfind
int rfind(char, const std::string&);

std::string replace(const std::string&,char,const std::string&);
//========
//--TUPLE--
//========

template <class T>
void decompose(std::tuple<T,T> tup,T &x, T &y) {
	x = std::get<0>(tup);
	y = std::get<1>(tup);
}

//--DEBUG--
template <class T>
void inline debug(T s) { std::cout << s << std::endl; }

template <class T>
void inline debug(const std::vector<T>& s) {dumpVec(s);}

float dist(point 	, point);