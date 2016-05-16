#ifndef GENERATEOBJECTCODE_H_
#define GENERATEOBJECTCODE_H_
#include <string>
#include <Parser.h>
#include <regex>
#include <string.h>
#include <ByteTable.h>
#include <SymTable.h>
#include <OpTab.h>
#include <bits/stdc++.h>
#include <LiteralTable.h>
#include <vector>
#include <muTable.h>
#include<EquSolver.h>

using namespace std;

class GenerateObjectCode
{
    public:
        muTable m = muTable();
        string get(string key);
        map<string, string> getMap();
        bool exists(string str);
        void add(string key, string address);
        string address;
        vector<string> obCode;
        map<string, string> regsisterMap;
        void multi_passes(string src [][5], int len, muTable mu_table);
        bool fi(string m , char c);
        string change(string exp, muTable my_table);
        void start(string src[][5], SymTable symTable, LiteralTable lit_table, int lennn);
        void print_output_file(Parser p, string src[][5], int length);
        void readRegistersMap();
        bool isHex(string word);
        bool isNum(string word);
        int toDecimal(string str);
        int strToDev(string str);
        void split(string data[], string temp, char t);
        bool is_exist(string temp, char t);
        bool is_relative_address(string target_address, string pc, bool base_used, string base_address, int i, string object_code[], string menomonicCode, int add_m, string add_pc, string add_base);
        void base_conv(int n, int b);
        int getRegistersNum(string menomonic);

        private:
        map<string, string> lll;

};

#endif // GENERATEOBJECTCODE_H
