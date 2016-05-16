#ifndef LiteralTable_H_
#define LiteralTable_H_

#include <iostream>
#include <utility>
#include <string>
#include <string.h>
#include <Parser.h>
#include <vector>

using namespace std;

class LiteralTable
{
public:
    string get(string key);
    string exists(string str);
    void add(string key, string address);
    vector<pair<string,string>> litVector;
};

#endif /* SYMTABLE_H_ */

