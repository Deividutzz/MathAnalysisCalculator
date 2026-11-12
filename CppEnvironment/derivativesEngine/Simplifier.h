#pragma once
#include <memory>
#include <cmath>
#include <stdexcept>
#include "FunctionDataStructure.h"

using namespace std;

class Simplifier {
public:
    Simplifier() = default;

    unique_ptr<Node> simplify(const Node* root) {
        if (!root) return nullptr;

        if (root->getType() == NodeType::CONSTANT) {return make_unique<Node>(root->getValue());}
        if (root->getType() == NodeType::VARIABLE) {return make_unique<Node>(root->getVariable());}

        /// Step 1: Call recursively left subtree and right subtree (Bottom-Up Post-Order Traversal)
        auto simplifiedLeft = simplify(root->getLeft());
        auto simplifiedRight = simplify(root->getRight());

        /// Step 2: Identify cases of every node of each subtree (unary case)
        if (simplifiedLeft && !simplifiedRight) {
            if (simplifiedLeft->getType() == NodeType::CONSTANT) {
                auto value = simplifiedLeft->getValue();

                switch (root->getType()) {
                    case NodeType::SIN: return make_unique <Node>(sin(value));
                    case NodeType::COS: return make_unique <Node>(cos(value));
                    case NodeType::EXP: return make_unique <Node>(exp(value));
                    case NodeType::LOG:
                        if (value < 1) throw runtime_error("Domain error: Logarithm value cannot be less than 1");
                            return make_unique <Node>(log(value));
                    default: break;
                }
            }
            return make_unique <Node>(root->getType(), std::move(simplifiedLeft));
        }

        if (auto assocFolded = foldAssociative(
            root->getType(),simplifiedLeft, simplifiedRight)) {
            return simplify(assocFolded.get());
        }

        /// Step 3: Binary operators
        /// If both sides are constant these should immediately be simplified.
        if (simplifiedLeft->getType() == NodeType::CONSTANT
        && simplifiedRight->getType() == NodeType::CONSTANT) {
            auto ltValue = simplifiedLeft->getValue();
            auto rtValue = simplifiedRight->getValue();

            switch (root->getType()) {
                case NodeType::ADD: return make_unique <Node>(ltValue + rtValue);
                case NodeType::SUBTRACT: return make_unique <Node>(ltValue - rtValue);
                case NodeType::MULTIPLY: return make_unique <Node>(ltValue * rtValue);
                case NodeType::DIVIDE:
                    if ( rtValue == 0.0 ) throw runtime_error("Domain error: Division by zero cannot be performed");
                    return make_unique <Node>(ltValue / rtValue);
                case NodeType::POWER: return make_unique <Node>(pow(ltValue, rtValue));
                default: break;
            }
        }

        /// Step 4: Algebric Redundancy Eliminations
        /// Inspect identities and annihilators side-by-side
        NodeType op = root->getType();

        switch (op) {
            case NodeType::ADD:
                if (isConstant(simplifiedRight.get(), 0.0)) return simplifiedLeft;
                if (isConstant(simplifiedLeft.get(), 0.0)) return simplifiedRight;

                break;
            case NodeType::SUBTRACT:
                if (isConstant(simplifiedRight.get(), 0.0)) return simplifiedLeft;
                //if (isConstant(simplifiedLeft.get(), 0.0)) return simplifiedRight;
                /// this case needs further correction because the case is 0 - x

                break;
            case NodeType::MULTIPLY:
                if (isConstant(simplifiedLeft.get(),0.0)
                || isConstant(simplifiedRight.get(), 0.0)) return make_unique<Node>(0.0);

                if (isConstant(simplifiedRight.get(),1.0)) return simplifiedLeft; // x*1 = x
                if (isConstant(simplifiedLeft.get(),1.0)) return simplifiedRight; // 1*x = x

                break;
            case NodeType::DIVIDE:
                if (isConstant(simplifiedLeft.get(),0.0)) return make_unique<Node>(0.0);
                if (isConstant(simplifiedRight.get(),1.0)) return simplifiedLeft;

                break;
            case NodeType::POWER:
                if (isConstant(simplifiedRight.get(),1.0)) return simplifiedLeft;
                if (isConstant(simplifiedRight.get(),0.0)) return make_unique<Node>(1.0);
                if (isConstant(simplifiedLeft.get(),0.0)) return make_unique<Node>(0.0);
                if (isConstant(simplifiedLeft.get(),1.0)) return make_unique<Node>(1.0);

                break;
            default: break;
        }

        // If no simplification rules were found, make a new unique node with the simplified subtrees
        return make_unique<Node>(op, std::move(simplifiedLeft), std::move(simplifiedRight));
    }
private:
    // Helper method for folding constants across associative chains
    unique_ptr<Node> foldAssociative(NodeType op,
    unique_ptr<Node>& left, unique_ptr<Node>& right) {
        if (op != NodeType::ADD && op != NodeType::MULTIPLY) {
            return nullptr;
        }

        /// Lambda helper
        auto compute = [&](double a, double b) {
            return (op == NodeType::ADD) ? (a + b) : (a * b);
        };

        // Case A: Left is an operator of the same type, Right is a constant
        if (left->getType() == op && right->getType() == NodeType::CONSTANT) {
            double c2 = right->getValue();
            Node* innerLeft = left->getLeft();
            Node* innerRight = left->getRight();

            // Subcase 1: (c1 op x) op c2 -> (c1 op c2) op x
            if (innerLeft->getType() == NodeType::CONSTANT) {
                double combined = compute(innerLeft->getValue(), c2);
                return make_unique<Node>(op,make_unique<Node>(combined),innerRight->clone());
            }
            // Subcase 2: (x op c1) op c2  ->  (c1 op c2) op x
            if (innerRight->getType() == NodeType::CONSTANT) {
                double combined = compute(innerRight->getValue(), c2);
                return make_unique<Node>(op, make_unique<Node>(combined), innerLeft->clone());
            }
        }

        // Case B: Left is a constant, Right is an op of the same type
        if (left->getType() == NodeType::CONSTANT && right->getType() == op) {
            double c1 = left->getValue();
            Node* innerLeft = right->getLeft();
            Node* innerRight = right->getRight();

            // Subcase 3: c1 op (c2 op x)  ->  (c1 op c2) op x
            if (innerLeft->getType() == NodeType::CONSTANT) {
                double combined = compute(c1, innerLeft->getValue());
                return make_unique<Node>(op, make_unique<Node>(combined), innerRight->clone());
            }
            // Subcase 4: c1 op (x op c2)  ->  (c1 op c2) op x
            if (innerRight->getType() == NodeType::CONSTANT) {
                double combined = compute(c1, innerRight->getValue());
                return make_unique<Node>(op, make_unique<Node>(combined), innerLeft->clone());
            }
        }

        return nullptr; // It means the pattern did not match
    }

    // Helper to check if a node is a specific constant value safely
    static bool isConstant(const Node* node, double value) {
        return node && node->getType() == NodeType::CONSTANT && std::abs(node->getValue() - value) < 1e-9;
    }
};