#include "leaf_node.hpp"

class BPlusTree {
   public:
    BPlusTree();
    Record* search(int key) const;
    void insert(int key, int value);
    void remove(int key);
    void print_tree() const;
    void print_leaves() const;

   private:
    LeafNode* find_leaf(int key) const;
    LeafNode* start_new_tree(int key, Record* record);
    Node* delete_entry(Node* n, int key, void* pointer);
    void print_node(Node* n) const;
    Node* root;
};