#pragma once
#include <memory>
#include <string>
#include <cctype>
#include <iostream>

#include "FunctionParser.h"
#include "FunctionUnparser.h"
#include "DifferentiationEngine.h"
#include "FunctionDataStructure.h"
#include "Simplifier.h"
using namespace std;

class Expression {
public:
    Expression(string equation) : parser(equation) {
        try {
            //this->parser = new Parser(equation);
            expression = parser.getAST();
        } catch (const exception &e) {
            cout << "Parsing failed: " << e.what() << endl;
        }
    }

    string getDiff(char dx) {
        if (!expression) {
            return "Error: No valid expression tree found.";
        }
        Simplifier simple;

        VariableIdentifier var = parser.getConverter(dx);
        unique_ptr<Node> differential = differentiate(var);
        differential = simple.simplify(differential.get());

        return ASTPrinter::unparse(differential.get());
        // return a method that unparse this
    }

private:
    Parser parser;
    unique_ptr<Node> expression;

    unique_ptr<Node> differentiate(VariableIdentifier dx) {
        Differentiator diff;

        return diff.differentiate(expression.get(),dx);
    }
};