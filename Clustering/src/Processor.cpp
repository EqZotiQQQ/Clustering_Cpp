#include "../headers/Processor.h"
#include "../headers/Dot.h"
#include "../headers/Centroid.h"

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"

Processor::Processor(int weight, int height, int cluster_cnt, const RUN_TYPE type):
            image(cv::Mat(weight, height, CV_8UC3, cv::Scalar(0,0,0))),
            cluster_cnt(cluster_cnt),
            run_type(run_type),
            clust_window_name(std::string("Clustering")) {
    centroids.reserve(cluster_cnt);
}

Processor::~Processor() {

}

void Processor::process() noexcept {
    bool lunch_status{ true };
    cv::namedWindow(clust_window_name, cv::WINDOW_AUTOSIZE);

    while (lunch_status) {  // main loop
        if (this->run_type == RUN_TYPE::STATIC) {
            static_process();
            lunch_status = false;
        }

        auto c = cv::waitKey(0);
        if (c == 27) {
            lunch_status = false;
        }
    }
}

void Processor::static_process() noexcept {
    dots.push_back({21,25});
    dots.push_back({8,50});
    dots.push_back({60,75});
    dots.push_back({32,45});
    dots.push_back({23,54});
    kmeans();
}

void Processor::show(const cv::Mat& img) const noexcept {
    cv::imshow(clust_window_name, img);
}

void Processor::print_dots() const noexcept {
    for (const auto& dot : dots) {
        dot.print();
    }
}

bool Processor::kmeans() noexcept {
    random_init_centroids();
    double epsilon = 1; // dunno tmp value.
    double difference = calculate_clusters();
    while (epsilon >= difference) { // main process loop

    }
}


void Processor::random_init_centroids() noexcept {

}

double Processor::calculate_clusters() noexcept {

}