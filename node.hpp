#pragma once

struct Node {
    Node() : parent(nullptr), num_keys(0) {}
    Node* parent;
    bool is_leaf;
    int num_keys;
};