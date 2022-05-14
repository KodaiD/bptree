#include <cassert>
#include <iostream>
#include <vector>

#include "bptree.hpp"

const int DATA_SIZE = 10;

int main() {
    BPlusTree bptree;
    std::vector<int> values;
    values.reserve(DATA_SIZE);

    for (int i = 0; i < DATA_SIZE; i++) {
        bptree.insert(i, i);
        values.emplace_back(i);
        std::cout << "insert: " << i << "\n\n";
        bptree.print_tree();
        std::cout << "==================" << std::endl;
    }

    for (int i = 0; i < DATA_SIZE; i++) {
        Record* record = bptree.search(i);
        assert(record->value == i);
    }

    for (int i = 0; i < DATA_SIZE; i++) {
        bptree.remove(values[i]);
        std::cout << "delete: " << values[i] << "\n\n";
        bptree.print_tree();
        std::cout << "==================" << std::endl;
    }

    return 0;
}