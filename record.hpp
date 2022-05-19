#pragma once

struct Record {
    Record(int key, int value) : key(key), value(value) {}
    int key;
    int value;
};