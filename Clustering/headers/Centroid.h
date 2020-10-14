#ifndef CENTROID_H
#define CENTROID_H
#include "Processor.h"
#include "Dot.h"

#include <vector>
#include <utility>


struct Centroid {
    Centroid(int x, int y) :
        centroid_pos(std::make_pair(x, y)) {}
    std::pair<int, int> centroid_pos;
    std::vector<std::pair<int, int>> cluster;   // list of elements linked with this centroid
};

#endif CENTROID_H