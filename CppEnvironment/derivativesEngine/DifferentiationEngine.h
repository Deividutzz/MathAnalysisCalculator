#pragma once
#include <memory>
#include <cctype>
#include "FunctionDataStructure.h"

using namespace std;

class Differentiator {
public:
    Differentiator() = default;
        /// dx is with respect to which variable we will differentiate
        //this->expression = std::move(expression);

    unique_ptr<Node> differentiate(const Node* root, VariableIdentifier dx) {
        // travel down the tree and identify cases
        if (!root) return nullptr;

        /// the following implem respects a recursion algorithm that
        /// uses the concept breaking every member into small pieces,
        /// that will be later possible to differentiate. (kinda like divide et impera)
        switch (root->getType()) {
            case NodeType::CONSTANT:
                return createConstantNode(0.0);
            case NodeType::VARIABLE:
                if (root->getVariable() == dx) {
                    return createConstantNode(1.0);
                }
                return createConstantNode(0.0);
            case NodeType::ADD:
                return createBinaryNode( NodeType::ADD,
                    differentiate(root->getLeft(),dx),
                    differentiate(root->getRight(),dx)
                    );
            case NodeType::SUBTRACT:
                return createBinaryNode( NodeType::SUBTRACT,
                    differentiate(root->getLeft(),dx),
                    differentiate(root->getRight(),dx)
                    );
            case NodeType::MULTIPLY:
                /// Rule: (u * v)' = u' * v + u * v'
                return createBinaryNode( NodeType::ADD,
                createBinaryNode(NodeType::MULTIPLY,differentiate(root->getLeft(),dx),root->getRight()->clone()),
                createBinaryNode(NodeType::MULTIPLY,root->getLeft()->clone(),differentiate(root->getRight(),dx))
                );
            case NodeType::DIVIDE:
                /// Rule: (u / v)' = (u' * v - u * v') / (v^2)
                return createBinaryNode( NodeType::DIVIDE,
                createBinaryNode(NodeType::SUBTRACT,
                    createBinaryNode(NodeType::MULTIPLY,differentiate(root->getLeft(),dx),root->getRight()->clone()),
                    createBinaryNode(NodeType::MULTIPLY, root->getLeft()->clone(), differentiate(root->getRight(),dx))
                    ),
                createBinaryNode(NodeType::POWER,root->getRight()->clone(),createConstantNode(2.0))
                );
            case NodeType::SIN: {
                auto cos_u = createUnaryNode(NodeType::COS,root->getLeft()->clone());

                auto u_prim = differentiate(root->getLeft(),dx);
                return createBinaryNode(NodeType::MULTIPLY, std::move(cos_u), std::move(u_prim));
            }
            case NodeType::COS: {
                auto sin_u = createUnaryNode(NodeType::SIN,root->getLeft()->clone());

                auto u_prim = differentiate(root->getLeft(),dx);
                auto result = createBinaryNode(NodeType::MULTIPLY, std::move(sin_u), std::move(u_prim));
                return createBinaryNode(NodeType::SUBTRACT, createConstantNode(0.0),std::move(result));
            }
            case NodeType::EXP: { // Rule: (e^u)' = eexpexp^u * u'
                auto exp_u = createUnaryNode(NodeType::EXP,root->getLeft()->clone());
                auto u_prim = differentiate(root->getLeft(),dx);
                return createBinaryNode(NodeType::MULTIPLY, std::move(exp_u), std::move(u_prim));
            }
            case NodeType::LOG: { // Rule: (ln(u))' = (1 / u) * u'  or simplier u' / u
                auto cloned_u = root->getLeft()->clone();
                auto u_prim = differentiate(root->getLeft(),dx);
                return createBinaryNode(NodeType::DIVIDE, std::move(u_prim), std::move(cloned_u));
            }
            case NodeType::POWER: {
                // root->getLeft() este baza (u)
                // root->getRight() este exponentul (v)

                bool isBaseConstant = (root->getLeft()->getType() == NodeType::CONSTANT ||
                    (root->getLeft()->getType() == NodeType::VARIABLE && root->getLeft()->getVariable() != dx));
                bool isExponentConstant = (root->getRight()->getType() == NodeType::CONSTANT ||
                    (root->getRight()->getType() == NodeType::VARIABLE && root->getRight()->getVariable() != dx));

                // Case 1: (u^n)' = n * u^(n-1) * u'
                if (isExponentConstant) {
                    auto v_node = root->getRight()->clone();
                    auto newv_node = createBinaryNode(NodeType::SUBTRACT,root->getRight()->clone(),createConstantNode(1.0));

                    auto new_pwr = createBinaryNode(NodeType::POWER,root->getLeft()->clone(),std::move(newv_node));

                    auto v_ori_u_pwr = createBinaryNode(NodeType::MULTIPLY, std::move(v_node), std::move(new_pwr));
                    auto u_prim = differentiate(root->getLeft(), dx);

                    return createBinaryNode(NodeType::MULTIPLY, std::move(v_ori_u_pwr), std::move(u_prim));
                }

                // Case 2: (a^u)' = a^u * ln(a) * u'
                if (isBaseConstant) {
                    // root->getLeft() is constant
                    // root->getRight() is the function

                    auto a_pwr_u = root->clone();
                    auto ln_a = createUnaryNode(NodeType::LOG, root->getLeft()->clone());
                    auto u_prim = differentiate(root->getRight(), dx);
                    auto a_pwr_u_ori_ln_a = createBinaryNode(NodeType::MULTIPLY, std::move(a_pwr_u), std::move(ln_a));

                    return createBinaryNode(NodeType::MULTIPLY, std::move(a_pwr_u_ori_ln_a), std::move(u_prim));
                }

                // Case 3: (u^v)'
                // Harder... so, for now we mark it as unknown or error:
                throw std::runtime_error("Derivatives of a function to the power of another function with respect to their variable are not supported");
            }

            default:
                throw std::runtime_error("Unknown node type!");
        }
        //currentdx = dx;
    }
private:
    /// Factory methods
    unique_ptr<Node> createConstantNode(double value) {
        return make_unique<Node>(value);
    }

    unique_ptr<Node> createBinaryNode(NodeType type, unique_ptr<Node> left, unique_ptr<Node> right) {
        return make_unique<Node>(type, std::move(left), std::move(right));
    }

    unique_ptr<Node> createUnaryNode(NodeType type, unique_ptr<Node> left) {
        return make_unique<Node>(type, std::move(left));
    }
};