#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <Parser.h>
#include <fstream>
#include <SymTable.h>
#include <LiteralTable.h>
#include <muTable.h>
#include<EquSolver.h>


using namespace std;

class Engine
{

public:
    Parser p = Parser();
    SymTable symtable = SymTable();
    LiteralTable lit_table = LiteralTable();
    string address = "";
    muTable mul_table = muTable();

public:
    Engine(string fileName);

private:
    int stringcmp(string one, string two);
    void perform(string fileName);
    int toDecimal(string str);
    void base_conv(int n, int b);
    int strToDev(string str);
    string toHex(int number);
    bool isNum(string word);
    void printLine(int lineNum);
    bool isHex(string word);
    void printSymTable();
    bool isAbsolute(string operand);
    bool isRelative(string operand);
    void calculate(string left, string operat, string right);
    void multi_passes(string src [][5], int len, muTable mu_table);
    bool fi(string m , char c);
    string change(string exp, muTable my_table);
};

#endif // ENGINE_H
