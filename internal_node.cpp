#include "internal_node.hpp"

InternalNode::InternalNode() {
    keys.fill(-1);
    nodes.fill(nullptr);
    is_leaf = false;
}

InternalNode::InternalNode(Node* left, int key, Node* right) {
    keys.fill(-1);
    nodes.fill(nullptr);
    is_leaf = false;

    keys[0] = key;
    nodes[0] = left;
    nodes[1] = right;
    num_keys++;
    left->parent = this;
    right->parent = this;
}

void InternalNode::insert(int left_index, int key, Node* right) {
    for (int i = num_keys; i > left_index; i--) {
        nodes[i + 1] = nodes[i];
        keys[i] = keys[i - 1];
    }
    nodes[left_index + 1] = right;
    keys[left_index] = key;
    num_keys++;
    return;
}

Node* InternalNode::insert_after_splitting(int left_index, int key,
                                           Node* right) {
    std::array<Node*, ORDER + 1> temp_pointers;
    std::array<int, MAX_NUM_KEYS + 1> temp_keys;

    for (int i = 0, j = 0; i < num_keys + 1; i++, j++) {
        if (j == left_index + 1) j++;
        temp_pointers[j] = nodes[i];
    }
    for (int i = 0, j = 0; i < num_keys; i++, j++) {
        if (j == left_index) j++;
        temp_keys[j] = keys[i];
    }
    temp_pointers[left_index + 1] = right;
    temp_keys[left_index] = key;

    InternalNode* new_node = new InternalNode;
    num_keys = 0;
    keys.fill(-1);
    int i, j;
    for (i = 0; i < HALF_NUM_KEYS; i++) {
        nodes[i] = temp_pointers[i];
        keys[i] = temp_keys[i];
        num_keys++;
    }
    nodes[i] = temp_pointers[i];
    int k_prime = temp_keys[HALF_NUM_KEYS];
    for (++i, j = 0; i < ORDER; i++, j++) {
        new_node->nodes[j] = temp_pointers[i];
        new_node->keys[j] = temp_keys[i];
        new_node->num_keys++;
    }
    new_node->nodes[j] = temp_pointers[i];
    new_node->parent = parent;
    Node* child;
    for (i = 0; i <= new_node->num_keys; i++) {
        child = (Node*)new_node->nodes[i];
        child->parent = new_node;
    }

    return this->insert_in_parent(k_prime, new_node);
}

Node* InternalNode::insert_in_parent(int key, Node* right) {
    InternalNode* parent_n = static_cast<InternalNode*>(this->parent);
    if (!parent_n) return new InternalNode(this, key, right);

    int left_index = 0;
    while (left_index <= parent->num_keys &&
           parent_n->nodes[left_index] != this)
        left_index++;
    if (parent_n->num_keys < ORDER - 1) {
        parent_n->insert(left_index, key, right);
        return nullptr;
    }

    return parent_n->insert_after_splitting(left_index, key, right);
}