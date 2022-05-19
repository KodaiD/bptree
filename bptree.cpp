#include "bptree.hpp"

#include <iostream>

#include "internal_node.hpp"
#include "leaf_node.hpp"

BPlusTree::BPlusTree() { root = nullptr; }

Record* BPlusTree::search(int key) const {
    if (!root) {
        return nullptr;
    }
    LeafNode* leaf = find_leaf(key);

    int i = 0;
    while (i < leaf->num_keys && leaf->records[i]->key != key) i++;
    if (i == leaf->num_keys)
        return nullptr;
    else
        return (Record*)leaf->records[i];
}

void BPlusTree::insert(int key, int value) {
    Record* record = search(key);
    if (record) {
        record->value = value;
        return;
    }

    record = new Record(key, value);
    if (!root) {
        root = start_new_tree(key, record);
        return;
    }
    LeafNode* leaf = find_leaf(key);

    if (leaf->num_keys < MAX_NUM_KEYS) {
        leaf->insert(key, record);
        return;
    }
    Node* new_root = leaf->insert_after_splitting(key, record);
    if (new_root) root = new_root;
    return;
}

void BPlusTree::remove(int key) {
    Record* key_record = search(key);
    Node* leaf = find_leaf(key);
    if (key_record && leaf) {
        root = delete_entry(leaf, key, key_record);
    }
    return;
}

void BPlusTree::print_tree() const {
    if (!root) return;
    print_node(root);
    std::cout << std::endl;
}

void BPlusTree::print_leaves() const {
    if (!root) return;
    Node* cur = root;
    while (!cur->is_leaf) {
        InternalNode* nonleaf = static_cast<InternalNode*>(cur);
        cur = nonleaf->nodes[0];
    }
    LeafNode* cur_leaf = static_cast<LeafNode*>(cur);
    while (cur_leaf) {
        for (int i = 0; i < cur_leaf->num_keys; i++)
            std::cout << cur_leaf->records[i]->value << " ";
        std::cout << std::endl;
        cur_leaf = cur_leaf->next;
    }
}

LeafNode* BPlusTree::find_leaf(int key) const {
    if (!root) return nullptr;

    Node* cur = root;
    while (!cur->is_leaf) {
        InternalNode* nonleaf = static_cast<InternalNode*>(cur);
        int i = 0;
        while (i < cur->num_keys) {
            if (key >= nonleaf->keys[i])
                i++;
            else
                break;
        }
        cur = nonleaf->nodes[i];
    }
    return static_cast<LeafNode*>(cur);
}

LeafNode* BPlusTree::start_new_tree(int key, Record* record) {
    LeafNode* root = new LeafNode;
    root->records[0] = record;
    root->num_keys++;
    return root;
}

Node* BPlusTree::delete_entry(Node* n, int key, void* p) {
    // delete from node
    if (!n->is_leaf) {
        InternalNode* nonleaf = static_cast<InternalNode*>(n);
        Node* node = static_cast<Node*>(p);
        int i = 0;
        while (i < ORDER && nonleaf->keys[i] != key) i++;
        for (++i; i < nonleaf->num_keys; i++)
            nonleaf->keys[i - 1] = nonleaf->keys[i];

        int num_pointers = nonleaf->num_keys + 1;
        i = 0;
        while (nonleaf->nodes[i] != node) i++;
        for (++i; i < num_pointers; i++)
            nonleaf->nodes[i - 1] = nonleaf->nodes[i];

        n->num_keys--;
        for (int i = nonleaf->num_keys + 1; i < ORDER; i++)
            nonleaf->nodes[i] = nullptr;
    } else {
        LeafNode* leaf = static_cast<LeafNode*>(n);
        int i = 0;
        while (i < MAX_NUM_KEYS && leaf->records[i]->key != key) i++;
        for (++i; i < leaf->num_keys; i++)
            leaf->records[i - 1] = leaf->records[i];
        leaf->num_keys--;
        for (int i = leaf->num_keys; i < MAX_NUM_KEYS; i++)
            leaf->records[i] = nullptr;
    }

    if (n == root) {
        if (root->num_keys > 0) return root;
        if (!root->is_leaf) {
            InternalNode* root_n = static_cast<InternalNode*>(root);
            Node* new_root = root_n->nodes[0];
            new_root->parent = nullptr;
            return new_root;
        }
        delete root;
        return nullptr;
    }

    if (n->num_keys < HALF_NUM_KEYS) {
        int neighbor_index = ORDER;
        for (int i = 0; i <= n->parent->num_keys; i++)
            if (static_cast<InternalNode*>(n->parent)->nodes[i] == n)
                neighbor_index = i - 1;
        if (neighbor_index == ORDER) exit(1);

        int k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;

        InternalNode* parent_n = static_cast<InternalNode*>(n->parent);

        int k_prime = parent_n->keys[k_prime_index];
        Node* n_prime = neighbor_index == -1 ? parent_n->nodes[1]
                                             : parent_n->nodes[neighbor_index];
        int capacity = n->is_leaf ? ORDER : ORDER - 1;

        if (n_prime->num_keys + n->num_keys < capacity) {
            // coalesce
            if (!n->is_leaf) {
                InternalNode* nonleaf = static_cast<InternalNode*>(n);
                InternalNode* nonleaf_prime =
                    static_cast<InternalNode*>(n_prime);
                InternalNode* tmp;
                if (neighbor_index == -1) {
                    tmp = nonleaf;
                    nonleaf = nonleaf_prime;
                    nonleaf_prime = tmp;
                }
                int neighbor_insertion_index = nonleaf_prime->num_keys;
                nonleaf_prime->keys[neighbor_insertion_index] = k_prime;
                nonleaf_prime->num_keys++;
                int n_end = nonleaf->num_keys;
                int i, j;
                for (i = neighbor_insertion_index + 1, j = 0; j < n_end;
                     i++, j++) {
                    nonleaf_prime->keys[i] = nonleaf->keys[j];
                    nonleaf_prime->nodes[i] = nonleaf->nodes[j];
                    nonleaf_prime->num_keys++;
                    nonleaf->num_keys--;
                }
                nonleaf_prime->nodes[i] = nonleaf->nodes[j];
                for (i = 0; i < nonleaf_prime->num_keys + 1; i++) {
                    nonleaf_prime->nodes[i]->parent = nonleaf_prime;
                }
                root = delete_entry(nonleaf->parent, k_prime, nonleaf);
                delete nonleaf;
                return root;
            } else {
                LeafNode* leaf = static_cast<LeafNode*>(n);
                LeafNode* leaf_prime = static_cast<LeafNode*>(n_prime);
                LeafNode* tmp;
                if (neighbor_index == -1) {
                    tmp = leaf;
                    leaf = leaf_prime;
                    leaf_prime = tmp;
                }
                int neighbor_insertion_index = leaf_prime->num_keys;
                for (int i = neighbor_insertion_index, j = 0;
                     j < leaf->num_keys; i++, j++) {
                    leaf_prime->records[i] = leaf->records[j];
                    leaf_prime->num_keys++;
                }
                leaf_prime->next = leaf->next;
                root = delete_entry(leaf->parent, k_prime, leaf);
                delete leaf;
                return root;
            }
        } else {
            // redistribute
            if (!n->is_leaf) {
                InternalNode* nonleaf = static_cast<InternalNode*>(n);
                InternalNode* nonleaf_prime =
                    static_cast<InternalNode*>(n_prime);
                if (neighbor_index != -1) {
                    for (int i = nonleaf->num_keys + 1; i > 0; i--) {
                        nonleaf->keys[i] = nonleaf->keys[i - 1];
                        nonleaf->nodes[i] = nonleaf->nodes[i - 1];
                    }
                    nonleaf->nodes[0] =
                        nonleaf_prime->nodes[nonleaf_prime->num_keys];
                    nonleaf->nodes[0]->parent = nonleaf;
                    nonleaf_prime->nodes[nonleaf_prime->num_keys] = nullptr;
                    nonleaf->keys[0] = k_prime;
                    static_cast<InternalNode*>(nonleaf->parent)
                        ->keys[k_prime_index] =
                        nonleaf_prime->keys[nonleaf_prime->num_keys - 1];
                } else {
                    nonleaf->keys[nonleaf->num_keys] = k_prime;
                    nonleaf->nodes[nonleaf->num_keys + 1] =
                        nonleaf_prime->nodes[0];
                    nonleaf->nodes[nonleaf->num_keys + 1]->parent = nonleaf;
                    static_cast<InternalNode*>(nonleaf->parent)
                        ->keys[k_prime_index] = nonleaf_prime->keys[0];
                    for (int i = 0; i < nonleaf_prime->num_keys - 1; i++) {
                        nonleaf_prime->keys[i] = nonleaf_prime->keys[i + 1];
                        nonleaf_prime->nodes[i] = nonleaf_prime->nodes[i + 1];
                    }
                    nonleaf_prime->nodes[nonleaf_prime->num_keys - 1] =
                        nonleaf_prime->nodes[nonleaf_prime->num_keys];
                }
                nonleaf->num_keys++;
                nonleaf_prime->num_keys--;
                return root;
            } else {
                LeafNode* leaf = static_cast<LeafNode*>(n);
                LeafNode* leaf_prime = static_cast<LeafNode*>(n_prime);
                int i;
                if (neighbor_index != -1) {
                    for (i = n->num_keys; i > 0; i--) {
                        leaf->records[i] = leaf->records[i - 1];
                    }
                    leaf->records[0] =
                        leaf_prime->records[leaf_prime->num_keys - 1];
                    leaf_prime->records[leaf_prime->num_keys - 1] = nullptr;
                    static_cast<InternalNode*>(leaf->parent)
                        ->keys[k_prime_index] = leaf->records[0]->key;
                } else {
                    leaf->records[n->num_keys] = leaf_prime->records[0];
                    static_cast<InternalNode*>(leaf->parent)
                        ->keys[k_prime_index] = leaf_prime->records[1]->key;
                    for (i = 0; i < leaf_prime->num_keys - 1; i++) {
                        leaf_prime->records[i] = leaf_prime->records[i + 1];
                    }
                }
                leaf->num_keys++;
                leaf_prime->num_keys--;
                return root;
            }
        }
    }
    return root;
}

void BPlusTree::print_node(Node* n) const {
    if (!n) return;
    if (!n->is_leaf) {
        InternalNode* nonleaf = static_cast<InternalNode*>(n);
        for (int i = 0; i <= n->num_keys; i++) {
            print_node(nonleaf->nodes[i]);
        }
        for (int i = 0; i < n->num_keys; i++) {
            std::cout << nonleaf->keys[i] << " ";
        }
        std::cout << std::endl;
    } else {
        LeafNode* leaf = static_cast<LeafNode*>(n);
        std::cout << "leaf: ";
        for (int i = 0; i < n->num_keys; i++) {
            std::cout << leaf->records[i]->key << " ";
        }
        std::cout << std::endl;
    }
}