#include <iostream>
#include <memory>
#include <string>
#include "FunctionUnparser.h"
#include "ExpressionHandler.h"

using namespace std;
int main()
{
    string equation = "";
    getline(cin, equation);
    Expression expr(equation);
    cout << "The partial differential is: " << expr.getDiff('x');
    // need later an unparser
    return 0;
}

/*
 *Reference:
 *

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
        VariableIdentifier var = parser.getConverter(dx);
        unique_ptr<Node> differential = differentiate(var);
        // return a method that unparse this
    }

private:
    Parser parser;
    unique_ptr<Node> expression;

    unique_ptr<Node> differentiate(VariableIdentifier dx) {
        Differentiator diff;
        diff.differentiate(std::move(expression.get()),dx);
    }
};
*/