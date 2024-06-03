#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <map>
#include <algorithm>

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

// Verifica se uma string é uma função válida
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

// Verifica se uma expressão infixa é válida
std::string validateExpression(const std::vector<std::string>& tokens) {
    int parenthesesBalance = 0;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        if (token == "(") {
            parenthesesBalance++;
        } else if (token == ")") {
            parenthesesBalance--;
            if (parenthesesBalance < 0) {
                return "Mismatched parentheses";
            }
        } else if (isOperator(token[0]) && token.size() == 1) {
            if (i == 0 || i == tokens.size() - 1) {
                return "Invalid expression"; // Operador no início ou no fim
            }
            if (isOperator(tokens[i + 1][0]) && tokens[i + 1].size() == 1) {
                return "Invalid expression"; // Operadores duplos
            }
        } else if (isFunction(token)) {
            if (i + 1 >= tokens.size() || tokens[i + 1] != "(") {
                return "Invalid expression"; // Função deve ser seguida por '('
            }
        } else if (std::isdigit(token[0]) || (token.size() > 1 && std::isdigit(token[1]))) {
            // Número é considerado válido
        } else {
            return "Invalid expression";
        }
    }
    return parenthesesBalance == 0 ? "Valid" : "Mismatched parentheses";
}

// Converte uma expressão infixa para posfixa usando o algoritmo de Shunting Yard
std::string infixToPostfix(const std::string& infix) {
    std::stack<std::string> operators;
    std::stringstream output;
    auto tokens = tokenize(infix);

    std::string validationResult = validateExpression(tokens);
    if (validationResult != "Valid") {
        return validationResult;
    }

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
            if (!operators.empty() && operators.top() == "(") {
                operators.pop(); // Remove o '('
            } else {
                return "Mismatched parentheses";
            }
            if (!operators.empty() && isFunction(operators.top())) {
                output << operators.top() << ' ';
                operators.pop();
            }
        } else if (isOperator(token[0])) {
            while (!operators.empty() && ((token[0] != '^' && precedence(operators.top()[0]) >= precedence(token[0])) ||
                   (token[0] == '^' && precedence(operators.top()[0]) > precedence(token[0])))) {
                output << operators.top() << ' ';
                operators.pop();
            }
            operators.push(token);
        }
    }

    // Esvazia a pilha de operadores
    while (!operators.empty()) {
        if (operators.top() == "(") {
            return "Mismatched parentheses";
        }
        output << operators.top() << ' ';
        operators.pop();
    }

    return output.str();
}

// Função de teste
void runTests() {
    struct TestCase {
        std::string infix;
        std::string expected;
    };

    std::vector<TestCase> testCases = {
        {"3+4*2/(1-5)^2^3", "3 4 2 * 1 5 - 2 3 ^ ^ / + "},
        {"SIN(3+4)*COS(2-1)", "3 4 + SIN 2 1 - COS * "},
        {"-3+4*-2/(1--5)^2^3", "0 3 - 4 0 2 - * 1 0 5 - - / 2 3 ^ ^ + "},
        {"3++4", "Invalid expression"},
        {"SIN(3+4)*INVALID(2-1)", "Invalid expression"},
        {"3+4**2", "Invalid expression"},
        {"(3+4", "Mismatched parentheses"}
    };

    for (const auto& test : testCases) {
        std::string result = infixToPostfix(test.infix);
        std::cout << "Infix: " << test.infix << std::endl;
        std::cout << "Expected Postfix: " << test.expected << std::endl;
        std::cout << "Actual Postfix: " << result << std::endl;
        std::cout << (result == test.expected ? "PASS" : "FAIL") << std::endl;
        std::cout << "-----------------------" << std::endl;
    }
}

int main() {
    runTests();
    return 0;
}
