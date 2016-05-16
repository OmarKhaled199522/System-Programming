#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <string.h>
#include <Parser.h>
#include "LiteralTable.h"

using namespace std;

int index;
Parser parser;

string LiteralTable::exists(string str)
{
    index = 0;
    for(vector<pair<string,string>>::iterator it = litVector.begin(); it != litVector.end(); it++)
    {
        string key = it->first;

        if(parser.stringcmp(str.c_str(), key.c_str()) == 0)
        {
            return it->second;
        }
        index++;
    }
    return "";
}

void LiteralTable::add(string key, string address)
{
    pair<string,string> literal (key, address);
    exists(key);
    if(index == litVector.size())
    {
        litVector.push_back(literal);
    }
    else
    {
        litVector[index] = literal;
    }
}

string LiteralTable::get(string key)
{
    string result = exists(key);
    if(index != litVector.size())
    {
        return result;
    }
    else
    {
        return "";
    }
}


