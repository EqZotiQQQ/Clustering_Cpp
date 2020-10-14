#ifndef DOT_H
#define DOT_H

#include "Centroid.h"
#include "Processor.h"

#include <iostream>
#include <memory>
#include <utility>

struct Dot {
    Dot(const int x, const int y) :
            pos(std::make_pair(x, y)),
            owner(nullptr) {}
    std::pair<int, int> pos;
    struct Centroid* owner;
    double centr_dist;
    void print() const {
        if (owner) {
            printf("[%d %d], %s", pos.first, pos.second, "Owner exists\n");
        }
        else {
            printf("[%d %d], %s", pos.first, pos.second, "Owner doesn't exist\n");
        }
    }
};

#endif DOT_H