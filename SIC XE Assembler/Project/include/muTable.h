#ifndef MUTABLE_H
#define MUTABLE_H


#include <iostream>
#include <map>
#include <string>
using namespace std;

class muTable {
public:
	string get(string key);
	bool exists(string str);
	void add(string key, string address);
	map<string, string> getMap();
	void makeAbsolute(string key);
	bool isAbs(string key);
private:
	map<string, string> symMap;
	map<string, bool> relMap;
};

#endif /* SYMTABLE_H_ */
