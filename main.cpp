/*
Shunting yard implementation © 2024 by Cleuton Sampaio
is licensed under CC BY-SA 4.0. To view a copy of this license,
visit https://creativecommons.org/licenses/by-sa/4.0/
 */
#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <cmath>
#include <stdexcept>

// Verifica se um caractere é operador
bool eOperador(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

// Retorna a precedência do operador
int precedencia(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

// Verifica se uma string é uma função válida
bool eFuncao(const std::string& token) {
    return token == "EXP" || token == "SQR" || token == "SIN" || token == "COS";
}

// Analisa a expressão e divide em tokens
std::vector<std::string> tokenizar(const std::string& infix) {
    std::vector<std::string> tokens;
    std::string token;
    bool expectOperand = true; // Indica se estamos esperando um operando (usado para operadores unários)

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
            expectOperand = false;
        } else if (std::isalpha(infix[i])) {
            token += infix[i];
            while (i + 1 < infix.size() && std::isalpha(infix[i + 1])) {
                token += infix[++i];
            }
            tokens.push_back(token);
            token.clear();
            expectOperand = false;
        } else if (eOperador(infix[i])) {
            if (expectOperand && infix[i] == '-') {
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
            expectOperand = true;
        } else if (infix[i] == '(' || infix[i] == ')') {
            tokens.push_back(std::string(1, infix[i]));
            expectOperand = (infix[i] == '(');
        }
    }
    return tokens;
}

// Verifica se uma expressão infixa é válida
std::string validarExpressao(const std::vector<std::string>& tokens) {
    int parenthesesBalance = 0;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        if (token == "(") {
            parenthesesBalance++;
        } else if (token == ")") {
            parenthesesBalance--;
            if (parenthesesBalance < 0) {
                return "Parenteses desbalanceados";
            }
        } else if (eOperador(token[0]) && token.size() == 1) {
            if (i == 0 || i == tokens.size() - 1) {
                return "Expressao invalida"; // Operador no início ou no fim
            }
            if (eOperador(tokens[i + 1][0]) && tokens[i + 1].size() == 1) {
                return "Expressao invalida"; // Operadores duplos
            }
        } else if (eFuncao(token)) {
            if (i + 1 >= tokens.size() || tokens[i + 1] != "(") {
                return "Expressao invalida"; // Função deve ser seguida por '('
            }
        } else if (std::isdigit(token[0]) || (token.size() > 1 && std::isdigit(token[1]))) {
            // Número é considerado válido
        } else {
            return "Expressao invalida";
        }
    }
    return parenthesesBalance == 0 ? "OK" : "Parenteses desbalanceados";
}

// Converte uma expressão infixa para posfixa usando o algoritmo de Shunting Yard
std::string infixToPostfix(const std::string& infix) {
    std::stack<std::string> operators;
    std::stringstream output;
    auto tokens = tokenizar(infix);

    std::string resultadoValidacao = validarExpressao(tokens);
    if (resultadoValidacao != "OK") {
        return resultadoValidacao;
    }

    for (const auto& token : tokens) {
        if (std::isdigit(token[0]) || (token.size() > 1 && std::isdigit(token[1]))) {
            // Token é um operando (número)
            output << token << ' ';
        } else if (eFuncao(token)) {
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
                return "Parenteses desbalanceados";
            }
            if (!operators.empty() && eFuncao(operators.top())) {
                output << operators.top() << ' ';
                operators.pop();
            }
        } else if (eOperador(token[0])) {
            while (!operators.empty() && ((token[0] != '^' && precedencia(operators.top()[0]) >= precedencia(token[0])) ||
                   (token[0] == '^' && precedencia(operators.top()[0]) > precedencia(token[0])))) {
                output << operators.top() << ' ';
                operators.pop();
            }
            operators.push(token);
        }
    }

    // Esvazia a pilha de operadores
    while (!operators.empty()) {
        if (operators.top() == "(") {
            return "Parenteses desbalanceados";
        }
        output << operators.top() << ' ';
        operators.pop();
    }

    return output.str();
}

double senoGraus(double value) {
    return std::sin(value * M_PI / 180.0);
}

double cossenoGraus(double value) {
    return std::cos(value * M_PI / 180.0);
}


// Avalia uma expressão em notação polonesa reversa (RPN)
double avaliarRPN(const std::string& rpn) {
    std::stack<double> stack;
    std::istringstream tokens(rpn);
    std::string token;

    while (tokens >> token) {
        if (std::isdigit(token[0]) || (token.size() > 1 && std::isdigit(token[1]))) {
            stack.push(std::stod(token));
        } else if (eOperador(token[0])) {
            if (stack.size() < 2) throw std::runtime_error("Invalid RPN expression");
            double b = stack.top(); stack.pop();
            double a = stack.top(); stack.pop();
            switch (token[0]) {
                case '+': stack.push(a + b); break;
                case '-': stack.push(a - b); break;
                case '*': stack.push(a * b); break;
                case '/': stack.push(a / b); break;
                case '^': stack.push(std::pow(a, b)); break;
                default: throw std::runtime_error("Unknown operator");
            }
        } else if (eFuncao(token)) {
            if (stack.empty()) throw std::runtime_error("Invalid RPN expression");
            double a = stack.top(); stack.pop();
            if (token == "SIN") stack.push(senoGraus(a));
            else if (token == "COS") stack.push(cossenoGraus(a));
            else if (token == "EXP") stack.push(std::exp(a));
            else if (token == "SQR") stack.push(std::sqrt(a));
            else throw std::runtime_error("Unknown function");
        } else {
            throw std::runtime_error("Unknown token");
        }
    }

    if (stack.size() != 1) throw std::runtime_error("Invalid RPN expression");
    return stack.top();
}


// Função de teste
void executarTestes() {
    struct TestCase {
        std::string infix;
        std::string RPNexperada;
        double valorEsperado;
    };

    std::vector<TestCase> testCases = {
        {"3+4*2/(1-5)^2^3", "3 4 2 * 1 5 - 2 3 ^ ^ / + ", 3.0001220703125},
        {"SIN(3+4)*COS(2-1)", "3 4 + SIN 2 1 - COS * ", 0.121851},
        {"-3+4*-2/(1--5)^2^3", "-3 4 -2 * 1 -5 - 2 3 ^ ^ / + ", -3 + 4 * -2 / std::pow(1 - -5, 8)},
        {"3++4", "Expressao invalida", 0.0},
        {"SIN(3+4)*INVALID(2-1)", "Expressao invalida", 0.0},
        {"3+4**2", "Expressao invalida", 0.0},
        {"(3+4", "Parenteses desbalanceados", 0.0}
    };

    for (const auto& test : testCases) {
        std::string RPNresultante = infixToPostfix(test.infix);
        std::cout << "Infixa: " << test.infix << std::endl;
        std::cout << "RPN esperada: " << test.RPNexperada << std::endl;
        std::cout << "RPN retornada: " << RPNresultante << std::endl;

        if (RPNresultante == test.RPNexperada) {
            std::cout << "Conversao em RPN: OK" << std::endl;
        } else {
            std::cout << "Conversao em RPN: FALHA" << std::endl;
        }

        if (RPNresultante == test.RPNexperada && RPNresultante != "Expressao invalida" && RPNresultante != "Parenteses desbalanceados") {
            double valorResultante = avaliarRPN(RPNresultante);
            std::cout << "Valor esperado: " << test.valorEsperado << std::endl;
            std::cout << "Valor calculado: " << valorResultante << std::endl;
            if (std::fabs(valorResultante - test.valorEsperado) < 1e-6) {
                std::cout << "Avaliacao da RPN: OK" << std::endl;
            } else {
                std::cout << "Avaliacao da RPN: FALHA" << std::endl;
            }
        }
        std::cout << "-----------------------" << std::endl;
    }
}

int main() {
    executarTestes();
    return 0;
}
