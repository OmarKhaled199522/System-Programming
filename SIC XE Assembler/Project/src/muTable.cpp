/*
 * SymTable.cpp
 *
 *  Created on: Apr 24, 2016
 *      Author: dod
 */


#include <iostream>
#include <map>
#include <string>
#include "muTable.h"
using namespace std;

bool muTable::exists(string str){

	map<string, string>::iterator it;
	it = symMap.find(str);
	if(it != symMap.end()){
		return true;
	}
	return false;
}

void muTable::add(string key, string address){
	symMap[key] = address;
	relMap[key] = false;
}

string muTable::get(string key){
	if(exists(key)){
		return symMap.find(key)->second;

	}else{
		return "";
	}
}

void muTable::makeAbsolute(string key){
    relMap[key] = true;
}

bool muTable::isAbs(string key){
    if(exists(key)){
		return relMap.find(key)->second;

	}else{
		return false;
	}
}

map<string, string> muTable::getMap(){
    return symMap;
}
