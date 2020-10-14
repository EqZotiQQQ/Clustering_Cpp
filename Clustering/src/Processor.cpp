#include "../headers/Processor.h"
#include "../headers/Dot.h"
#include "../headers/Centroid.h"

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"

#include <numeric>


Processor::Processor(const int cols,
                const int rows,
                const int cluster_cnt,
                const RUN_TYPE type,
                const std::string& window_name):
    image(cv::Mat(cols, rows, CV_8UC3, cv::Scalar(0, 0, 0))),
    cluster_cnt(cluster_cnt),
    run_type(run_type),
    clust_window_name(window_name)
{
}

Processor::~Processor() {

}

void Processor::process() noexcept {
    bool lunch_status{ true };
    cv::namedWindow(clust_window_name, cv::WINDOW_AUTOSIZE);
    show(image);

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
    for (int i = 0; i < 50; ++i) {
        dots.push_back({ rand() % image.cols - 10, rand() % image.rows - 10 });
    }
    kmeans();
    print_connections();
    draw_elements();
    show(image);
}

void Processor::show(const cv::Mat& img) const noexcept {
    cv::imshow(clust_window_name, img);
}

void Processor::print_dots() const noexcept {
    for (const auto& dot : dots) {
        dot.print();
    }
}

void Processor::print_centoids_pos() const noexcept {
    for (const auto& centoid : centroids) {
        centoid.print();
    }
}

bool Processor::kmeans() noexcept {
    random_init_centroids();
    double epsilon = 1; // dunno tmp value.
    double difference = calculate_distances();
    return true;
    while (epsilon >= difference) { // main process loop
        update_centroids();
        difference = calculate_distances();
    }
}

void Processor::update_centroids() noexcept {
    int x = 0, y = 0;
    for (auto iter = centroids.begin(); iter != centroids.end(); ++iter) {
        for (auto clust = iter->cluster.begin(); clust != iter->cluster.end(); ++clust) {
            
        }
    }
}

void Processor::random_init_centroids() noexcept {
    for (int i = 0; i < cluster_cnt; ++i) {
        centroids.push_back({ rand() % image.cols - 10, rand() % image.rows - 10 });
    }
}

double Processor::calculate_distances() noexcept {
    for (auto it_dot = dots.begin(); it_dot != dots.end(); ++it_dot) {
        double min_distance = -1;
        Centroid* centroid = nullptr;
        for (auto it_centr = centroids.begin(); it_centr != centroids.end(); ++it_centr) {
            double distance = std::sqrt(std::pow(it_centr->pos.first - it_dot->pos.first, 2) + std::pow(it_centr->pos.second - it_dot->pos.second, 2));
            if (min_distance == -1 || min_distance > distance) {
                centroid = &(*it_centr);
                min_distance = distance;
            }
        }
        it_dot->owner = centroid;
        it_dot->centr_dist = min_distance;
        centroid->cluster.push_back(it_dot->pos);
    }
    return 0;   // should return centoid offset from previous position
}

void Processor::draw_elements() const noexcept {
    for (const auto& dot : dots) {
        cv::circle(image, cv::Point(dot.pos.first, dot.pos.second), 3, cv::Scalar(200, 250, 200), -1, cv::LINE_AA);
    }
    for (const auto& centroid : centroids) {
        cv::circle(image, cv::Point(centroid.pos.first, centroid.pos.second), 3, cv::Scalar(0, 0, 250), -1, cv::LINE_AA);

    }
}

void Processor::print_connections() noexcept {
    for (const auto& i : dots) {
        cv::line(image, cv::Point(i.owner->pos.first, i.owner->pos.second), cv::Point(i.pos.first, i.pos.second), cv::Scalar(80, 80, 80), 2, cv::LINE_4);
    }
}