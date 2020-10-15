#include "../headers/Processor.h"
#include "../headers/Dot.h"
#include "../headers/Centroid.h"

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"

#include <numeric>
#include <random>


Processor::Processor(const int rows,
                const int columns,
                const int cluster_cnt,
                const RUN_TYPE type,
                const std::string& window_name):
    image(cv::Mat(rows, columns, CV_8UC3, cv::Scalar(0, 0, 0))),
    cluster_cnt(cluster_cnt),
    run_type(type),
    clust_window_name(window_name)
{
}

Processor::~Processor() {

}

void Processor::process() noexcept {
    std::thread worker;
    bool lunch_status{ true };
    cv::namedWindow(clust_window_name, cv::WINDOW_AUTOSIZE);
    cv::imshow(clust_window_name, image);

    while (lunch_status) {  // main loop
        std::cout << "worker" << (int)run_type << std::endl;
        if (run_type == RUN_TYPE::LATENCY_FLOW) {
            worker = std::thread([this] {this->latency_flow(); });
            lunch_status = false;
        }
        auto c = cv::waitKey(0);
        if (c == 27) {
            lunch_status = false;
            if (worker.joinable()) {
                worker.detach();
            }
        }
    }
}

void Processor::latency_flow() noexcept {
    std::random_device rand_dev;
    std::mt19937 gen(rand_dev());
    std::uniform_int_distribution<> rand_rows(0, image.cols);
    std::uniform_int_distribution<> rand_cols(0, image.rows);
    for (int i = 0; i < 500; ++i) {
        int x = rand_rows(gen);
        int y = rand_cols(gen);
        if (std::find_if(dots.cbegin(), dots.cend(), [x = x, y = y](const Dot & dot) {return dot.pos.first == x && dot.pos.second == y; }) != dots.cend()) {
            i--;
            std::cout << "skip\n";
            continue;
        }
        dots.push_back({ x, y });
    }
    kmeans();
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
    double centroid_offset = 500;
    calculate_distances();
    while (epsilon <= centroid_offset) { // main process loop
        image = cv::Mat(image.rows, image.cols, CV_8UC3, cv::Scalar(0, 0, 0));
        printf("offset = %f\n", centroid_offset);
        centroid_offset = update_centroids();
        calculate_distances();
        print_centoids_pos();
        print_connections();
        draw_elements();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        print_connections();
        draw_elements();
        cv::imshow(clust_window_name, image);
    }
    printf("offset = %f\n", centroid_offset);

    return true;
}

double Processor::update_centroids() noexcept {
    int offset = 0;
    for (auto iter = centroids.begin(); iter != centroids.end(); ++iter) {
        int x = 0, y = 0;
        for (auto clust = iter->cluster.begin(); clust != iter->cluster.end(); ++clust) {
            x += (*clust)->pos.first;
            y += (*clust)->pos.second;
        }
        x /= iter->cluster.size();
        y /= iter->cluster.size();
        offset += abs(iter->pos.first - x) + abs(iter->pos.second - y);
        iter->pos = std::make_pair(x, y);
    }
    return offset;
}

void Processor::random_init_centroids() noexcept {
    std::random_device rand_dev;
    std::mt19937 gen(rand_dev());
    std::uniform_int_distribution<> rand_rows(0, image.cols);
    std::uniform_int_distribution<> rand_cols(0, image.rows);
    for (int i = 0; i < cluster_cnt; ++i) {
        int x = rand_rows(gen);
        int y = rand_cols(gen);
        if (std::find_if(centroids.cbegin(), centroids.cend(), [x = x, y = y](const Centroid & cent) {return cent.pos.first == x && cent.pos.second == y; }) != centroids.cend()) {
            i--;
            std::cout << "skip\n";
            continue;
        }
        dots.push_back({ x, y });

        centroids.push_back({ rand_rows(gen), rand_cols(gen) });
    }
}

void Processor::calculate_distances() noexcept {
    for (auto it_centr = centroids.begin(); it_centr != centroids.end(); ++it_centr) {
        it_centr->cluster.clear();
    }
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
        centroid->cluster.push_back(&(*it_dot));
    }
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