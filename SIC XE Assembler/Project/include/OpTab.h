#ifndef OPTAB_H
#define OPTAB_H

#include <iostream>
#include <map>
#include <string>
using namespace std;

class OpTab {
public:
	string get(string key);
	void setMap();
	bool exists(string str);
	map<string, string> getMap();
	OpTab();
private:
	map<string, string> opMap;
};

#endif // OPTAB_H
