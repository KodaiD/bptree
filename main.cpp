#include <cassert>
#include <iostream>
#include <vector>

#include "bptree.hpp"

const int DATA_SIZE = 1000;

BPlusTree bptree;
std::vector<int> values;

void insertion_test() {
    for (int i = 0; i < DATA_SIZE; i++) {
        bptree.insert(i, i);
        values.emplace_back(i);
        std::cout << "insert: " << i << "\n\n";
        bptree.print_tree();
        std::cout << "==================" << std::endl;
    }
}

void search_test() {
    for (int i = 0; i < DATA_SIZE; i++) {
        Record* record = bptree.search(i);
        assert(record->value == i);
    }
}

void deletion_test() {
    for (int i = 0; i < DATA_SIZE; i++) {
        bptree.remove(values[i]);
        std::cout << "delete: " << values[i] << "\n\n";
        bptree.print_tree();
        std::cout << "==================" << std::endl;
    }
}

int main() {
    values.reserve(DATA_SIZE);

    insertion_test();
    search_test();
    bptree.print_leaves();
    deletion_test();

    return 0;
}