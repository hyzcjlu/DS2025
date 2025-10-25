#include <iostream>
#include <cstring>
#include <cmath>
#include <cctype>
#include <stack>
#include <string>
#include <map>
#include <stdexcept>
#include <cstdlib>
using namespace std;

class ExpressionCalculator {
private:
    enum Operator { ADD, SUB, MUL, DIV, POW, FACT, L_PAREN, R_PAREN, END, SIN, COS, TAN, LOG, LN };
    
    map<Operator, int> precedence;
    
    void initPrecedence() {
        precedence[SIN] = precedence[COS] = precedence[TAN] = precedence[LOG] = precedence[LN] = 5;
        precedence[POW] = 4;
        precedence[FACT] = 3;
        precedence[MUL] = precedence[DIV] = 2;
        precedence[ADD] = precedence[SUB] = 1;
        precedence[L_PAREN] = precedence[R_PAREN] = precedence[END] = 0;
    }
    
    void skipSpaces(const string& expr, int& index) {
        while (index < expr.length() && expr[index] == ' ') index++;
    }
    
    double readNumber(const string& expr, int& index) {
        double number = 0.0;
        bool hasDecimal = false;
        double decimalFactor = 0.1;
        
        while (index < expr.length() && isdigit(expr[index])) {
            number = number * 10 + (expr[index] - '0');
            index++;
        }
        
        if (index < expr.length() && expr[index] == '.') {
            index++;
            while (index < expr.length() && isdigit(expr[index])) {
                number += (expr[index] - '0') * decimalFactor;
                decimalFactor *= 0.1;
                index++;
            }
        }
        
        return number;
    }
    
    Operator readOperator(const string& expr, int& index) {
        skipSpaces(expr, index);
        if (index >= expr.length()) return END;
        
        char c = expr[index];
        switch (c) {
            case '+': index++; return ADD;
            case '-': index++; return SUB;
            case '*': index++; return MUL;
            case '/': index++; return DIV;
            case '^': index++; return POW;
            case '!': index++; return FACT;
            case '(': index++; return L_PAREN;
            case ')': index++; return R_PAREN;
        }
        
        if (index + 2 < expr.length()) {
            string substr = expr.substr(index, 3);
            if (substr == "sin" || substr == "SIN") { index += 3; return SIN; }
            if (substr == "cos" || substr == "COS") { index += 3; return COS; }
            if (substr == "tan" || substr == "TAN") { index += 3; return TAN; }
            if (substr == "log" || substr == "LOG") { index += 3; return LOG; }
        }
        
        if (index + 1 < expr.length()) {
            string substr = expr.substr(index, 2);
            if (substr == "ln" || substr == "LN") { index += 2; return LN; }
        }
        
        return END;
    }
    
    double calculate(Operator op, double a, double b = 0) {
        switch (op) {
            case ADD: return a + b;
            case SUB: return a - b;
            case MUL: return a * b;
            case DIV: 
                if (b == 0) throw runtime_error("Division by zero");
                return a / b;
            case POW: return pow(a, b);
            case FACT: {
                int n = static_cast<int>(a);
                if (n < 0 || a != n) throw runtime_error("Factorial requires non-negative integer");
                double result = 1;
                for (int i = 2; i <= n; i++) result *= i;
                return result;
            }
            case SIN: return sin(a);
            case COS: return cos(a);
            case TAN: return tan(a);
            case LOG: 
                if (a <= 0) throw runtime_error("Logarithm requires positive argument");
                return log10(a);
            case LN: 
                if (a <= 0) throw runtime_error("Natural logarithm requires positive argument");
                return log(a);
            default: return 0;
        }
    }
    
    void processOperator(stack<double>& numbers, stack<Operator>& operators) {
        Operator op = operators.top();
        operators.pop();
        
        if (op == SIN || op == COS || op == TAN || op == LOG || op == LN) {
            if (numbers.empty()) throw runtime_error("Missing operand for function");
            double a = numbers.top(); numbers.pop();
            numbers.push(calculate(op, a));
        } else if (op == FACT) {
            if (numbers.empty()) throw runtime_error("Missing operand for factorial");
            double a = numbers.top(); numbers.pop();
            numbers.push(calculate(op, a));
        } else {
            if (numbers.size() < 2) throw runtime_error("Missing operands");
            double b = numbers.top(); numbers.pop();
            double a = numbers.top(); numbers.pop();
            numbers.push(calculate(op, a, b));
        }
    }

public:
    ExpressionCalculator() { initPrecedence(); }
    
    double evaluate(const string& expression) {
        stack<double> numbers;
        stack<Operator> operators;
        int index = 0;
        bool expectOperand = true;
        
        while (index <= expression.length()) {
            skipSpaces(expression, index);
            
            if (index >= expression.length()) {
                while (!operators.empty()) processOperator(numbers, operators);
                break;
            }
            
            char c = expression[index];
            
            if (isdigit(c) || c == '.') {
                if (!expectOperand) throw runtime_error("Unexpected number");
                numbers.push(readNumber(expression, index));
                expectOperand = false;
            } else {
                Operator op = readOperator(expression, index);
                
                if (op == END) throw runtime_error("Unrecognized character");
                
                if (expectOperand) {
                    if (op == SUB) {
                        numbers.push(0);
                        operators.push(SUB);
                    } else if (op == SIN || op == COS || op == TAN || op == LOG || op == LN) {
                        operators.push(op);
                    } else if (op == L_PAREN) {
                        operators.push(op);
                    } else {
                        throw runtime_error("Unexpected operator");
                    }
                } else {
                    if (op == FACT) {
                        operators.push(op);
                        processOperator(numbers, operators);
                    } else if (op == R_PAREN) {
                        while (!operators.empty() && operators.top() != L_PAREN) {
                            processOperator(numbers, operators);
                        }
                        if (operators.empty()) throw runtime_error("Mismatched parentheses");
                        operators.pop();
                        
                        if (!operators.empty() && 
                            (operators.top() == SIN || operators.top() == COS || 
                             operators.top() == TAN || operators.top() == LOG || operators.top() == LN)) {
                            processOperator(numbers, operators);
                        }
                    } else {
                        while (!operators.empty() && 
                               precedence[operators.top()] >= precedence[op] &&
                               operators.top() != L_PAREN) {
                            processOperator(numbers, operators);
                        }
                        operators.push(op);
                        expectOperand = true;
                    }
                }
            }
        }
        
        if (numbers.size() != 1) throw runtime_error("Invalid expression");
        return numbers.top();
    }
};

int main() {
    #ifdef _WIN32
    system("chcp 65001 > nul");
    #endif
    
    ExpressionCalculator calc;
    string input;
    
    cout << "Expression Calculator" << endl;
    cout << "=====================" << endl;
    cout << "Supported operations: + - * / ^ ! ( )" << endl;
    cout << "Supported functions: sin cos tan log ln" << endl;
    cout << "Examples: 3+4*2, (2+3)^2, sin(3.1416/2), 5!" << endl;
    cout << "Type 'quit' to exit" << endl << endl;
    
    while (true) {
        cout << "Enter expression: ";
        getline(cin, input);
        
        if (input == "quit" || input == "exit") break;
        if (input.empty()) continue;
        
        try {
            double result = calc.evaluate(input);
            cout << "Result: " << result << endl;
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
        
        cout << endl;
    }
    
    return 0;
}
