#include <iostream>
#include <stack>
#include <sstream>
#include <string>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <vector>

// Функция для определения приоритета оператора
int precedence(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    if (op == '^')
        return 3;
    return 0;
}

// Функция для выполнения арифметических операций
double applyOp(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0)
                throw std::runtime_error("Division by zero");
            return a / b;
        case '^': return std::pow(a, b);
    }
    return 0;
}

// Функция для проверки, является ли строка числом
bool isNumber(const std::string& s) {
    std::istringstream iss(s);
    double d;
    return (iss >> d) && (iss.eof());
}

// Функция для выделения чисел и операторов из строки
std::vector<std::string> tokenize(const std::string& expression) {
    std::vector<std::string> tokens;
    std::string num;
    bool lastCharWasOperator = true;

    for (char ch : expression) {
        if (std::isdigit(ch) || ch == '.') {
            num += ch;
            lastCharWasOperator = false;
        } else {
            if (!num.empty()) {
                tokens.push_back(num);
                num.clear();
            }
            if (ch == '-' && lastCharWasOperator) {
                num += ch;
            } else {
                tokens.push_back(std::string(1, ch));
                lastCharWasOperator = (ch != ')');
            }
        }
    }

    if (!num.empty()) {
        tokens.push_back(num);
    }

    return tokens;
}

// Функция для проверки правильности выражения
bool validateExpression(const std::vector<std::string>& tokens) {
    int openBrackets = 0;
    bool lastTokenWasOperator = true;
    bool lastTokenWasOpeningBracket = false;

    for (const std::string& token : tokens) {
        if (token == "(") {
            openBrackets++;
            lastTokenWasOperator = true;
            lastTokenWasOpeningBracket = true;
        } else if (token == ")") {
            openBrackets--;
            if (openBrackets < 0 || lastTokenWasOperator) {
                return false;  // Неправильное расположение скобок
            }
            lastTokenWasOperator = false;
            lastTokenWasOpeningBracket = false;
        } else if (isNumber(token)) {
            if (!lastTokenWasOperator && !lastTokenWasOpeningBracket) {
                return false;  // Два числа подряд
            }
            lastTokenWasOperator = false;
            lastTokenWasOpeningBracket = false;
        } else {
            if (lastTokenWasOperator) {
                return false;  // Два оператора подряд
            }
            lastTokenWasOperator = true;
            lastTokenWasOpeningBracket = false;
        }
    }

    if (openBrackets != 0 || lastTokenWasOperator) {
        return false;  // Несоответствие открывающих и закрывающих скобок или выражение заканчивается оператором
    }

    return true;
}

// Функция для преобразования инфиксной записи в постфиксную (ОПЗ)
std::string infixToPostfix(const std::string& expression) {
    std::vector<std::string> tokens = tokenize(expression);
    if (!validateExpression(tokens)) {
        throw std::runtime_error("Invalid expression");
    }

    std::stack<char> operators;
    std::string postfix;

    for (const std::string& token : tokens) {
        if (isNumber(token)) {
            postfix += token + " ";
        } else if (token == "(") {
            operators.push('(');
        } else if (token == ")") {
            while (!operators.empty() && operators.top() != '(') {
                postfix += operators.top();
                postfix += " ";
                operators.pop();
            }
            operators.pop(); // Убираем '(' из стека
        } else {
            while (!operators.empty() && precedence(operators.top()) >= precedence(token[0])) {
                postfix += operators.top();
                postfix += " ";
                operators.pop();
            }
            operators.push(token[0]);
        }
    }

    while (!operators.empty()) {
        postfix += operators.top();
        postfix += " ";
        operators.pop();
    }

    return postfix;
}

// Функция для вычисления значения выражения в постфиксной записи
double evaluatePostfix(const std::string& expression) {
    std::stack<double> values;
    std::istringstream tokens(expression);
    std::string token;

    while (tokens >> token) {
        if (isNumber(token)) {
            values.push(std::stod(token));
        } else {
            double val2 = values.top(); values.pop();
            double val1 = values.top(); values.pop();
            double result = applyOp(val1, val2, token[0]);
            values.push(result);
        }
    }

    return values.top();
}

// Главная функция
int main() {
    std::string expression;
    std::cout << "Input expression: ";
    std::getline(std::cin, expression);

    try {
        std::string postfix = infixToPostfix(expression);
        std::cout << "Postfix: " << postfix << std::endl;
        double result = evaluatePostfix(postfix);
        std::cout << "Result: " << result << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
