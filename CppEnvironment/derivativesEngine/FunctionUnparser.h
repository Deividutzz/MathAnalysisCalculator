#pragma once
#include <string>
#include <sstream>
#include <stdexcept>
#include "FunctionDataStructure.h"
using namespace std;

class ASTPrinter {
public:
    // Public method that converts from AST to str
    static std::string unparse(const Node* root) {
        if (!root) return "";

        switch (root->getType()) {
            case NodeType::CONSTANT: {
                // Convertim double la string.
                // Folosim o logică simplă pentru a elimina zerourile inutile (ex: 2.000000 -> 2)
                std::string valStr = std::to_string(root->getValue());
                valStr.erase(valStr.find_last_not_of('0') + 1, std::string::npos);
                if (valStr.back() == '.') {
                    valStr.pop_back();
                }
                return valStr;
            }

            case NodeType::VARIABLE:
                // Mapăm identificatorul la litera corespunzătoare
                switch (root->getVariable()) {
                    case VariableIdentifier::X: return "x";
                    case VariableIdentifier::Y: return "y";
                    case VariableIdentifier::Z: return "z";
                    default: return "unknown_var";
                }

            case NodeType::ADD:
                return "(" + unparse(root->getLeft()) + " + " + unparse(root->getRight()) + ")";

            case NodeType::SUBTRACT:
                return "(" + unparse(root->getLeft()) + " - " + unparse(root->getRight()) + ")";

            case NodeType::MULTIPLY:
                return "(" + unparse(root->getLeft()) + " * " + unparse(root->getRight()) + ")";

            case NodeType::DIVIDE:
                return "(" + unparse(root->getLeft()) + " / " + unparse(root->getRight()) + ")";

            case NodeType::POWER:
                return "(" + unparse(root->getLeft()) + " ^ " + unparse(root->getRight()) + ")";

            case NodeType::SIN:
                return "sin(" + unparse(root->getLeft()) + ")";

            case NodeType::COS:
                return "cos(" + unparse(root->getLeft()) + ")";

            case NodeType::EXP:
                return "exp(" + unparse(root->getLeft()) + ")";

            case NodeType::LOG:
                return "log(" + unparse(root->getLeft()) + ")";

            default:
                throw std::runtime_error("Unknown node type");
        }
    }
};
