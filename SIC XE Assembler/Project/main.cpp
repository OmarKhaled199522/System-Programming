#include <iostream>
#include <Engine.h>
#include <string>
#include<cstdlib>
#include<cstring>
#include<GenerateObjectCode.h>
#include <list>
using namespace std;

int main(int argc, char* const argv[])
{

    string fileName = "mySrc.txt";
    if(argv[1]!=NULL)
        fileName = argv[1];
    Engine e = Engine(fileName);

    GenerateObjectCode ll = GenerateObjectCode();
    ll.start(e.p.src, e.symtable, e.p.literalTable, e.p.getN());
    ll.print_output_file(e.p, e.p.src, e.p.getN());
    return 0;
}
