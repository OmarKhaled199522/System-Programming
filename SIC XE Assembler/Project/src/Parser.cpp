/*
 * Parser.cpp
 *
 *  Created on: Apr 24, 2016
 *      Author: dod
 */

#include "Parser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include<string.h>
#include<ByteTable.h>
#include <LiteralTable.h>
#include <map>
#include <utility>

using namespace std;

string src[1000][5];
int counter = 0;
ByteTable bytetable;
LiteralTable literalTable;
int literalCounter = 0;

void Parser::Parse(string fileName)
{

    string line;
    ifstream myfile(fileName.c_str());
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            vector<string> wordsVector = split(line);
            build(wordsVector);
            counter++;
        }
        myfile.close();
    }
    else
        cout << "Unable to open file";

}

void Parser::build(vector<string> vec)
{

    if (vec.size() == 0);
    else if(vec.at(0)[0]=='.')
    {
        ostringstream imploded;
        copy(vec.begin(), vec.end(), ostream_iterator<string>(imploded, " "));
        src[counter][4] = imploded.str();

    }
    else if (vec.size() == 1) src[counter][2] = vec.at(0);
    else
    {
        int first, first1;
        string byte = "BYTE";
        string rsub = "RSUB";
        if( bytetable.exists(vec.at(1)) || isDir(vec.at(1)) )
        {
            first = 2;
            src[counter][1] = vec.at(0);
            src[counter][2] = vec.at(1);
            if((vec.size() != 2) && (stringcmp(vec.at(1).c_str(), rsub.c_str()) != 0))
            {
                first = getCommentInd(2, vec);
                first1 = 2;
                src[counter][3] = setOperand(first1, first, vec);
                checkLiteral(src[counter][3]);
            }
        }
        else if(stringcmp(vec.at(1).c_str(), byte.c_str()) == 0)
        {
            src[counter][1] = vec.at(0);
            src[counter][2] = vec.at(1);
            first = 2;
            if(vec.size() != 2)
            {
                if(regex_match(vec.at(2), regex("(C|c|X|x)'.*'")))
                {
                    src[counter][3] = vec.at(2);
                    first = 3;
                }
                else
                {
                    first = 2;
                    for(vector<int>::size_type i = 3; i != vec.size(); i++)
                    {
                        if(regex_match(vec.at(i), regex(".*'")))
                        {
                            ostringstream imploded;
                            copy(vec.begin() + 2, vec.end() - (vec.size()-i-1), ostream_iterator<string>(imploded, " "));
                            src[counter][3] = imploded.str().substr(0, imploded.str().size()-1);
                            first = i+1;
                            break;
                        }
                    }
                }
            }
            first1 = first;
            first = getCommentInd(first, vec);
            src[counter][3] = src[counter][3] + setOperand(first1, first, vec);
        }
        else
        {
            first = 1;
            src[counter][2] = vec.at(0);
            if(stringcmp(vec.at(0).c_str(), rsub.c_str()) != 0)
            {
                first1 = first;
                first = getCommentInd(first, vec);
                src[counter][3] = setOperand(first1, first, vec);
                checkLiteral(src[counter][3]);
            }
        }
        ostringstream imploded;
        copy(vec.begin() + first, vec.end(), ostream_iterator<string>(imploded, " "));
        src[counter][4] = imploded.str();
    }
    string ltOrg = "LTORG";
    string ltEnd = "END";
    if(stringcmp(src[counter][2].c_str(), ltOrg.c_str()) == 0)
    {
        literalHandler();
    }
    else if(stringcmp(src[counter][2].c_str(), ltEnd.c_str()) == 0)
    {
        handler();
    }
}

string Parser::setOperand(int first, int last, vector<string> vec)
{

    ostringstream imploded;
    copy(vec.begin() + first, vec.end() - (vec.size()-last), ostream_iterator<string>(imploded, ""));
    string operand = imploded.str();
    return operand;
}

int Parser::getCommentInd(int first, vector<string> vec)
{

    int len = vec.size();
    int i=0;
    for(i=first; i<len; i++)
    {
        if(vec.at(i)[0]=='.') break;
    }
    return i;
}

vector<string> Parser::split(string line)
{

    int size = line.size();
    char str[size];
    for(int i = 0; i < size; i++)
        str[i] = line[i];

    str[size] = '\0';

    char * pch;

    pch = strtok (str,"\t ");

    vector<string> wordsVector;

    while (pch != NULL)
    {
        wordsVector.push_back(pch);
        pch = strtok (NULL, "\t ");
    }
    return wordsVector;
}

bool Parser::isDir(string word)
{
    string directives[] = { "START", "RESW", "RESB", "WORD", "END"};
    for(int i=0; i<5; i++)
    {
        if(stringcmp(directives[i].c_str(), word.c_str()) == 0)
        {
            return true;
        }
    }
    return false;
}

int Parser::getN()
{

    return counter;
}
int Parser::stringcmp(string one, string two)
{

    int counter = 0;
    if(one.size() == two.size())
    {
        int len = one.size();
        for(int i = 0 ; i < len ; i++)
        {
            if(std::tolower(one[i]) == std::tolower(two[i]))
            {
                counter++;
            }
            else
            {
                return 1;
            }
        }
        if(counter == len)
        {
            return 0;
        }
    }
    return 1;
}

void Parser::checkLiteral(string literal)
{
    if(regex_match(literal, regex("=(X|x|C|c)'.*'")) ||
            regex_match(literal, regex("=\\*")))
    {
        literalTable.add(literal, "*");
    }
}

void Parser::literalHandler()
{
    counter++;

    int index = 0;
    for(vector<pair<string,string>>::iterator it = literalTable.litVector.begin(); it != literalTable.litVector.end(); it++)
    {
        if(index < literalCounter)
        {
            index++;
            continue;
        }
        string key = it->first;
        src[counter][1] = "*";
        src[counter][2] = key;
        counter++;
    }

    literalCounter = literalTable.litVector.size();
}

void Parser::handler()
{
    src[counter + literalTable.litVector.size() - literalCounter][2] = src[counter][2];
    src[counter + literalTable.litVector.size() - literalCounter][3] = src[counter][3];
    src[counter + literalTable.litVector.size() - literalCounter][4] = src[counter][4];
    src[counter][3] = "";
    src[counter][4] = "";

    int index = 0;
    for(vector<pair<string,string>>::iterator it = literalTable.litVector.begin(); it != literalTable.litVector.end(); it++)
    {
        if(index < literalCounter)
        {
            index++;
            continue;
        }
        string key = it->first;
        src[counter][1] = "*";
        src[counter][2] = key;
        counter++;
    }

    literalCounter = literalTable.litVector.size();
}
