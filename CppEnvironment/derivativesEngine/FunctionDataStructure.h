// FunctionDataStructure.h
#pragma once
#include <memory>
using namespace std;

enum class VariableIdentifier { X, Y, Z };

enum class NodeType {
    CONSTANT,
    VARIABLE,
    ADD, SUBTRACT, MULTIPLY, DIVIDE, POWER,
    SIN, COS, EXP, LOG
};

class Node {
public:
    // 1. Constant Leaf Node
    Node(double number)
        : nodeType(NodeType::CONSTANT), numericVal(number), varIdentifier(VariableIdentifier::X) {}

    // 2. Variable Leaf Node
    Node(VariableIdentifier variable)
        : nodeType(NodeType::VARIABLE), numericVal(0.0), varIdentifier(variable) {}

    // 3. Binary Operator
    Node(NodeType type, std::unique_ptr<Node> left, std::unique_ptr<Node> right)
    : nodeType(type), numericVal(0.0), varIdentifier(VariableIdentifier::X),
      leftChild(std::move(left)), rightChild(std::move(right))
    {
        // Validation
        if (!leftChild || !rightChild) {
            throw std::invalid_argument("Binary operator is missing operands");
        }
        if (type == NodeType::CONSTANT || type == NodeType::VARIABLE ||
            type == NodeType::SIN || type == NodeType::COS || type == NodeType::EXP || type == NodeType::LOG) {
            throw std::invalid_argument("Invalid node type for binary operator");
            }
    }

    // 4. Unary Operators (SIN, COS, EXP, LOG) / Function Node
    Node(NodeType type, std::unique_ptr<Node> left)
        : nodeType(type), numericVal(0.0), varIdentifier(VariableIdentifier::X),
          leftChild(std::move(left)), rightChild(nullptr)
    {
        // Validation
        if (!leftChild) {
            throw invalid_argument("Missing unary operator (function) operand");
        }
        if (type != NodeType::SIN && type != NodeType::COS && type != NodeType::EXP && type != NodeType::LOG) {
            throw invalid_argument("Invalid node type for unary operator");
        }
    }

    // Public getters to safely inspect the node
    NodeType getType() const { return nodeType; }
    double getValue() const { return numericVal; }
    VariableIdentifier getVariable() const { return varIdentifier; }

    Node* getLeft() const { return leftChild.get(); }
    Node* getRight() const { return rightChild.get(); }

    // Allows modifying children during tree assembly
    void setLeft(unique_ptr<Node> left) { leftChild = std::move(left); }
    void setRight(std::unique_ptr<Node> right) { rightChild = std::move(right); }

    unique_ptr<Node> clone() const {
        // 1. If it is a constant value, we add it to a new node
        if (nodeType == NodeType::CONSTANT) {
            return make_unique<Node>(numericVal);
        }
        // 2. If it is a variable, we add it to a new node
        if (nodeType == NodeType::VARIABLE) {
            return make_unique<Node>(varIdentifier);
        }
        // 3. If it is an operator, we clone recursively the children
        auto leftCopy = leftChild ? leftChild->clone() : nullptr;
        auto rightCopy = rightChild ? rightChild->clone() : nullptr;

        return make_unique<Node>(nodeType, std::move(leftCopy), std::move(rightCopy));
    }
private:
    NodeType nodeType;
    double numericVal;
    VariableIdentifier varIdentifier;

    // Smart pointers handle memory cleanup automatically when a Node is deleted
    unique_ptr<Node> leftChild;
    unique_ptr<Node> rightChild;
};
/*
class FunctionManager {
public:
    FunctionManager() : root(nullptr) {}
    FunctionManager(unique_ptr<Node> expressionRoot) : root(std::move(expressionRoot)) {}

    // Check if the manager currently stores a function
    bool isEmpty() const { return root == nullptr; }
    Node* getRoot() const { return root.get(); }

private:
    unique_ptr<Node> root;
};
*/