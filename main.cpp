#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <map>

// Verifica se um caractere é operador
bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

// Retorna a precedência do operador
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

// Verifica se um caractere é uma função
bool isFunction(const std::string& token) {
    return token == "EXP" || token == "SQR" || token == "SIN" || token == "COS";
}

// Analisa a expressão e divide em tokens
std::vector<std::string> tokenize(const std::string& infix) {
    std::vector<std::string> tokens;
    std::string token;
    for (size_t i = 0; i < infix.size(); ++i) {
        if (std::isspace(infix[i])) {
            continue;
        }
        if (std::isdigit(infix[i]) || infix[i] == '.') {
            token += infix[i];
            while (i + 1 < infix.size() && (std::isdigit(infix[i + 1]) || infix[i + 1] == '.')) {
                token += infix[++i];
            }
            tokens.push_back(token);
            token.clear();
        } else if (std::isalpha(infix[i])) {
            token += infix[i];
            while (i + 1 < infix.size() && std::isalpha(infix[i + 1])) {
                token += infix[++i];
            }
            tokens.push_back(token);
            token.clear();
        } else if (isOperator(infix[i])) {
            if (infix[i] == '-' && (i == 0 || isOperator(infix[i - 1]) || infix[i - 1] == '(')) {
                // Trata o operador unário
                token += '-';
                if (i + 1 < infix.size() && (std::isdigit(infix[i + 1]) || infix[i + 1] == '.')) {
                    token += infix[++i];
                    while (i + 1 < infix.size() && (std::isdigit(infix[i + 1]) || infix[i + 1] == '.')) {
                        token += infix[++i];
                    }
                }
                tokens.push_back(token);
                token.clear();
            } else {
                tokens.push_back(std::string(1, infix[i]));
            }
        } else if (infix[i] == '(' || infix[i] == ')') {
            tokens.push_back(std::string(1, infix[i]));
        }
    }
    return tokens;
}

// Converte uma expressão infixa para posfixa usando o algoritmo de Shunting Yard
std::string infixToPostfix(const std::string& infix) {
    std::stack<std::string> operators;
    std::stringstream output;
    auto tokens = tokenize(infix);

    for (const auto& token : tokens) {
        if (std::isdigit(token[0]) || (token.size() > 1 && std::isdigit(token[1]))) {
            // Token é um operando (número)
            output << token << ' ';
        } else if (isFunction(token)) {
            // Token é uma função
            operators.push(token);
        } else if (token == "(") {
            // Token é um parêntese de abertura
            operators.push(token);
        } else if (token == ")") {
            // Token é um parêntese de fechamento
            while (!operators.empty() && operators.top() != "(") {
                output << operators.top() << ' ';
                operators.pop();
            }
            operators.pop(); // Remove o '('
            if (!operators.empty() && isFunction(operators.top())) {
                output << operators.top() << ' ';
                operators.pop();
            }
        } else if (isOperator(token[0])) {
            // Token é um operador
            while (!operators.empty() && precedence(operators.top()[0]) >= precedence(token[0])) {
                output << operators.top() << ' ';
                operators.pop();
            }
            operators.push(token);
        }
    }

    // Esvazia a pilha de operadores
    while (!operators.empty()) {
        output << operators.top() << ' ';
        operators.pop();
    }

    return output.str();
}

// Função principal para teste
int main() {
    std::string infix1 = "3+4*2/(1-5)^2^3";
    std::string postfix1 = infixToPostfix(infix1);
    std::cout << "Infix: " << infix1 << std::endl;
    std::cout << "Postfix: " << postfix1 << std::endl;

    std::string infix2 = "SIN(3+4)*COS(2-1)";
    std::string postfix2 = infixToPostfix(infix2);
    std::cout << "Infix: " << infix2 << std::endl;
    std::cout << "Postfix: " << postfix2 << std::endl;

    std::string infix3 = "-3+4*-2/(1--5)^2^3";
    std::string postfix3 = infixToPostfix(infix3);
    std::cout << "Infix: " << infix3 << std::endl;
    std::cout << "Postfix: " << postfix3 << std::endl;

    return 0;
}
