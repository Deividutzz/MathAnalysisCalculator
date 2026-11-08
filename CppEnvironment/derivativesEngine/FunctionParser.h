// Parser.h
#pragma once
#include <memory>
#include <string>
#include <cctype>

#include "DifferentiationEngine.h"
#include "FunctionDataStructure.h"

using namespace std;

class Parser {
public:
    Parser(string input) {
        string clearStr = "";
        for (char c : input) {
            if (!isspace(c)) clearStr += c;
        }
        this->input = clearStr;
        this->currentIndex = 0;

        this->expression = parseExpression();
        if (currentIndex < this->input.length()) {
            throw runtime_error("Unexpected trailing characters starting at: " + this->input.substr(currentIndex));
        }
    }

    unique_ptr<Node> getAST() {
        return std::move(expression);
    }

    VariableIdentifier getConverter(char op) {
        return convertVariable(op);
    }
private:
    string input;
    int currentIndex = 0;
    unique_ptr<Node> expression;

    unique_ptr<Node> parseExpression() {
        unique_ptr<Node> left = parseTerm();

        char op = peek();
        while (op == '+' || op == '-') {
            consume(peek());

            unique_ptr<Node> right = parseTerm();
            if (!right) throw runtime_error("Missing operand after operator!");
            NodeType nodeType = convertOperator(op);

            left = make_unique<Node>(nodeType, std::move(left), std::move(right));
            op = peek();
        }
        return left;
    }
    unique_ptr<Node> parseTerm() {
        unique_ptr<Node> left = parseFactor();

        char op = peek();
        while (op == '*' || op == '/') {
            consume(peek());

            unique_ptr<Node> right = parseFactor();
            if (!right) throw runtime_error("Missing operand after operator!");
            NodeType nodeType = convertOperator(op);

            left =  make_unique<Node>(nodeType, std::move(left), std::move(right));
            op = peek();
        }
        return left;
    }
    unique_ptr<Node> parseFactor() {
        if (peek() == '-') {
            consume(peek());

            unique_ptr<Node> child = parseFactor();
            if (!child) throw runtime_error("Expected expression after '-'");
            /// substracting from 0 the found (child) value
            return make_unique<Node>(NodeType::SUBTRACT, make_unique<Node>(0.0), std::move(child));
        }

        unique_ptr<Node> left = parsePrimary();

        char op = peek();
        while (op == '^') {
            consume(peek());

            unique_ptr<Node> right = parseFactor();
            if (!right) throw runtime_error("Missing operand after operator!");
            NodeType nodeType = convertOperator(op);

            left =  make_unique<Node>(nodeType, std::move(left), std::move(right));
            op = peek();
        }
        return left;
    }
    unique_ptr<Node> parsePrimary() {
        char current = peek();

        if (isalpha(current) && !(current == 'x' || current == 'y' || current == 'z')) {
            string funcName = "";
            while (isalpha(peek())) {
                funcName += peek();
                consume(peek());
            }

            consume('(');
            unique_ptr<Node> inner = parseExpression(); /// parsing the entire function from inside the trig function
            consume(')');

            if (funcName == "sin") return make_unique<Node>(NodeType::SIN, std::move(inner));
            if (funcName == "cos") return make_unique<Node>(NodeType::COS, std::move(inner));
            if (funcName == "exp") return make_unique<Node>(NodeType::EXP, std::move(inner));
            if (funcName == "log") return make_unique<Node>(NodeType::LOG, std::move(inner));

            throw runtime_error("Unknown function: " + funcName);
        }

        if (current == 'x' || current == 'y' || current == 'z') {
            consume(current);
            return make_unique<Node>(convertVariable(current));
        }

        if (isdigit(current) || current == '.') {
            string numStr = "";
            while (isdigit(peek()) || peek() == '.') {
                numStr += peek();
                consume(peek());
            }
            return make_unique<Node>(stod(numStr));
        }

        if (current == '(') {
            consume('(');

            // Re-route processing right back to the top-level rule!
            unique_ptr<Node> insideTree = parseExpression();

            if (peek() == ')') {
                consume(')');
            }
            return insideTree;
        }

        return nullptr;
    }

    char peek() {
        if (currentIndex < input.length())
            return input[currentIndex];
        return '\0';
    }

    void consume(char expected) {
        if (peek() == expected) {
            currentIndex++;
        } else {
            throw runtime_error(string("Unexpected character: expected '") + expected + "' but got '" + peek() + "'");
        }
    }

    static NodeType convertOperator(char op) {
        switch (op) {
            case '+':
                return NodeType::ADD;
            case '-':
                return NodeType::SUBTRACT;
            case '*':
                return NodeType::MULTIPLY;
            case '/':
                return NodeType::DIVIDE;
            case '^':
                return NodeType::POWER;
            default:
                return NodeType::CONSTANT; // Fallback safety
        }
    }
    static VariableIdentifier convertVariable(char op) {
        switch (op) {
            case 'x':
                return VariableIdentifier::X;
            case 'y':
                return VariableIdentifier::Y;
            case 'z':
                return VariableIdentifier::Z;
            default:
                return VariableIdentifier::X;
        }
    }
};

/*
    try
    {
        Parser myParser("2 + 3 * (x ^ 2 - y)");
        unique_ptr<Node> root = myParser.getAST();
        if (root != nullptr) {
            cout << "AST built successfully without memory leaks!" << endl;
        }
    } catch (const exception& e) {
        cout << "Parsing failed: " << e.what() << endl;
    }
*/