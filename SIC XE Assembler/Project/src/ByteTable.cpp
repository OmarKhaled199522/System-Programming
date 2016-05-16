/*
 * OpTable.cpp
 *
 *  Created on: Apr 24, 2016
 *      Author: dod
 */
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include<string.h>
#include <map>
#include "ByteTable.h"
using namespace std;

bool ByteTable::exists(string str){

	map<string, string>::iterator it;
	transform(str.begin(), str.end(), str.begin(), ::toupper);
	it = byteMap.find(str);
	if(it != byteMap.end()){
		return true;
	}
	return false;
}
void ByteTable::setMap(){
    string word, opWord;

    ifstream bytesFile ("bytes.txt");

    int i = 0;
    while(bytesFile >> word)
    {

        if(i++ % 2 == 0)
            opWord = word;
        else
            byteMap[opWord] = word;
    }

    bytesFile.close();
}
ByteTable::ByteTable(){
	setMap();
}

map<string, string> ByteTable::getMap(){
    return byteMap;
}

string ByteTable::get(string key){
    transform(key.begin(), key.end(), key.begin(), ::toupper);
	if(exists(key)){
		return byteMap.find(key)->second;

	}else{
		return '\0';
	}
}
