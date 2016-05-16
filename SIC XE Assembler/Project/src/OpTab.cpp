#include "OpTab.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include<string.h>
#include <map>

using namespace std;

bool OpTab::exists(string str){

	map<string, string>::iterator it;
	transform(str.begin(), str.end(), str.begin(), ::toupper);
	it = opMap.find(str);
	if(it != opMap.end()){
		return true;
	}
	return false;
}
void OpTab::setMap(){
    string word, opWord;

    ifstream opsFile ("ops.txt");

    int i = 0;
    while(opsFile >> word)
    {
        if(i++ % 2 == 0)
            opWord = word;
        else
            opMap[opWord] = word;
    }
    opsFile.close();
}
OpTab::OpTab(){
	setMap();
}

map<string, string> OpTab::getMap(){
    return opMap;
}

string OpTab::get(string key){
	transform(key.begin(), key.end(), key.begin(), ::toupper);
	if(exists(key)){
		return opMap.find(key)->second;

	}else{
		return "";
	}
}
