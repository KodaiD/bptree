#include <array>

constexpr int ORDER = 4;
constexpr int MAX_NUM_KEYS = ORDER - 1;
constexpr int HALF_NUM_KEYS = MAX_NUM_KEYS / 2;

struct Record {
    Record(int key, int value) : key(key), value(value) {}
    int key;
    int value;
};

struct Node {
    Node() : parent(nullptr), num_keys(0) {}
    Node* parent;
    bool is_leaf;
    int num_keys;
};

struct LeafNode : public Node {
    LeafNode() : next(nullptr) {
        records.fill(nullptr);
        is_leaf = true;
    }
    void insert(int key, Record* record);
    Node* insert_after_splitting(int key, Record* record);
    Node* insert_in_parent(int key, Node* right);

    std::array<Record*, ORDER - 1> records;
    LeafNode* next;
};

struct InternalNode : public Node {
    InternalNode() {
        keys.fill(-1);
        nodes.fill(nullptr);
        is_leaf = false;
    }
    InternalNode(Node* left, int key, Node* right) {
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
    void insert(int left_index, int key, Node* right);
    Node* insert_after_splitting(int left_index, int key, Node* right);
    Node* insert_in_parent(int key, Node* right);

    std::array<int, ORDER - 1> keys;
    std::array<Node*, ORDER> nodes;
};

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