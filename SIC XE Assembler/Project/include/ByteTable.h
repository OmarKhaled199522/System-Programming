#ifndef OPTABLE_H_
#define OPTABLE_H_

#include <iostream>
#include <map>
#include <string>
using namespace std;


class ByteTable {
public:
	string get(string key);
	void setMap();
	bool exists(string str);
	map<string, string> getMap();
	ByteTable();
private:
	map<string, string> byteMap;
};

#endif /* SYMTABLE_H_ */
