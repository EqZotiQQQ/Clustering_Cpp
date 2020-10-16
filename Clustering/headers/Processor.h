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
        const int rows = 400, 
        const int columns = 700, 
        const int cluster_cnt = 5, 
        const RUN_TYPE type = RUN_TYPE::LATENCY_FLOW, 
        const std::string& window_name = "Clustering"
    );
    ~Processor();
    void launch() noexcept;
private:
    void latency_flow() noexcept;
    void static_flow() noexcept;
    static void s_mouse_callback(int event, int x, int y, int flags, void* param) noexcept;
    bool kmeans() noexcept;
    bool kmeans_realtime() noexcept;
    void random_init_centroids() noexcept;
    void link_cluster_and_elements() noexcept;
    void draw_elements() const noexcept;
    void draw_connections() noexcept;
    double update_centroids() noexcept;
    void process_realtime(const int x, const int y) noexcept;

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