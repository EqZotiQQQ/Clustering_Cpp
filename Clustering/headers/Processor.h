#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <utility>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp>

#include "Centroid.h"
#include "Dot.h"


enum class RUN_TYPE {
    STATIC = 0,     // place elements staticly 
    LATENCY_FLOW = 1,    // u should place elements   
    RANDOM = 2      // leave elements be automaticaly placed on the fields
};

class Processor {
public:
    Processor(const int rows = 1000,
                    const int columns = 1400,
                    const int cluster_cnt = 20,
                    const RUN_TYPE type = RUN_TYPE::LATENCY_FLOW,
                    const std::string& window_name = "Clustering");
    ~Processor();
    void process() noexcept;
private:
    void latency_flow() noexcept;
    void print_dots() const noexcept;
    void print_centoids_pos() const noexcept;
    bool kmeans() noexcept;
    void random_init_centroids() noexcept;
    void calculate_distances() noexcept;
    void draw_elements() const noexcept;
    void print_connections() noexcept;
    double update_centroids() noexcept;

    const std::string clust_window_name;
    cv::Mat image;
    std::vector<Centroid> centroids;
    std::vector<Dot> dots;
    RUN_TYPE run_type;
    int cluster_cnt;
};

#endif // PROCESSOR_H