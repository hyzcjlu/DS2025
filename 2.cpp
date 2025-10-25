#include <iostream>
#include <cstring>
#include <cmath>
#include <cctype>
using namespace std;

#ifdef _WIN32
#include <windows.h>
#endif

// 运算符枚举
enum Optr {
    ADD, SUB, MUL, DIV, POW, FACT, L_P, R_P, EOE, 
    SIN, COS, TAN, LOG, LN
};

// 栈模板类
template <typename T>
class Stack {
private:
    T data[100];
    int top;
public:
    Stack() : top(-1) {}
    bool isEmpty() const { return top == -1; }
    bool isFull() const { return top == 99; }
    void push(T elem) { if (!isFull()) data[++top] = elem; }
    T pop() { return (top >= 0) ? data[top--] : T(); }
    T getTop() const { return (top >= 0) ? data[top] : T(); }
};

// 运算符优先级表
const char pri[14][14] = {
    //       +     -     *     /     ^     !     (     )    \0   SIN   COS   TAN   LOG   LN
    /* + */  '>',  '>',  '<',  '<',  '<',  '<',  '<',  '>',  '>',  '<',  '<',  '<',  '<',  '<',
    /* - */  '>',  '>',  '<',  '<',  '<',  '<',  '<',  '>',  '>',  '<',  '<',  '<',  '<',  '<', 
    /* * */  '>',  '>',  '>',  '>',  '<',  '<',  '<',  '>',  '>',  '<',  '<',  '<',  '<',  '<',
    /* / */  '>',  '>',  '>',  '>',  '<',  '<',  '<',  '>',  '>',  '<',  '<',  '<',  '<',  '<',
    /* ^ */  '>',  '>',  '>',  '>',  '>',  '<',  '<',  '>',  '>',  '<',  '<',  '<',  '<',  '<',
    /* ! */  '>',  '>',  '>',  '>',  '>',  '>',  ' ',  '>',  '>',  '<',  '<',  '<',  '<',  '<',
    /* ( */  '<',  '<',  '<',  '<',  '<',  '<',  '<',  '=',  ' ',  '<',  '<',  '<',  '<',  '<',
    /* ) */  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',  ' ',
    /* \0*/  '<',  '<',  '<',  '<',  '<',  '<',  '<',  ' ',  '=',  '<',  '<',  '<',  '<',  '<',
    /* SIN*/ '<',  '<',  '<',  '<',  '<',  '<',  '<',  '>',  '>',  '<',  '<',  '<',  '<',  '<',
    /* COS*/ '<',  '<',  '<',  '<',  '<',  '<',  '<',  '>',  '>',  '<',  '<',  '<',  '<',  '<',
    /* TAN*/ '<',  '<',  '<',  '<',  '<',  '<',  '<',  '>',  '>',  '<',  '<',  '<',  '<',  '<',
    /* LOG*/ '<',  '<',  '<',  '<',  '<',  '<',  '<',  '>',  '>',  '<',  '<',  '<',  '<',  '<',
    /* LN */ '<',  '<',  '<',  '<',  '<',  '<',  '<',  '>',  '>',  '<',  '<',  '<',  '<',  '<'
};

// 字符转运算符
Optr charToOptr(const char* expr, int& i) {
    // 跳过空格
    while (expr[i] == ' ') i++;
    
    if (expr[i] == '\0') return EOE;

    // 单字符运算符
    switch (expr[i]) {
        case '+': i++; return ADD;
        case '-': i++; return SUB;
        case '*': i++; return MUL;
        case '/': i++; return DIV;
        case '^': i++; return POW;
        case '!': i++; return FACT;
        case '(': i++; return L_P;
        case ')': i++; return R_P;
    }

    // 多字符函数
    if (strncmp(&expr[i], "sin", 3) == 0 || strncmp(&expr[i], "SIN", 3) == 0) {
        i += 3;
        return SIN;
    }
    if (strncmp(&expr[i], "cos", 3) == 0 || strncmp(&expr[i], "COS", 3) == 0) {
        i += 3;
        return COS;
    }
    if (strncmp(&expr[i], "tan", 3) == 0 || strncmp(&expr[i], "TAN", 3) == 0) {
        i += 3;
        return TAN;
    }
    if (strncmp(&expr[i], "log", 3) == 0 || strncmp(&expr[i], "LOG", 3) == 0) {
        i += 3;
        return LOG;
    }
    if (strncmp(&expr[i], "ln", 2) == 0 || strncmp(&expr[i], "LN", 2) == 0) {
        i += 2;
        return LN;
    }

    return EOE;
}

// 计算函数
double calculate(Optr op, double a, double b = 0) {
    switch (op) {
        case ADD: return a + b;
        case SUB: return a - b;
        case MUL: return a * b;
        case DIV: 
            if (b == 0) {
                cerr << "Error: Division by zero!" << endl;
                return 0;
            }
            return a / b;
        case POW: return pow(a, b);
        case FACT: {
            int n = static_cast<int>(a);
            if (n < 0 || a != n) {
                cerr << "Error: Factorial requires non-negative integer!" << endl;
                return 0;
            }
            double res = 1;
            for (int i = 1; i <= n; ++i) res *= i;
            return res;
        }
        case SIN: return sin(a);
        case COS: return cos(a);
        case TAN: return tan(a);
        case LOG: 
            if (a <= 0) {
                cerr << "Error: log argument must be positive!" << endl;
                return 0;
            }
            return log10(a);
        case LN:  
            if (a <= 0) {
                cerr << "Error: ln argument must be positive!" << endl;
                return 0;
            }
            return log(a);
        default: return 0;
    }
}

// 解析并计算表达式
double evaluate(const char* expr) {
    Stack<double> numStack;
    Stack<Optr> optrStack;
    optrStack.push(EOE);
    
    int i = 0;
    int len = strlen(expr);
    bool expectOperand = true;

    while (i < len || !optrStack.isEmpty()) {
        // 跳过空格
        while (i < len && expr[i] == ' ') i++;
        
        if (i >= len) {
            // 表达式结束，处理剩余运算符
            while (!optrStack.isEmpty() && optrStack.getTop() != EOE) {
                Optr op = optrStack.pop();
                if (op == SIN || op == COS || op == TAN || op == LOG || op == LN) {
                    if (numStack.isEmpty()) {
                        cerr << "Error: Missing operand for function!" << endl;
                        return 0;
                    }
                    double a = numStack.pop();
                    numStack.push(calculate(op, a));
                } else if (op == FACT) {
                    if (numStack.isEmpty()) {
                        cerr << "Error: Missing operand for factorial!" << endl;
                        return 0;
                    }
                    double a = numStack.pop();
                    numStack.push(calculate(op, a));
                } else {
                    if (numStack.isEmpty()) {
                        cerr << "Error: Missing operands!" << endl;
                        return 0;
                    }
                    double b = numStack.pop();
                    if (numStack.isEmpty()) {
                        cerr << "Error: Missing operands!" << endl;
                        return 0;
                    }
                    double a = numStack.pop();
                    numStack.push(calculate(op, a, b));
                }
            }
            break;
        }

        char c = expr[i];
        
        // 解析数字
        if (isdigit(c) || c == '.') {
            double num = 0.0;
            bool hasDecimal = false;
            double decimalFactor = 0.1;

            // 整数部分
            while (i < len && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }

            // 小数部分
            if (i < len && expr[i] == '.') {
                hasDecimal = true;
                i++;
                while (i < len && isdigit(expr[i])) {
                    num += (expr[i] - '0') * decimalFactor;
                    decimalFactor *= 0.1;
                    i++;
                }
            }

            numStack.push(num);
            expectOperand = false;
        }
        // 处理运算符和函数
        else {
            int old_i = i;
            Optr currOp = charToOptr(expr, i);
            
            if (currOp == EOE && old_i == i) {
                // 无法识别的字符
                cerr << "Error: Unrecognized character '" << expr[i] << "'" << endl;
                i++;
                continue;
            }

            // 处理负号（作为一元运算符）
            if (currOp == SUB && expectOperand) {
                // 处理一元负号：压入0，然后进行减法
                numStack.push(0);
                optrStack.push(SUB);
                expectOperand = true;
                continue;
            }

            if (currOp == R_P) {
                // 处理右括号：计算直到遇到左括号
                while (!optrStack.isEmpty() && optrStack.getTop() != L_P) {
                    Optr op = optrStack.pop();
                    if (op == SIN || op == COS || op == TAN || op == LOG || op == LN) {
                        if (numStack.isEmpty()) {
                            cerr << "Error: Missing operand for function!" << endl;
                            return 0;
                        }
                        double a = numStack.pop();
                        numStack.push(calculate(op, a));
                    } else if (op == FACT) {
                        if (numStack.isEmpty()) {
                            cerr << "Error: Missing operand for factorial!" << endl;
                            return 0;
                        }
                        double a = numStack.pop();
                        numStack.push(calculate(op, a));
                    } else {
                        if (numStack.isEmpty()) {
                            cerr << "Error: Missing operands!" << endl;
                            return 0;
                        }
                        double b = numStack.pop();
                        if (numStack.isEmpty()) {
                            cerr << "Error: Missing operands!" << endl;
                            return 0;
                        }
                        double a = numStack.pop();
                        numStack.push(calculate(op, a, b));
                    }
                }
                if (optrStack.isEmpty() || optrStack.getTop() != L_P) {
                    cerr << "Error: Mismatched parentheses!" << endl;
                    return 0;
                }
                optrStack.pop(); // 弹出左括号
                expectOperand = false;
                continue;
            }

            Optr topOp = optrStack.getTop();
            
            // 检查优先级表
            if (topOp < 0 || topOp > 13 || currOp < 0 || currOp > 13) {
                cerr << "Error: Invalid operator!" << endl;
                return 0;
            }
            
            char priority = pri[topOp][currOp];

            if (priority == '<') {
                optrStack.push(currOp);
                expectOperand = true;
            }
            else if (priority == '=') {
                optrStack.pop();
                expectOperand = false;
            }
            else if (priority == '>') {
                optrStack.pop();
                
                if (topOp == SIN || topOp == COS || topOp == TAN || topOp == LOG || topOp == LN) {
                    if (numStack.isEmpty()) {
                        cerr << "Error: Missing operand for function!" << endl;
                        return 0;
                    }
                    double a = numStack.pop();
                    numStack.push(calculate(topOp, a));
                }
                else if (topOp == FACT) {
                    if (numStack.isEmpty()) {
                        cerr << "Error: Missing operand for factorial!" << endl;
                        return 0;
                    }
                    double a = numStack.pop();
                    numStack.push(calculate(topOp, a));
                }
                else {
                    if (numStack.isEmpty()) {
                        cerr << "Error: Missing operands!" << endl;
                        return 0;
                    }
                    double b = numStack.pop();
                    if (numStack.isEmpty()) {
                        cerr << "Error: Missing operands!" << endl;
                        return 0;
                    }
                    double a = numStack.pop();
                    numStack.push(calculate(topOp, a, b));
                }
                expectOperand = false;
                
                // 继续比较当前运算符与新的栈顶运算符
                continue;
            }
            else {
                cerr << "Error: Invalid expression syntax!" << endl;
                return 0;
            }
        }
    }

    // 检查最终结果
    if (numStack.isEmpty()) {
        cerr << "Error: Empty expression!" << endl;
        return 0;
    }
    
    double result = numStack.pop();
    if (!numStack.isEmpty()) {
        cerr << "Error: Incomplete expression!" << endl;
        return 0;
    }
    
    return result;
}

int main() {
    // 解决中文乱码 - 使用UTF-8编码
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    #endif

    cout << "Expression Calculator Test:" << endl;
    cout << "===========================" << endl;

    // 简单测试用例
    const char* tests[] = {
        "3+4*2",           // 11
        "(2+3)^2",         // 25  
        "sin(3.1416/2)",   // ~1
        "log(100)",        // 2
        "5!",              // 120
        "3.5 * 2 + 1.5",   // 8.5
        "10 / 2 - 1",      // 4
        "2 + 3 * 4",       // 14
        "(2 + 3) * 4"      // 20
    };
    
    double expected[] = {11, 25, 1, 2, 120, 8.5, 4, 14, 20};
    
    for (int i = 0; i < 9; i++) {
        double result = evaluate(tests[i]);
        cout << tests[i] << " = " << result;
        if (fabs(result - expected[i]) < 0.0001) {
            cout << " ?" << endl;
        } else {
            cout << " ? (expected: " << expected[i] << ")" << endl;
        }
    }

    // 复杂测试用例
    cout << "\nAdvanced Tests:" << endl;
    cout << "===============" << endl;
    
    const char* advTests[] = {
        "2*sin(0.5236) + ln(2.71828)",  // ~2 (2*0.5 + 1)
        "cos(0) + log(10)",             // 2 (1 + 1)
        "3^2 + 4^2"                     // 25
    };
    
    for (int i = 0; i < 3; i++) {
        double result = evaluate(advTests[i]);
        cout << advTests[i] << " = " << result << endl;
    }

    #ifdef _WIN32
    system("pause");
    #endif
    return 0;
}
