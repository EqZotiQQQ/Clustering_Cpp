#ifndef CENTROID_H
#define CENTROID_H
#include "Processor.h"
#include "Dot.h"

#include <vector>
#include <utility>


struct Centroid {
    Centroid(int x, int y) :
        pos(std::make_pair(x, y)) {}
    std::pair<int, int> pos;
    std::vector<Dot*> cluster;   // list of elements linked with this centroid
    void print() const {
        printf("Claster position:[%d %d]. Count of elements in cluster: %d\n", pos.first, pos.second, cluster.size());
    }
};

#endif CENTROID_H