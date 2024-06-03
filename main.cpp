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

// Converte uma expressão infixa para posfixa usando o algoritmo de Shunting Yard
std::string infixToPostfix(const std::string& infix) {
    std::stack<std::string> operators;
    std::stringstream output;
    std::istringstream tokens(infix);
    std::string token;

    while (tokens >> token) {
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
    std::string infix = "3 + 4 * 2 / ( 1 - 5 ) ^ 2 ^ 3";
    std::string postfix = infixToPostfix(infix);
    std::cout << "Infix: " << infix << std::endl;
    std::cout << "Postfix: " << postfix << std::endl;

    infix = "SIN ( 3 + 4 ) * COS ( 2 - 1 )";
    postfix = infixToPostfix(infix);
    std::cout << "Infix: " << infix << std::endl;
    std::cout << "Postfix: " << postfix << std::endl;

    return 0;
}
