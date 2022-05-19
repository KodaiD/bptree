#include "leaf_node.hpp"

#include "internal_node.hpp"

LeafNode::LeafNode() : next(nullptr) {
    records.fill(nullptr);
    is_leaf = true;
}

void LeafNode::insert(int key, Record* record) {
    int insertion_point = 0;
    while (insertion_point < num_keys && records[insertion_point]->key < key)
        insertion_point++;

    for (int i = num_keys; i > insertion_point; i--) {
        records[i] = records[i - 1];
    }
    records[insertion_point] = record;
    num_keys++;
}

Node* LeafNode::insert_after_splitting(int key, Record* record) {
    LeafNode* new_leaf = new LeafNode;
    std::array<Record*, ORDER> temp_records;

    int insertion_index = 0;
    while (insertion_index < ORDER - 1 && records[insertion_index]->key < key)
        insertion_index++;

    for (int i = 0, j = 0; i < num_keys; i++, j++) {
        if (j == insertion_index) j++;
        temp_records[j] = records[i];
    }
    temp_records[insertion_index] = record;

    records.fill(nullptr);
    num_keys = 0;

    for (int i = 0, j = 0; i < MAX_NUM_KEYS + 1; i++) {
        if (i <= HALF_NUM_KEYS) {
            records[i] = temp_records[i];
            num_keys++;
        } else {
            new_leaf->records[j] = temp_records[i];
            new_leaf->num_keys++;
            j++;
        }
    }
    new_leaf->next = next;
    next = new_leaf;

    new_leaf->parent = parent;
    int new_key = new_leaf->records[0]->key;

    return insert_in_parent(new_key, new_leaf);
}

Node* LeafNode::insert_in_parent(int key, Node* right) {
    if (!parent) return new InternalNode(this, key, right);
    InternalNode* parent_n = static_cast<InternalNode*>(parent);

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