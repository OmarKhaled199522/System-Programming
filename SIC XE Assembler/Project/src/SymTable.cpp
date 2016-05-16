/*
 * SymTable.cpp
 *
 *  Created on: Apr 24, 2016
 *      Author: dod
 */


#include <iostream>
#include <map>
#include <string>
#include "SymTable.h"
using namespace std;

bool SymTable::exists(string str){

	map<string, string>::iterator it;
	it = symMap.find(str);
	if(it != symMap.end()){
		return true;
	}
	return false;
}

void SymTable::add(string key, string address){
	symMap[key] = address;
	relMap[key] = false;
}

string SymTable::get(string key){
	if(exists(key)){
		return symMap.find(key)->second;

	}else{
		return "";
	}
}

void SymTable::makeAbsolute(string key){
    relMap[key] = true;
}

bool SymTable::isAbs(string key){
    if(exists(key)){
		return relMap.find(key)->second;

	}else{
		return false;
	}
}

map<string, string> SymTable::getMap(){
    return symMap;
}
