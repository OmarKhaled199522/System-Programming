/*
 * Parser.h
 *
 *  Created on: Apr 24, 2016
 *      Author: dod
 */

#ifndef PARSER_H_
#define PARSER_H_
#include <string>
#include <vector>
#include <iostream>
#include <LiteralTable.h>
using namespace std;

class Parser
{
public:

    void Parse(string fileName);
    string src [1000][5];
    bool isDir(string word);
    int getN();
    int stringcmp(string one, string two);
    LiteralTable literalTable;
private:
    vector<string> split(string line);
    void build(vector<string> vec);
    int getCommentInd(int first, vector<string> vec);
    string setOperand(int first, int last, vector<string> vec);
    void checkLiteral(string literal);
    void literalHandler();
    void handler();
};

#endif /* PARSER_H_ */
