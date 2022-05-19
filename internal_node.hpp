#pragma once

#include <array>

#include "definition.hpp"
#include "node.hpp"

struct InternalNode : public Node {
    InternalNode();
    InternalNode(Node* left, int key, Node* right);
    void insert(int left_index, int key, Node* right);
    Node* insert_after_splitting(int left_index, int key, Node* right);
    Node* insert_in_parent(int key, Node* right);

    std::array<int, ORDER - 1> keys;
    std::array<Node*, ORDER> nodes;
};