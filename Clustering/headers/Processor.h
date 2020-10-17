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
    REAL_TIME = 0,
    LATENCY_FLOW = 1,
    STATIC = 2
};

class Processor {
public:
    Processor(
        const int rows, 
        const int columns, 
        const int cluster_cnt, 
        const RUN_TYPE type,
        const std::string& window_name
    );
    ~Processor();
    void launch() noexcept;
private:
    void latency_flow(const int x, const int y) noexcept;
    void static_flow() noexcept;
    static void s_mouse_callback(int event, int x, int y, int flags, void* param) noexcept;
    bool kmeans_realtime() noexcept;
    void random_init_centroids() noexcept;
    void link_cluster_and_elements() noexcept;
    void draw_elements() const noexcept;
    void draw_connections() noexcept;
    double update_centroids() noexcept;
    void dot_generator() noexcept;

    void print_dots_stats() const noexcept;
    void print_cluster_stats() const noexcept;

    const std::string clust_window_name;
    cv::Mat image;
    std::vector<Centroid> clusters;
    std::vector<Dot> dots;
    RUN_TYPE run_type;
    int cluster_cnt;
};

#endif // PROCESSOR_H