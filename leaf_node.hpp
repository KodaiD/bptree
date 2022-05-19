#pragma once

#include <array>

#include "definition.hpp"
#include "node.hpp"
#include "record.hpp"

struct LeafNode : public Node {
    LeafNode();
    void insert(int key, Record* record);
    Node* insert_after_splitting(int key, Record* record);
    Node* insert_in_parent(int key, Node* right);

    std::array<Record*, ORDER - 1> records;
    LeafNode* next;
};