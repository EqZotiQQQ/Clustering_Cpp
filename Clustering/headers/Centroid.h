#ifndef CENTROID_H
#define CENTROID_H
#include "Processor.h"
#include "Dot.h"
#include <vector>
#include <utility>


struct Centroid {
    Centroid(const int x, const int y, const cv::Scalar& color) :
        pos(std::make_pair(x, y)),
        color(color) {}
    std::pair<int, int> pos;
    std::vector<Dot*> cluster;   // list of elements linked with this centroid
    cv::Scalar color;
    void print() const {
        printf("Claster position:[%d %d]. Count of elements in cluster: %ld\n", pos.first, pos.second, cluster.size());
    }
};

#endif // CENTROID_H