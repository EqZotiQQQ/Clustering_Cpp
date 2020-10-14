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
    DYNAMIC = 1,    // u should place elements   
    RANDOM = 2      // leave elements be automaticaly placed on the fields
};

class Processor {
public:
    Processor(const int weight = 300,
                    const int height = 200,
                    const int cluster_cnt = 3,
                    const RUN_TYPE type = RUN_TYPE::STATIC,
                    const std::string& window_name = "Clustering");
    ~Processor();
    void process() noexcept;
private:
    void static_process() noexcept;
    void show(const cv::Mat& img) const noexcept;
    void print_dots() const noexcept;
    void print_centoids_pos() const noexcept;
    bool kmeans() noexcept;
    void random_init_centroids() noexcept;
    double calculate_distances() noexcept;
    void draw_elements() const noexcept;
    void print_connections() noexcept;
    void update_centroids() noexcept;

    const std::string clust_window_name;
    cv::Mat image;
    std::vector<Centroid> centroids;
    std::vector<Dot> dots;
    RUN_TYPE run_type;
    int cluster_cnt;
};

#endif // PROCESSOR_H