#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <math.h>
#include <random>
#include <sstream>
#include <pthread.h>

class GameObject;

typedef std::tuple<float,float> point;

extern pthread_t logic_thread;
extern pthread_t network_thread;
extern pthread_t client_update_thread;

extern pthread_mutex_t serverLock;
extern pthread_mutex_t workerLock;

extern pthread_cond_t freeSocket;
extern pthread_cond_t claimedSocket;
extern pthread_cond_t canUpdateClients;

//=========
//--VECTORS--
//=========

void lockThreads();
	
void unlockThreads();

int vecavg(const std::vector<int>&);

template <class T>
inline std::vector<T> operator+(std::vector<T> v,T t) {
	v.push_back(t);
	return v;
}

template <class T>
inline std::vector<T> operator+(std::vector<T> v1,std::vector<T> v2) {
	for(int i=0;i<v2.size();i++) {
		v1.push_back(v2.at(i));
	}
	return v1;
}

template <class T>
T choice_uniform(const std::vector<T> &v) {
	return v.at(rand()%v.size());
}

int nullchars(const std::string&);
std::string clean(std::string&);
int tsize(const std::string&);

template <class T>
std::vector<T> appendToLast(std::vector<T> v, T t) {
	v.at(v.size()-1) = v.at(v.size()-1)+t;
	return v;
}

//dumpvec
template <class T>
void dumpVec(const std::vector<T>& v) {

	std::cout << "--DUMPVEC BEGIN--" << std::endl;
	for(int i=0;i<v.size();i++) {
		std::cout << ">>" << v.at(i) << "<<" << std::endl;
	}
	std::cout << "--DUMPVEC END--" << std::endl;

}

void pad(std::string&,char,int);

int toInt(std::string);

//isdigit
bool isdigit(const std::string& s);
bool issymbol(char);

//strip
std::string strip(const std::string &s);
std::vector<std::string> strip(const std::vector<std::string> &v);

//common
//returns first n characters that strings have in common
std::string common(const std::string &s1, const std::string &s2);


//split
std::vector<std::string> split(char, const std::string&);

//join
std::string join(char, const std::vector<std::string>&);
std::string join(const std::string &c, const std::vector<std::string>& v);


//rsplit
//split by token but retain token
std::vector<std::string> rsplit(char, const std::string&);

//extend
//extends lines from second vector to end of first
template <class T>
std::vector<T> extend(const std::vector<T>& v1, const std::vector<T>& v2) {

	std::vector<T> v3 = v1;

	for(int i=0;i<v2.size();i++) {
		v3.push_back(v2.at(i));
	}
	return v3;
}


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

bool contains(const std::string &s,const std::string &token);

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

//sdlerror
void getSDLError();

//autocomplete
bool autocomplete(std::string&, const std::vector<std::string>&);
bool autocomplete(std::string&, const std::vector<GameObject*>&);

//tolower
std::string toLower(const std::string& s);

//startswith
bool startsWith(const std::string&, const std::string&);

template <class T>
int find(T o,const std::vector<T> v) {
	for(int i=0;i<v.size();i++) {
		if(o == v.at(i)) {
			return i;
		}
	}
	return -1;
}

int find(char c,const std::string &s);

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
void inline debug(T s) {
	lockThreads();
	std::cout << s << std::endl;
	unlockThreads();
}

template <class T>
void inline debug(const std::vector<T>& s) {dumpVec(s);}

float dist(point, point);

///////////////
//CONDITION SET
///////////////

struct ConditionSet {
	std::vector<bool> conds;

	inline bool valid() {
		return !contains(conds,false);
	}

	ConditionSet(){}

	ConditionSet(const std::vector<bool> &c): conds(c) {
		
	}
};
