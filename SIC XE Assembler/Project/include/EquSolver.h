#ifndef EQUSOLVER_H
#define EQUSOLVER_H
#include<iostream>
#include<stack>
#include <string>
#include <stdexcept>
#include <cctype>
#include <sstream>
#include <stack>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

class EquSolver
{
    public:
        // Function to evaluate Postfix expression and return output
        int EvaluatePostfix(string expression);

        // Function to perform an operation and return output.
        int PerformOperation(char operation, int operand1, int operand2);

        // Function to verify whether a character is operator symbol or not.
        bool isOperator(char currentChar);

        // Function to verify whether a character is numeric digit.
        bool IsNumericDigit(char C);
        int cal(string expression);
        int EvaluatePfix(string expression);
        bool precedence(char leftOperator, char rightOperator);
        std::string convertToPostfix(const std::string& infix);
        int start(string infix);

};

#endif // EQUSOLVER_H

