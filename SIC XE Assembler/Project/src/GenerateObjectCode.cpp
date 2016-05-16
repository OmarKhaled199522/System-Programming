#include "GenerateObjectCode.h"
#include <Parser.h>
#include <regex>
#include <string.h>
#include <ByteTable.h>
#include <SymTable.h>
#include <OpTab.h>
#include <LiteralTable.h>
#include<bits/stdc++.h>
#include <vector>
#include<EquSolver.h>

using namespace std;

int mod_size;
string address;
map<string, string> regsisterMap;
vector<string> vec;

void GenerateObjectCode::add(string key, string address){
	lll[key] = address;
}

string GenerateObjectCode::get(string key){
	if(exists(key)){
		return lll.find(key)->second;

	}else{
		return "";
	}
}

bool GenerateObjectCode::exists(string str){

	map<string, string>::iterator it;
	it = lll.find(str);
	if(it != lll.end()){
		return true;
	}
	return false;
}

map<string, string> GenerateObjectCode::getMap(){
    return lll;
}

void GenerateObjectCode::readRegistersMap(){

    ifstream regsFile ("regs.txt");

    if(regsFile.is_open())
    {
        string word, regName;
        int i = 0;

        while(regsFile >> word)
        {
            if(i++ % 2 == 0)
                regName = word;
            else
                regsisterMap[regName] = word;
        }
        regsFile.close();
    }
}

void GenerateObjectCode:: start(string src[][5], SymTable symTable, LiteralTable lit_table, int lennn){

    multi_passes(src, lennn, m);
    readRegistersMap();
    int rowsNum = 1000;
    string object_code[rowsNum];
    vector<string> modification_record;
    ByteTable byteTable = ByteTable();
    OpTab opTable = OpTab();
    bool base_used = false;
    string base_address;
    int hh = lennn;

    for(int i = 0; i < hh; i++){
        cout << " 0 " << src[i][0] << "\n";
        cout << " 1 " << src[i][1] << "\n";
        cout << " 2 " << src[i][2] << "\n";
        cout << " 3 " << src[i][3] << "\n";
        cout << " 4 " << src[i][4] << "\n";

        regex is_literal_string ("\\s*[=][C]['](.+)[']\\s*");
        regex is_literal_hexa ("\\s*[=][X]['][0-9A-F]+[']\\s*");
        if(regex_match(src[i][2], is_literal_string)){
            string obj_code = "";
            int len = (src[i][2].length()-1);
            for(int k = 3 ; k < len ; k++){
                int dec_val = src[i][2][k];
                address = "";
                base_conv(dec_val, 16);
                obj_code += address;
            }
            object_code[i] = obj_code;
            cout<<obj_code;
            continue;
        }
        if(regex_match(src[i][2], is_literal_hexa)){
            string obj_code = "";
            int len = (src[i][2].length()-1);
            for(int k = 3 ; k < len ; k++){
                obj_code += src[i][2][k];
            }
            object_code[i] = obj_code;
            cout<<obj_code;
            continue;
        }
        if(src[i][2] == "RSUB" && src[i][3] != ""){
            printf("RSUB should't has operand");
            continue;
        }
        if(src[i][2] == ""){
            object_code[i] = "-1";
            continue;

        } else if(src[i][2] == "BASE"){
            if(src[i][3] == "*"){
                base_address = src[i][0];
                base_used = true;
            }else{
                string operand = src[i][3];
                base_address = symTable.get(operand);
                if(base_address == ""){
                    printf("label does't exist base statement");
                    continue;
                }
                base_used = true;
            }
        } else if(src[i][2] == "NOBASE"){
            base_used = false;
            base_address = "";
            if(src[i][1]!= "" || src[i][3] != ""){
                printf("NOBASE statement does't have label nor operand ");
                continue;
            }

        }else if(src[i][2] == "WORD"){//lazm nshoof el max length lel word
            regex integer ("\\d+");
            if(regex_match(src[i][3], integer)){
               address = "";
               int decimal1 = toDecimal(src[i][3]);

               base_conv(decimal1, 10);
               if(address.length() > 4){
                    printf("error number more than 4decimal bits");
                    continue;
               }
               address = "";
               base_conv(decimal1, 16);
               object_code[i] = address;
               cout<<address;
               continue;

            }
            else if(isNum(get(src[i][1]))){
               address = "";
               int decimal1 = toDecimal(get(src[i][1]));
               base_conv(decimal1, 10);
               if(address.length() > 4){
                    printf("error number more than 4decimal bits");
                    continue;
               }
               address = "";
               base_conv(decimal1, 16);
               object_code[i] = address;
               cout<<address;
               continue;
            }
            else{
                printf("WORD must have a numeric operand");
                continue;
            }
        }else if(src[i][2] == "BYTE"){
            regex in ("\\s*[C]['](.+)[']\\s*");
            regex in1 ("\\s*[X]['][0-9A-F]+[']\\s*");
            if(regex_match(src[i][3], in)){
                string obj_code = "";
                int len = (src[i][3].length()-1);
                for(int k = 2 ; k < len ; k++){
                    int dec_val = src[i][3][k];
                    address = "";
                    base_conv(dec_val, 16);
                    obj_code += address;
                }
                object_code[i] = obj_code;
                cout<<obj_code;
                continue;
            }else if(regex_match(src[i][3], in1)){
                string obj_code = "";
                int len = (src[i][3].length()-1);
                for(int k = 2 ; k < len ; k++){
                    obj_code += src[i][3][k];
                }
                object_code[i] = obj_code;
                cout<<obj_code;
                continue;
            }else{
                printf("wrong constant definition");
            }

        }else if(byteTable.exists(src[i][2])){

            string type = byteTable.get(src[i][2]);
            string menomonic = src[i][2];

            if(menomonic[0] == '+') menomonic.erase(0,1);
            string menomonicCode = opTable.get(menomonic);
            if(menomonicCode == ""){
                continue;
            }
            string operand = src[i][3];

            if(type.compare("2") == 0){

                int registersNum = getRegistersNum(menomonic);

                if(registersNum == 1){

                    if(operand.size() != 1){
                        //
                        //printf(".... you! hassan should handle it!!!!");
                        printf("error in format2, expected 1 register but size != 1");
                        //printf("hassan 3awz ykml el bernamg m3a en feeh error msh 3aref ezay!");
                        continue;

                    } else {

                        //string operand0 = "";
                        //operand0.push_back(operand[0]);
                        string registerCode = regsisterMap[ operand/*0*/];
                        if(registerCode.compare("") == 0){

                            printf("wrong register name!");
                            continue;
                        }

                        object_code[i] = menomonicCode + registerCode + "0";
                                cout << object_code[i] << "\n";

                        continue;
                    }

                } else {

                    if(operand.size() != 3){

                        //printf(".... you! hassan should handle it!!!!");
                        printf("error in format2, expected 2 register but size != 3");
                        //printf("hassan 3awz ykml el bernamg m3a en feeh error msh 3aref ezay!");
                        continue;

                    } else {

                        string operand1 = "";
                        string operand2 = "";
                        operand1.push_back(operand[0]);
                        operand2.push_back(operand[2]);
                        string registerCode1 = regsisterMap[operand1];
                        string registerCode2 = regsisterMap[operand2];
                        //cout<< operand1;
                        //cout<< operand2;

                        if(registerCode1.compare("") == 0 || registerCode2.compare("") == 0){

                            printf("wrong register name!");
                            continue;
                        }

                        if(operand[1] != ','){

                            printf("expected , in format 2");
                            continue;
                        }

                        object_code[i] = menomonicCode + registerCode1 + registerCode2;
                                cout << object_code[i] << "\n";

                    }
                }

            } else if(type.compare("3") == 0){
                if(operand[0] == '#' || operand[0] == '@'){

                    bool isNumber = true;
                    int len  = operand.size();
                    for(int j = 1; j < len && isNumber; j++){

                        if(operand[j] < '0' || operand[j] > '9')
                            isNumber = false;
                    }

                    if(isNumber){

                        int immediateNumber = strToDev(operand.substr(1));
                        address = "";
                        base_conv(immediateNumber , 16);
                        if(address.size() > 3){

                            printf("Immediate number is larger than 3 half bytes");
                            continue;

                        } else if (address.size() < 3){

                            while(address.size() != 3) address = "0" + address;

                        }

                        string hexaOpernadNum = address;
                        int tempNum = toDecimal(menomonicCode) + 1;
                        if(operand[0] == '@'){
                            tempNum++;
                        }
                        address = "";
                        base_conv(tempNum , 16);
                        while(address.size() != 2) address = "0" + address;
                        menomonicCode = address;
                        object_code[i] = menomonicCode + "0" + hexaOpernadNum;
                                cout << object_code[i] << "\n";

                        continue;

                    } else {
                        string operand_label = operand.substr(1);
                        string operand_address = symTable.get(operand_label);
                        if(operand_address == ""){
                            cout <<  "error label name " << operand_label;
                            continue;
                        }
                        if(operand[0] == '@'){
                            if(!is_relative_address(operand_address, src[i+1][0], base_used, base_address, i, object_code, menomonicCode, 2, "2" , "4")){
                            cout <<  "error no pc,base,number format 3 # / @" ;
                            }
                        }
                        else{
                            if(!is_relative_address(operand_address, src[i+1][0], base_used, base_address, i, object_code, menomonicCode, 1, "2" , "4")){
                            cout <<  "error no pc,base,number format 3 # / @" ;
                            }
                        }

                        continue;
                    }
                }
                if(is_exist(operand,',')){
                    string data[2];
                    split(data, operand, ',');
                    if(data[1] != "X"){
                        cout <<  "error no x , X" ;
                        continue;
                    }
                    string operand_address = symTable.get(data[0]);
                    if(operand_address != ""){
                        if(!is_relative_address(operand_address, src[i+1][0], base_used, base_address, i, object_code, menomonicCode, 3, "A" , "C")){
                            cout <<  "error no pc,base,number format 3  (X)" ;
                        }
                        continue;
                    }
                    else if( (isHex(data[0]) == true) && (data[0].length() <= 3) ){
                        while(data[0].size() != 3) data[0] = "0" + data[0];
                        int tempNum = toDecimal(menomonicCode) + 3;
                        address = "";
                        base_conv(tempNum , 16);
                        while(address.size() != 2) address = "0" + address;
                        menomonicCode = address;
                        object_code[i] = menomonicCode + "8" + data[0];
                                cout << object_code[i] << "\n";

                        continue;
                    }
                    else{
                        cout <<  "error no address valid format 3  (X)" ;
                        continue;
                    }
                }
                if(isHex(operand) && operand.length() <= 3){
                    while(operand.size() != 3) operand = "0" + operand;
                    int tempNum = toDecimal(menomonicCode) + 3;
                    address = "";
                    base_conv(tempNum , 16);
                    while(address.size() != 2) address = "0" + address;
                    menomonicCode = address;
                    object_code[i] = menomonicCode + "0" + operand;
                            cout << object_code[i] << "\n";

                    continue;
                }
                else{
                    string operand_address = symTable.get(operand);
                    if(operand_address == ""){
                        operand_address = lit_table.get(operand);
                        //cout<<operand_address<<"alksldnvjkfn.-----------";

                    }
                    if(operand_address != ""){
                        if(!is_relative_address(operand_address, src[i+1][0], base_used, base_address, i, object_code, menomonicCode, 3, "2" , "4")){
                            cout <<  "error no pc,base,number format 3  no ay 7aga " ;
                        }
                        continue;
                    }
                    else{
                        cout <<  "error label does not exist " ;
                        continue;
                    }
                }
            }
            else{//format 4
                string operand_address = symTable.get(operand);
                if(operand_address == ""){
                    operand_address = lit_table.get(operand);
                }
                if(operand_address != ""){
                    int tempNum = toDecimal(menomonicCode) + 3;
                    address = "";
                    base_conv(tempNum , 16);
                    while(address.size() != 2) address = "0" + address;
                    while(operand_address.size() != 5) operand_address = "0" + operand_address;
                    menomonicCode = address;
                    object_code[i] = menomonicCode + "1" + operand_address;
                            cout << object_code[i] << "\n";
                    int len = 5 - (src[i][0].length());
                    string m_record = "M";
                    for(int y = 0 ; y < len+1 ; y++){
                        m_record = m_record + "0";
                    }
                    int val = toDecimal(src[i][0]) + 1;
                    address = "";
                    base_conv(val, 16);
                    m_record = m_record + address;
                    m_record = m_record + "05";
                    cout<<m_record;
                    modification_record.push_back(m_record);
                    continue;
                }
                else{
                    if(is_exist(operand,',')){
                        string data[2];
                        split(data, operand, ',');
                        if(data[1] != "X"){
                            cout <<  "error no x , X" ;
                            continue;
                        }
                        string operand_address = symTable.get(data[0]);
                        if(operand_address != ""){
                            int tempNum = toDecimal(menomonicCode) + 3;
                            address = "";
                            base_conv(tempNum , 16);
                            while(address.size() != 2) address = "0" + address;
                            menomonicCode = address;
                            object_code[i] = menomonicCode + "9" + operand_address;
                                    cout << object_code[i] << "\n";
                            int len = 5 - (src[i][0].length());
                            string m_record = "M";
                            for(int y = 0 ; y < len+1 ; y++){
                                m_record = m_record + "0";
                            }
                            int val = toDecimal(src[i][0]) + 1;
                            address = "";
                            base_conv(val, 16);
                            m_record = m_record + address;
                            m_record = m_record + "05";
                            cout<<m_record;
                            modification_record.push_back(m_record);
                            continue;
                        }
                        else{
                            cout <<  "error label does not exist " ;
                            continue;
                        }
                    }
                    else{
                        if(operand[0] == '#' || operand[0] == '@'){
                            string operand_label = operand.substr(1);
                            string operand_address = symTable.get(operand_label);
                            if(operand_address == ""){
                                bool isNumber = true;
                                int len  = operand.size();
                                for(int j = 1; j < len && isNumber; j++){

                                    if(operand[j] < '0' || operand[j] > '9')
                                        isNumber = false;
                                }

                                if(isNumber){

                                    int immediateNumber = strToDev(operand.substr(1));
                                    address = "";
                                    base_conv(immediateNumber , 16);
                                    if(address.size() > 5){

                                        printf("Immediate number is larger than 5 half bytes");
                                        continue;

                                    } else if (address.size() < 5){

                                        while(address.size() != 5) address = "0" + address;

                                    }

                                    string hexaOpernadNum = address;
                                    int tempNum = toDecimal(menomonicCode) + 1;
                                    if(operand[0] == '@'){
                                        tempNum++;
                                    }
                                    address = "";
                                    base_conv(tempNum , 16);
                                    while(address.size() != 2) address = "0" + address;
                                    menomonicCode = address;
                                    object_code[i] = menomonicCode + "1" + hexaOpernadNum;
                                            cout << object_code[i] << "\n";

                                    continue;

                                }else{
                                    cout <<  "error label name " << operand_label;
                                    continue;
                                }

                            }
                            if(operand[0] == '#'){
                                int tempNum = toDecimal(menomonicCode) + 1;
                                address = "";
                                base_conv(tempNum , 16);
                                while(address.size() != 2) address = "0" + address;
                                menomonicCode = address;
                                object_code[i] = menomonicCode + "1" + operand_address;
                                        cout << object_code[i] << "\n";

                                continue;
                            }
                            else{
                                int tempNum = toDecimal(menomonicCode) + 2;
                                address = "";
                                base_conv(tempNum , 16);
                                while(address.size() != 2) address = "0" + address;
                                menomonicCode = address;
                                object_code[i] = menomonicCode + "9" + operand_address;
                                        cout << object_code[i] << "\n";

                                continue;
                            }


                        }
                        else{
                            cout <<  "error format 4 lable does not exist ";
                            continue;

                        }

                    }
                }
            }
        }
    }

    for(int i = 0 ; i < hh ; i++){
        cout << object_code[i] << "\n";

    }

    for(int i = 0 ; i < hh ; i++){
            cout <<"i = " << i <<  " = " << src[i][0] << "\n";
    }
    int len = sizeof(object_code) / sizeof(string);
    for(int i = 0 ; i < len ; i++){
        obCode.push_back(object_code[i]);

    }
    mod_size = modification_record.size();
    for(int i = 0 ; i < mod_size ; i++){
        vec.push_back(modification_record[i]);
    }

}

bool GenerateObjectCode::isHex(string number)
{
    int len = number.size();
    for(int i = 0; i < len; i++)
    {
        if(!((number[i] >= '0' && number[i] <= '9') || (number[i] >= 'A' && number[i] <= 'F')))
            return false;
    }
    return true;
}

void GenerateObjectCode::split(string data[], string temp, char t)
{
    int len = temp.size();
    bool found = false;
    string s1 = "";
    string s2 = "";

    for(int i = 0 ; i < len ; i++){
        if(found){
            s2 += temp[i];
            continue;
        }
        if(temp[i] == t){
            found = true;
            continue;
        }
        s1 += temp[i];
    }

    data[0] = s1;
    data[1] = s2;
}

bool GenerateObjectCode::is_exist(string temp, char t){
    int len = temp.size();
    for(int i = 0 ; i < len ; i++){
        if(temp[i] == t){
            return true;
        }
    }
    return false;
}

bool GenerateObjectCode::is_relative_address(string target_address, string pc, bool base_used, string base_address, int i, string object_code[], string menomonicCode, int add_m, string add_pc, string add_base)
{
    cout << "TA = " <<  target_address << "\n";
    cout << "PC = " << pc << "\n";

    int diff = toDecimal(target_address) - toDecimal(pc);
    if(diff >= -2048 && diff <= 2047){
        if(diff < 0){
            diff = diff * -1;
            diff--;
            diff = 4095 - diff;
        }
        address = "";
        base_conv(diff,16);
        while(address.size() != 3) address = "0" + address;
        string diff_hex = address;
        int tempNum = toDecimal(menomonicCode) + add_m;
        address = "";
        base_conv(tempNum , 16);
        while(address.size() != 2) address = "0" + address;
        menomonicCode = address;
        object_code[i] = menomonicCode + add_pc + diff_hex;
                cout << object_code[i] << "\n";

        return true;
    }
    else if(base_used){

        diff = toDecimal(target_address) - toDecimal(base_address);
        if(diff >= 0 && diff <= 4095){
            address = "";
            base_conv(diff,16);
            while(address.size() != 3) address = "0" + address;
            string diff_hex = address;
            int tempNum = toDecimal(menomonicCode) + add_m;
            address = "";
            base_conv(tempNum , 16);
            while(address.size() != 2) address = "0" + address;
            menomonicCode = address;
            object_code[i] = menomonicCode + add_base + diff_hex;
                    cout << object_code[i] << "\n";

            return true;
        }
    }
    return false;

}
void GenerateObjectCode::base_conv(int n, int b)
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

int GenerateObjectCode::getRegistersNum(string menomonic){

    if(menomonic.compare("CLEAR")== 0 || menomonic.compare("TIXR") == 0)
        return 1;

    else return 2;
}


int GenerateObjectCode::toDecimal(string str)
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

void GenerateObjectCode::print_output_file(Parser p, string src[][5], int length){
    std::ofstream outfile ("test.txt");
    string objecCode = "H";
    string g = "";
    string prog_name = "";
    string prog_address = "0000";
    for(int i = 0 ; i < length ; i++){
        if(src[i][2] == "START"){
            prog_name = src[i][1];
            prog_address = src[i][3];
            break;
        }
    }
    if(prog_name == ""){
        prog_name = "DEFULT";
    }
    if(prog_address.length() > 5){
        prog_address = "000000";
    }
    objecCode = objecCode + prog_name;
    int l = prog_name.length();
    for(int i = 0 ; i < 6 - l ; i++){
        objecCode = objecCode + " ";
    }
    int len = prog_address.length();
    for(int i = 0 ; i < 6 - len ; i++){
        objecCode = objecCode + "0";
    }
    objecCode = objecCode + prog_address;
    g = objecCode;
    address = "";
    base_conv(toDecimal(src[p.getN()-1][0]) - toDecimal(prog_address), 16);
    len = address.length();
    for(int i = 0 ; i < 6-len ; i++){
        objecCode = objecCode + "0";
    }
    objecCode = objecCode + address;
    outfile<<objecCode<<"\n";
    for (int i = 0; i < length ; ++i) {
        objecCode = "T";
        int len = src[i][0].length();
        for(int j = 0 ; j <6 - len ; j++){
            objecCode = objecCode + "0";
        }
        objecCode  = objecCode + src[i][0];
        objecCode = objecCode + "  ";

        while(i< length/*j = 0 ; j < 59&&(i < length) ; j = j + obCode[i].length()*/){
            if(obCode[i] == "" || obCode[i] == "-1"){
                i++;
                if(i >= length){
                    break;
                }
                continue;
            }
            cout<<i<<"-------------- \n";
            objecCode = objecCode + obCode[i];
            int lenn = objecCode.length() + obCode[i+1].length();
            if(lenn > 69 || !(i < length)){
                break;
            }
            bool v = false;
            while(src[i+1][2] == "RESW" || src[i+1][2] == "RESB"){
                i++;
                v = true;
            }
            if(v){
                break;
            }
            i++;
            if(i >= length){
                break;
            }
        }
        address = "";
        base_conv((objecCode.length()-9)/2, 16);
        objecCode = regex_replace(objecCode, std::regex("  "), address);
        outfile<<objecCode<<"\n";
    }
    for(int k = 0 ; k < mod_size ; k++){
        outfile<<vec[k]<<"\n";
    }
    string h = "";
    int o = 6 - prog_name.length();
    for(int i = 0 ; i < o ; i++){
        h = h + " ";
    }
    g = regex_replace(g, std::regex("H"), "E");
    g = regex_replace(g, std::regex(prog_name + h), "");
    objecCode = g;
    outfile<<objecCode<<"\n";
}
int GenerateObjectCode::strToDev(string str)
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

void GenerateObjectCode::multi_passes(string src [][5], int len, muTable mu_table){
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
                add(temp[j][0],temp[j][2]);
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
                        add(temp[k][0],temp[k][2]);
                    }
                    else{
                        EquSolver s = EquSolver();
                        int z = s.start(exp);
                        address = "";
                        base_conv(z,10);
                        temp[k][2] = address;
                        temp[k][3] = "1";
                        add(temp[k][0],address);
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

bool GenerateObjectCode::fi(string m , char c){
    for(int i = 0 ; i < m.length() ; i++){
        if(m[i] == c){
            return true;
        }
    }
    return false;
}

string GenerateObjectCode::change(string exp, muTable my_table){
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
        string operand = get(temp_s);
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
bool GenerateObjectCode::isNum(string word)
{
    bool isNumber = true;
    if(word == "") return false;
    for(string::const_iterator k = word.begin(); k != word.end(); ++k)
        isNumber &= isdigit(*k);
    return isNumber;
}
