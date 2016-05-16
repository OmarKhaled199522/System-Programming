#include "Engine.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <Parser.h>
#include <regex>
#include <string.h>
#include <ByteTable.h>
#include <SymTable.h>
#include <LiteralTable.h>
#include<muTable.h>
#include<EquSolver.h>

using namespace std;

ofstream myfile;

void Engine::perform(string fileName)
{

    myfile.open ("output.txt");
    myfile << "Line\t" << "Address\t" << "Label\t\t" << "Mnemonic\t" << "Operand\t\t\t\t" << "Comment\n" << endl;
    ByteTable byteTable = ByteTable();
    p.Parse(fileName);
    int len = p.getN();

    multi_passes(p.src, len, mul_table);
    int j = 0;
    string start = "START";
    string byte = "BYTE";
    string resb = "RESB";
    string resw = "RESW";
    string word = "WORD";
    string en = "END";
    string base = "BASE";
    string nobase = "NOBASE";
    string ltorg = "LTORG";
    string org = "ORG";
    string equ = "EQU";
    while(j < len && (stringcmp(p.src[j][2].c_str(), start.c_str()) != 0))
    {
        if(p.src[j][2] != "")
        {
            p.src[j][0] = "0000";
            break;
        }
        printLine(j);
        j++;
    }
    if(j == len)
    {
        myfile << "\t\tThere is no code !!"<< endl;
        return;
    }
    if((stringcmp(p.src[j][2].c_str(), start.c_str()) == 0))
    {
        p.src[j][0] = p.src[j][3];
        if(!isHex(p.src[j][0]))
        {
            myfile << "\t\terror: operand is not hexadicimal number '"<< p.src[j][0] << "'" << endl;
            p.src[j][0] = "0000";
        }
        if(p.src[j][0] == "")
        {
            p.src[j][0] = "0000";
        }
        if(p.src[j][0].size() > 4)
        {
            myfile << "\t\terror: operand is greater than 4 digits"<< endl;
            p.src[j][0] = "0000";
        }
        printLine(j);
        j += 1;
        p.src[j][0] = p.src[j-1][0];
    }
    else
    {
        p.src[j][0] = "0000";
    }
    for(int i = j; i < len ; i++)
    {
        //printLine(i);
        if(i == len - 1)
        {
            printLine(i);
            if(stringcmp(p.src[i][2].c_str(), en.c_str()) == 0)
            {
                if(p.src[i][1] != "")
                {
                    myfile << "\t\terror: label field must be blank in END Statement ' " << p.src[i][1] << " '" << endl;
                }
                break;
            }
            else
            {
                myfile << "\t\terror: program must terminated with END statment" << endl;
                break;
            }
        }

        if(stringcmp(p.src[i][2].c_str(), start.c_str()) == 0)
        {
            printLine(i);
            myfile << "\t\tdouble start "  << endl;
            p.src[i+1][0] = p.src[i][0];
            continue;
        }

        if((stringcmp(p.src[i][2].c_str(), base.c_str()) == 0) && (p.src[i][1] != ""))
        {
            printLine(i);
            myfile << "\t\terror there mustn't be a symbol in label field !!" << endl;
        }

        if((stringcmp(p.src[i][2].c_str(), nobase.c_str()) == 0) || (stringcmp(p.src[i][2].c_str(), ltorg.c_str()) == 0))
        {
            if((p.src[i][1] != "") || (p.src[i][3] != ""))
            {
                printLine(i);
                myfile << "\t\terror there mustn't be a symbol in label or operand field !!" << endl;
            }
        }

        if(p.src[i][2] != "")
        {
            // EQU
            if(stringcmp(p.src[i][2].c_str(), equ.c_str()) == 0)
            {
                if((p.src[i][1] == "") || (p.src[i][3] == ""))
                {
                    printLine(i);
                    myfile << "\t\terror label and operand field must not be empty!!" << endl;
                    p.src[i+1][0] = p.src[i][0];
                    continue;
                }
                if(isAbsolute(p.src[i][3]))
                {
                    p.src[i+1][0] = p.src[i][0];
                    p.src[i][0] = address;
                    symtable.add(p.src[i][1], p.src[i][0]);
                    symtable.makeAbsolute(p.src[i][1]);
                    printLine(i);
                    continue;
                }
                else if(isRelative(p.src[i][3]))
                {
                    p.src[i+1][0] = p.src[i][0];
                    p.src[i][0] = address;
                    printLine(i);
                }
                else if(p.src[i][3] == "*")
                {
                    printLine(i);
                    p.src[i+1][0] = p.src[i][0];
                }
                else
                {
                    printLine(i);
                    myfile << "\t\tError : invalid operand!!" << p.src[i][3] << endl;
                    p.src[i+1][0] = p.src[i][0];
                    continue;
                }
            } // ORG
            else if(stringcmp(p.src[i][2].c_str(), org.c_str()) == 0)
            {
                printLine(i);
                if(p.src[i][1] != "")
                {
                    myfile << "\t\terror there mustn't be a symbol in label field !!" << endl;
                }
                if(isRelative(p.src[i][3]))
                {
                    p.src[i+1][0] = address;
                }
                else if(p.src[i][3] == "*")
                {
                    //do nothing
                }
                else
                {
                    myfile << "\t\tError : invalid operand!!" << p.src[i][3] << endl;
                    p.src[i+1][0] = p.src[i][0];
                }
            }
            else if(p.src[i][1] == "*")
            {
                printLine(i);
                address = "";
                int size = 0;
                string operand = p.src[i][2];
                if(regex_match(p.src[i][2], regex("=(X|x)'.*'")))
                {
                    operand = operand.substr(3, (operand.size()-4));
                    size = ceil((double) operand.size()/2);
                    if(isHex(operand))
                    {
                        if(operand.size() > 14)
                        {
                            size = 7;
                            myfile << "\t\terror: operand is more than 14 hexa digits '"<< operand << "'" << endl;
                        }
                    }
                    else
                    {
                        myfile << "\t\terror: operand is not hexadicimal number '"<< operand << "'" << endl;
                    }
                }
                else if(regex_match(p.src[i][2], regex("[=][*]"))){
                    size = 0;
                }
                else
                {
                    operand = operand.substr(3, (operand.size()-4));
                    size = operand.size();
                    if(operand.size() > 15)
                    {
                        size = 15;
                        myfile << "\t\terror: operand is more than 15 characters '"<< operand << "'" << endl;
                    }
                }
                address = "";
                base_conv(toDecimal(p.src[i][0]) + size, 16);
                p.src[i+1][0] = address;
                p.literalTable.add(p.src[i][2], p.src[i][0]);
            }
            else if(byteTable.exists(p.src[i][2]))
            {
                printLine(i);
                string type = byteTable.get(p.src[i][2]);
                address = "";
                base_conv(toDecimal(p.src[i][0]) + toDecimal(type), 16);
                p.src[i+1][0] = address;
            }
            else
            {
                printLine(i);
                if(stringcmp(p.src[i][2].c_str(), word.c_str()) == 0)
                {
                    address = "";
                    base_conv(toDecimal(p.src[i][0]) + toDecimal("3"), 16);
                    p.src[i+1][0] = address;

                }
                else if(stringcmp(p.src[i][2].c_str(), byte.c_str()) == 0)
                {
                    address = "";
                    string operand = p.src[i][3];
                    if(!regex_match(operand, regex("(C|c|X|x)'.*'")))
                    {
                        myfile << "\t\terror in matching operand field  " << endl;
                        p.src[i+1][0] = p.src[i][0];
                        continue;
                    }
                    int size = 0;
                    operand = operand.substr(2, (operand.size()-3));
                    if(regex_match(p.src[i][3], regex("(X|x)'.*'")))
                    {
                        size = ceil((double) operand.size()/2);
                        if(isHex(operand))
                        {
                            if(operand.size() > 14)
                            {
                                size = 7;
                                myfile << "\t\terror: operand is more than 14 hexa digits '"<< operand << "'" << endl;
                            }
                        }
                        else
                        {
                            myfile << "\t\terror: operand is not hexadicimal number '"<< operand << "'" << endl;
                        }
                    }
                    else
                    {
                        size = operand.size();
                        if(operand.size() > 15)
                        {
                            size = 15;
                            myfile << "\t\terror: operand is more than 15 characters '"<< operand << "'" << endl;
                        }
                    }
                    address = "";
                    base_conv(toDecimal(p.src[i][0]) + size, 16);
                    p.src[i+1][0] = address;
                }
                else if(stringcmp(p.src[i][2].c_str(), resw.c_str()) == 0)
                {
                    string numberOfCells = p.src[i][3];
                    if((!isNum(numberOfCells)) || (numberOfCells.size()>4))
                    {
                        numberOfCells = mul_table.get(p.src[i][1]);
                        if((!isNum(numberOfCells)) || (numberOfCells.size()>4)){
                            myfile << "\t\terror: Wrong number in operand field  '" << p.src[i][3] << "'" << endl;
                            p.src[i+1][0] = p.src[i][0];
                            continue;
                        }
                    }
                    int z = strToDev(numberOfCells) * 3;
                    address = "";
                    base_conv(toDecimal(p.src[i][0]) + z, 16);
                    p.src[i+1][0] = address;
                }
                else if(stringcmp(p.src[i][2].c_str(), resb.c_str()) == 0)
                {
                    string numberOfCells = p.src[i][3];
                    if((!isNum(numberOfCells)) || (numberOfCells.size()>4))
                    {
                        numberOfCells = mul_table.get(p.src[i][1]);
                        if((!isNum(numberOfCells)) || (numberOfCells.size()>4)){
                            myfile << "\t\terror: Wrong number in operand field  '" << p.src[i][3] << "'" << endl;
                            p.src[i+1][0] = p.src[i][0];
                            continue;
                        }
                    }
                    address = "";
                    base_conv(toDecimal(p.src[i][0]) + strToDev(numberOfCells), 16);
                    p.src[i+1][0] = address;
                }
                else
                {
                    myfile << "\t\terror: invalid operation code '" << p.src[i][2] << "'" << endl;
                    p.src[i+1][0] = p.src[i][0];
                }
            }
        }
        else
        {
            printLine(i);
            p.src[i+1][0] = p.src[i][0];
        }

        if((p.src[i][1] != "") && (stringcmp(p.src[i][2].c_str(), base.c_str()) != 0)
                && (stringcmp(p.src[i][2].c_str(), nobase.c_str()) != 0)
                && (stringcmp(p.src[i][2].c_str(), org.c_str()) != 0)
                && (stringcmp(p.src[i][2].c_str(), ltorg.c_str()) != 0)
                && (p.src[i][1] != "*"))
        {
            if(symtable.exists(p.src[i][1]))
            {
                //error
                myfile << "\t\terror label ' "  << p.src[i][1] << " ' is dublicated" << endl;
            }
            else
            {
                if(byteTable.exists(p.src[i][1]) || p.isDir(p.src[i][1]))
                {
                    myfile << "\t\terror label can't be a mnemonic or directive  ' " << p.src[i][1] << " '" << endl;
                }
                else
                {
                    symtable.add(p.src[i][1], p.src[i][0]);
                }
            }
        }
    }
    printSymTable();
    for(int i = 0 ; i < len ; i++){
        string temp = mul_table.get(p.src[i][1]);
        if(temp != ""){
            p.src[i][3] = temp;
            cout<<p.src[i][3]<<"----------------------------";
        }
    }
    return;
}
bool Engine::isHex(string word)
{
    int size = word.size();
    for(int i=0; i<size; i++)
    {
        if(!isxdigit(word[i]))
        {
            return false;
        }
    }
    return true;
}
string Engine::toHex(int number)
{
    if(number / 16 == 0)
        return "" + ('0' +  number);
    string hex = "";
    hex += ("" + ('0' + number % 16)) + toHex(number / 16);
    return hex;
}

bool Engine::isNum(string word)
{
    bool isNumber = true;
    if(word == "") return false;
    for(string::const_iterator k = word.begin(); k != word.end(); ++k)
        isNumber &= isdigit(*k);
    return isNumber;
}

void Engine::base_conv(int n, int b)
{
    if(n==0)
        return;

    base_conv(n/b, b);
    int current_digit = n%b;

    if (current_digit < 10)
        address += '0'+ current_digit;
    else
    {
        address += (current_digit)-10+'A';
    }
}
int Engine::toDecimal(string str)
{
    int number = 0;
    int temp = 0;
    int k = 0;
    for(int i = str.length()-1 ; i >= 0 ; i--)
    {
        if(!isdigit(str[i]))
        {
            temp = (int)(str[i]-'0') - 7;

        }
        else
        {
            temp = (int)(str[i] - '0');
        }
        number += temp * pow(16, k);
        k++;

    }
    return number;
}

Engine::Engine(string fileName)
{
    perform(fileName);
}

void Engine::printSymTable()
{
    myfile << "\n\n\t\t****************************************\n";
    myfile << "\n\n\t\t\tS Y M B O L     T A B L E\n\n";
    myfile << "\t\t\tLabel\t\tAddress\n\n";
    for (auto i : symtable.getMap())
    {
        myfile << "\t\t\t" << i.first.c_str() << "\t\t" << i.second.c_str() << endl;
    }
    myfile.close();
}

void Engine::printLine(int lineNum)
{
    if( (lineNum > 0) && (p.src[lineNum][0]=="") )
    {
        p.src[lineNum][0] = p.src[lineNum-1][0];
    }
    myfile << (lineNum+1) << "\t" << p.src[lineNum][0] << "\t"
           << p.src[lineNum][1] << "\t\t" << p.src[lineNum][2] << "\t\t"
           << p.src[lineNum][3] << "\t\t\t\t" << p.src[lineNum][4] << endl;
}
int Engine::stringcmp(string one, string two)
{

    int counter = 0;
    if(one.size() == two.size())
    {
        int len = one.size();
        for(int i = 0 ; i < len ; i++)
        {
            if(tolower(one[i]) == tolower(two[i]))
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

bool Engine::isRelative(string operand)
{

    if(regex_match(operand, regex(".+(\\+|\\-)[0-9]+")))
    {
        smatch m;
        regex_search (operand, m, regex("\\+|\\-"));
        if(symtable.exists(operand.substr(0,m.position(0))))
        {
            calculate(operand.substr(0,m.position(0)), m[0], operand.substr(m.position(0)+1,operand.size()));
            return true;
        }
        return false;
    }
    else if(regex_match(operand, regex("[0-9]+\\+.+")))
    {
        smatch m;
        regex_search (operand, m, regex("\\+"));
        if(symtable.exists(operand.substr(m.position(0)+1,operand.size())))
        {
            calculate(operand.substr(0,m.position(0)), m[0], operand.substr(m.position(0)+1,operand.size()));
            return true;
        }
        return false;
    }
    else if(symtable.exists(operand))
    {
        address = symtable.get(operand);
        return true;
    }
    return false;
}

bool Engine::isAbsolute(string operand)
{

    if(regex_match(operand, regex("[0-9]+((\\+|\\-|\\*|\\/)[0-9]+)*")))
    {
        if(isNum(operand))
        {
            address = "";
            base_conv(strToDev(operand), 16);
            return true;
        }
        smatch m;
        regex_search (operand, m, regex("\\+|\\-|\\*|\\/"));
        calculate(operand.substr(0,m.position(0)), m[0], operand.substr(m.position(0)+1,operand.size()));
        return true;
    }
    else if(regex_match(operand, regex(".+\\-.+")))
    {
        smatch m;
        regex_search (operand, m, regex("\\-"));
        if(symtable.exists(operand.substr(0,m.position(0))) && symtable.exists(operand.substr(m.position(0)+1,operand.size())))
        {
            calculate(operand.substr(0,m.position(0)), m[0], operand.substr(m.position(0)+1,operand.size()));
            return true;
        }
        return false;
    }
    else if(regex_match(operand, regex(".+(\\+|\\-|\\*|\\/).+")))
    {
        smatch m;
        regex_search (operand, m, regex("(\\+|\\-|\\*|\\/)"));
        if(symtable.isAbs(operand.substr(0,m.position(0))) && symtable.isAbs(operand.substr(m.position(0)+1,operand.size())))
        {
            calculate(operand.substr(0,m.position(0)), m[0], operand.substr(m.position(0)+1,operand.size()));
            return true;
        }
        return false;
    }
    else if(regex_match(operand, regex(".+(\\+|\\-|\\*|\\/)[0-9]+")))
    {
        smatch m;
        regex_search (operand, m, regex("\\+|\\-|\\*|\\/"));
        if(symtable.isAbs(operand.substr(0,m.position(0))))
        {
            calculate(operand.substr(0,m.position(0)), m[0], operand.substr(m.position(0)+1,operand.size()));
            return true;
        }
        return false;
    }
    else if(regex_match(operand, regex("[0-9]+(\\+|\\-|\\*|\\/).+")))
    {
        smatch m;
        regex_search (operand, m, regex("\\+|\\-|\\*|\\/"));
        if(symtable.isAbs(operand.substr(m.position(0)+1,operand.size())))
        {
            calculate(operand.substr(0,m.position(0)), m[0], operand.substr(m.position(0)+1,operand.size()));
            return true;
        }
        return false;
    }
    else if(symtable.isAbs(operand))
    {
        address = symtable.get(operand);
        return true;
    }
    return false;
}
void Engine::multi_passes(string src [][5], int len, muTable mu_table){
    int size_arr = 0;
    for(int i = 0 ; i < len ; i++){
        if(src[i][1] != ""){
            if(src[i][2] == "WORD" ||src[i][2] == "RESW" ||src[i][2] == "RESB"){
                size_arr++;
            }
        }
    }
    string temp[size_arr][4];
    int j = 0;
    for(int i = 0 ; i < len ; i++){
        if(src[i][1] != ""){
            if(src[i][2] == "WORD" ||src[i][2] == "RESW" ||src[i][2] == "RESB"){
              temp[j][0] = src[i][1];
              temp[j][1] = src[i][2];
              temp[j][2] = src[i][3];
              if(isNum(temp[j][2])){
                temp[j][3] = "1";
                mu_table.add(temp[j][0],temp[j][2]);
              }
              else{
                temp[j][3] = "0";
              }
              j++;
            }
        }
    }
    bool is_done = false;
    for(int i = 0 ; i < size_arr && !is_done; i++){
        for(int k = 0 ; k < size_arr ; k++){
            is_done = true;
            if(temp[k][3] == "0"){
                string exp = temp[k][2];
                string result = change(exp,mu_table);
                bool b = false;
                while(result != "done" && result != "error" && result != "not_used"){
                    exp = result;
                    if(isNum(exp)){
                        temp[k][3] = "1";
                        temp[k][2] = exp;
                        b = true;
                        break;
                    }
                    result = change(exp,mu_table);
                }
                if(b){
                    continue;
                }
                if(result == "done"){
                    temp[k][2] = exp;
                    if(isNum(exp)){
                        temp[k][3] = "1";
                        mu_table.add(temp[k][0],temp[k][2]);
                    }
                    else{
                        EquSolver s = EquSolver();
                        int z = s.start(exp);
                        address = "";
                        base_conv(z,10);
                        temp[k][2] = address;
                        temp[k][3] = "1";
                        mu_table.add(temp[k][0],address);
                    }
                    continue;
                }
                if(result == "error"){
                  //error

                }
                if(result == "not_used"){
                    is_done = false;
                }
            }
        }
    }
    //for(int i = 0 ; i < size_arr ; i++){
     //   cout << temp[i][0] << "  " << temp[i][1] << "  " << temp[i][2] << "  " << temp[i][3] << '\n';
    //}

}

bool Engine::fi(string m , char c){
    for(int i = 0 ; i < m.length() ; i++){
        if(m[i] == c){
            return true;
        }
    }
    return false;
}

string Engine::change(string exp, muTable my_table){
    int len = exp.length();
    string t = "()+-*/%^";
    string m = "()+-*/1234567890%^";
    bool k = false;
    int f = 0;
    int l = 0;
    for(int i = 0 ; i < len ; i++){
        if(!k){
           if(!fi(m,exp[i])){
                f = i;
                k = true;
                continue;
            }
            else{
                continue;
            }
        }else{
            if(i == len -1){
                l = i;
                break;
            }
            if(fi(t,exp[i])){
                l = i-1;
                break;
            }
        }
    }
    if(k){
        string temp_s = "";
        for(int i = f ; i <= l ; i++){
            temp_s += exp[i];
        }
        string operand = my_table.get(temp_s);
        if(operand == ""){
            cout << "error un declear label";
            return "error";
        }
        else{
            if(isNum(operand)){
                string exp_new = "";
                for(int i = 0 ; i < len ; i++){
                    if(i != f){
                        exp_new += exp[i];
                    }
                    else{
                        exp_new += operand;
                        i = l ;
                    }
                }
                return exp_new;
            }
            else{
                return "not_used";
            }
        }

    }else{
       return  "done";
    }
}

int Engine::strToDev(string str)
{
    int number = 0;
    int temp = 0;
    int k = 0;

    for(int i = str.length()-1 ; i >= 0 ; i--)
    {
        if(!isdigit(str[i]))
        {
            temp = (int)(str[i]-'0') - 7;
        }
        else
        {
            temp = (int)(str[i] - '0');
        }
        number += temp * pow(10, k);
        k++;
    }

    return number;
}

void Engine::calculate(string left, string operat, string right)
{

    int n1, n2;
    if(isNum(left))
    {
        n1 = strToDev(left);
    }
    else
    {
        n1 = toDecimal(symtable.get(left));
    }

    if(isNum(right))
    {
        n2 = strToDev(right);
    }
    else
    {
        n2 = toDecimal(symtable.get(right));
    }

    address = "";
    if(operat=="+")
    {
        base_conv(n1+n2, 16);
    }
    else if(operat=="-")
    {
        base_conv(n1-n2, 16);
    }
    else if(operat=="*")
    {
        base_conv(n1*n2, 16);
    }
    else if(operat=="/")
    {
        base_conv(n1/n2, 16);
    }
}
