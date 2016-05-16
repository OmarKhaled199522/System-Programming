/*
 * Table.h
 *
 *  Created on: Apr 24, 2016
 *      Author: dod
 */

#ifndef SYMTABLE_H_
#define SYMTABLE_H_

#include <iostream>
#include <map>
#include <string>
using namespace std;

class SymTable {
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
