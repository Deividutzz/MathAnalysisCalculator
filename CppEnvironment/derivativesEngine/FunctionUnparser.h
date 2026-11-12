#pragma once
#include <string>
#include <sstream>
#include <stdexcept>
#include "FunctionDataStructure.h"
using namespace std;

class ASTPrinter {
public:
    // Public method that converts from AST to str
    static string unparse(const Node* root) {
        if (!root) return "";

        switch (root->getType()) {
            case NodeType::CONSTANT: {
                // Converting from const to str
                // Basic technique for removing redundant 0
                string valStr = to_string(root->getValue());
                valStr.erase(valStr.find_last_not_of('0') + 1, string::npos);
                if (valStr.back() == '.') {
                    valStr.pop_back();
                }
                return valStr;
            }

            case NodeType::VARIABLE:
                // Mapping the content with the correct variable
                switch (root->getVariable()) {
                    case VariableIdentifier::X: return "x";
                    case VariableIdentifier::Y: return "y";
                    case VariableIdentifier::Z: return "z";
                    default: return "unknown_var";
                }

            case NodeType::ADD: {
                int pred = getPrecedence(NodeType::ADD);
                return unparseChild(root->getLeft(),pred,false,NodeType::ADD)
                + "+" + unparseChild(root->getRight(),pred,true,NodeType::ADD);
            }
            case NodeType::SUBTRACT: {
                int pred = getPrecedence(NodeType::SUBTRACT);
                return unparseChild(root->getLeft(),pred,false,NodeType::SUBTRACT)
                + "-" + unparseChild(root->getRight(),pred,true,NodeType::SUBTRACT);
            }
            case NodeType::MULTIPLY: {
                int pred = getPrecedence(NodeType::MULTIPLY);
                return unparseChild(root->getLeft(),pred,false,NodeType::MULTIPLY)
                + "*" + unparseChild(root->getRight(),pred,true,NodeType::MULTIPLY);
            }
            case NodeType::DIVIDE: {
                int pred = getPrecedence(NodeType::DIVIDE);
                return unparseChild(root->getLeft(),pred,false,NodeType::DIVIDE)
                + "/" + unparseChild(root->getRight(),pred,true,NodeType::DIVIDE);
            }
            case NodeType::POWER: {
                int pred = getPrecedence(NodeType::POWER);
                return unparseChild(root->getLeft(),pred,false,NodeType::POWER)
                + "^" + unparseChild(root->getRight(),pred,true,NodeType::POWER);
            }
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
private:
    /// Helper to get the operator precedence
    /// (Higher number = Higher priority)
    static int getPrecedence(NodeType type) {
        switch (type) {
            case NodeType::ADD:
            case NodeType::SUBTRACT:
                return 1;

            case NodeType::MULTIPLY:
            case NodeType::DIVIDE:
                return 2;

            case NodeType::POWER:
                return 3;

            case NodeType::SIN:
            case NodeType::COS:
            case NodeType::EXP:
            case NodeType::LOG:
                return 4;
            default:
                return 5;
        }
    }

    /// Helper to safely unparse a child and wrap it in paretheses ONLY if required
    static string unparseChild(const Node* child,
    int parentPrecedence, bool isRightChild, NodeType parentType) {
        if (!child) return "";

        int childPrecedence = getPrecedence(child->getType());

        // Rule 1: If child has strictly lower precedence, it MUST have parantheses
        bool needParantheses = childPrecedence < parentPrecedence;

        // Rule 2: Handle non-commutative
        // or left-associative operators (SUBTRACT and DIVIDE)
        // If the right child has EQUAL precedence to a '-' or '/', we must wrap it
        if (isRightChild && childPrecedence == parentPrecedence) {
            if (parentType == NodeType::SUBTRACT
            || parentType == NodeType::DIVIDE) {
                needParantheses = true;
            }
        }

        // Rule 3: Exponentiation (PWR) is right-associative in parser
        // If the left child of a "^" has EQUAL precedence, then it must wrap
        // Example: (x ^ 2) ^ 3 is NOT the same as x ^ 2 ^ 3 (which means x ^ (2 ^ 3))
        if (!isRightChild && childPrecedence == parentPrecedence
            && parentType == NodeType::POWER) {
            needParantheses = true;
        }

        string childStr = unparse(child);
        if (needParantheses) {
            return "(" + childStr + ")";
        }
        return childStr;
    }
};
