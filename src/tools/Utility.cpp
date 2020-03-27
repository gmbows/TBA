#include "Utility.h"
#include "../common/Common.h"

#include <iostream>

#include <string>
#include <algorithm>

int rfind(char c, const std::string& s) {

	for(int i=s.length()-1;i>0;i--) {
		if(s[i] == c) {return i;}
	}
	return -1;

}

int find(char c,const std::string &s) {
	for(int i=0;i<s.size();i++) {
		if(c == s[i]) {
			return i;
		}
	}
	return -1;
}


std::string strip(const std::string &s) {
    std::string newString = "";
    for(int i=0;i<s.size();i++) {
        if(s[i] != ' ') {
            newString += s[i];
        }
    }
    return newString;
}

std::vector<std::string> strip(const std::vector<std::string> &v) {
    std::vector<std::string> newV = {};
    for(int i=0;i<v.size();i++) {
        if(v.at(i) != "") {
            newV.push_back(v.at(i));
        }
    }
    return newV;
}

std::string common(const std::string &s1, const std::string &s2) {
	std::string c = "";
	int range = std::min(s1.size(),s2.size());
	for(int i=0;i<range;i++) {
		if(s1[i] == s2[i]) {
			c += s1[i];
		} else {
			return c;
		}
	}
	return "";
}

//s is passed by reference 
bool autocomplete(std::string& s, const std::vector<std::string>& wordList) {

	if(s == "") {
		return false;
	}

	std::string testWord;
	std::string searchString = toLower(s);
	std::string matchlist;
	bool hasMultipleMatches = false;


	for(int i=0;i<wordList.size();i++) {
		testWord = toLower(wordList.at(i));
		if(startsWith(testWord,searchString)) {
			s = wordList.at(i);
			matchlist = s;
			for(int j=0;j<wordList.size();j++) {
				testWord = toLower(wordList.at(j));
				if(startsWith(testWord,searchString) and wordList.at(j) != s) {
					if(common(s,wordList.at(j)) != "") {
						s = common(s,wordList.at(j));
					}
					matchlist += " | "+wordList.at(j);	
					hasMultipleMatches = true;
				}
			}
			if(hasMultipleMatches) {
				TBAGame->displayText("\n"+matchlist+"\r");
				return false;
			} else {
				return true;
			}
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
			if(find(' ',s) >= 0) {
				std::vector<std::string> splitWord = split(' ',s);
				for(int j=0;j<splitWord.size();j++) {
					if(startsWith(splitWord.at(j),token)) return true;
				}
			}
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

std::string join(const std::string &c, const std::vector<std::string>& v) {

	std::string newString = v.at(0);
	if(v.size() == 0) {
		return "";
	}

	for(int i=1;i<v.size();i++) {
		newString += c + v.at(i);
	}

	return newString;

}

bool isdigit(const std::string& s) {
    std::vector<char> digits = {'0','1','2','3','4','5','6','7','8','9'};
	if(s.size() == 0) return false;
    for(int i=0;i<s.size();i++) {
        if(find(s[i], digits) == -1) {
            return false;
        }
    }
    return true;
}
bool issymbol(char c) {
    std::vector<char> symbols = {'!','@','#','$','%','^','&','*',')','(','-','=','+','_','`','~','/','\\','.','|','[',']','{','}',';',':','"','?','<','>'};
    return contains(symbols,c);
}

float dist(point p1, point p2) {
	float x1 = std::get<0>(p1);
	float y1 = std::get<1>(p1);
	float x2 = std::get<0>(p2);
	float y2 = std::get<1>(p2);

	return sqrt(pow((y1-y2),2)+pow(x1-x2,2));
}

void getSDLError() {
    const char *sdlError = SDL_GetError();
    if(sdlError && *sdlError)
    {
        ::std::cout << "SDL ERROR: " << sdlError << ::std::endl;
    }
}