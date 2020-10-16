#include "../headers/Processor.h"
#include "../headers/Dot.h"
#include "../headers/Centroid.h"

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"

#include <numeric>
#include <random>
#include <algorithm>


Processor::Processor(const int rows, const int columns, const int cluster_cnt, const RUN_TYPE type, const std::string& window_name):
                                image(cv::Mat(rows, columns, CV_8UC3, cv::Scalar(0, 0, 0))),
                                cluster_cnt(cluster_cnt),
                                run_type(type),
                                clust_window_name(window_name) {}

Processor::~Processor() {}

void Processor::launch() noexcept {
    std::thread worker;
    bool lunch_status{ true };
    cv::namedWindow(clust_window_name, cv::WINDOW_AUTOSIZE);
    cv::imshow(clust_window_name, image);

    while (lunch_status) {  // main loop
        std::cout << "worker" << (int)run_type << std::endl;
        if (run_type == RUN_TYPE::LATENCY_FLOW) {
            worker = std::thread([this] {this->latency_flow(); });
            lunch_status = false;
        } else if (run_type == RUN_TYPE::REAL_TIME) {
            cv::setMouseCallback(clust_window_name, s_mouse_callback, this);
        } else if (run_type == RUN_TYPE::STATIC) {
            worker = std::thread([this] {this->static_flow(); });
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

void Processor::s_mouse_callback(int event, int x, int y, int flags, void* param) noexcept {    /*param - image*/
    auto graph_processor = static_cast<Processor*>(param);
    if (event == cv::EVENT_LBUTTONDOWN) {
        graph_processor->process_realtime(x, y);
    }
}

void Processor::process_realtime(const int x, const int y) noexcept {
    if (std::find_if(dots.cbegin(), dots.cend(), [x = x, y = y](const Dot & dot) {return dot.pos.first == x && dot.pos.second == y; }) != dots.cend()) {
        return;
    }
    dots.push_back({ x, y });
    std::cout << dots.size() << std::endl;
    draw_elements();
    if (dots.size() <= cluster_cnt) {
        return;
    }
    kmeans();
}


void Processor::latency_flow() noexcept {
    random_init_centroids();
    std::random_device rand_dev;
    std::mt19937 gen(rand_dev());
    std::uniform_int_distribution<> rand_rows(0, image.cols);
    std::uniform_int_distribution<> rand_cols(0, image.rows);
    for (int i = 0; i < 50; ++i) {
        int x = rand_rows(gen);
        int y = rand_cols(gen);
        if (std::find_if(dots.cbegin(), dots.cend(), [x = x, y = y](const Dot & dot) {return dot.pos.first == x && dot.pos.second == y; }) != dots.cend()) {
            i--;
            std::cout << "skip\n";
            continue;
        }
        dots.push_back({ x, y });
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        if (dots.size() <= clusters.size()) {
            continue;
        }
        kmeans_realtime();
    }
}

bool Processor::kmeans_realtime() noexcept {
    double epsilon = 1;
    double centroid_offset;
    link_cluster_and_elements();

    do {
        image = cv::Mat(image.rows, image.cols, CV_8UC3, cv::Scalar(0, 0, 0));
        centroid_offset = update_centroids();
        printf("offset = %f\n", centroid_offset);
        link_cluster_and_elements();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        draw_connections();
        draw_elements();
        cv::imshow(clust_window_name, image);
    } while (epsilon <= centroid_offset);
    printf("offset = %f\n", centroid_offset);
    return true;
}

void Processor::static_flow() noexcept {
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

bool Processor::kmeans() noexcept {
    random_init_centroids();
    double epsilon = 1; // dunno tmp value.
    double centroid_offset = -1;
    link_cluster_and_elements();
    while (epsilon <= centroid_offset || centroid_offset == -1) { // main process loop
        image = cv::Mat(image.rows, image.cols, CV_8UC3, cv::Scalar(0, 0, 0));
        printf("offset = %f\n", centroid_offset);
        centroid_offset = update_centroids();
        link_cluster_and_elements();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        draw_connections();
        draw_elements();
        cv::imshow(clust_window_name, image);
    }
    printf("offset = %f\n", centroid_offset);
    return true;
}

double Processor::update_centroids() noexcept {
    int offset = 0;
    for (auto iter = clusters.begin(); iter != clusters.end(); ++iter) {
        int x = 0, y = 0;
        for (auto clust = iter->cluster.cbegin(); clust != iter->cluster.cend(); ++clust) {
            x += (*clust)->pos.first;
            y += (*clust)->pos.second;
        }
        print_dots_stats();
        print_cluster_stats();
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
        if (std::find_if(clusters.cbegin(), clusters.cend(), [x = x, y = y](const Centroid& cent) {return cent.pos.first == x && cent.pos.second == y; }) != clusters.cend()) {
            i--;
            std::cout << "skip\n";
            continue;
        }
        clusters.push_back({ rand_rows(gen), rand_cols(gen) });
    }
}

void Processor::link_cluster_and_elements() noexcept {
    for (auto it_centr = clusters.begin(); it_centr != clusters.end(); ++it_centr) {
        it_centr->cluster.clear();
    }
    for (auto it_dot = dots.begin(); it_dot != dots.end(); ++it_dot) {
        double min_distance = -1;
        Centroid* centroid = nullptr;
        for (auto it_centr = clusters.begin(); it_centr != clusters.end(); ++it_centr) {
            double distance = std::sqrt(std::pow(it_centr->pos.first - it_dot->pos.first, 2) + std::pow(it_centr->pos.second - it_dot->pos.second, 2));
            if ((min_distance == -1 || min_distance > distance)
                && ((it_centr->cluster.size() == 0) || (it_centr->cluster.size() < dots.size()))) {
                centroid = &(*it_centr);
                min_distance = distance;
            }
        }
        it_dot->owner = centroid;
        it_dot->centr_dist = min_distance;
        centroid->cluster.push_back(&(*it_dot));
    }
    /*Make USSR Great again. Joke just its bad when cluster contains 0 elems.*/
    for (auto centroid = clusters.begin(); centroid != clusters.end(); ++centroid) {
        if (centroid->cluster.size() == 0) {
            double min_distance = -1;
            Dot* move_dot = nullptr;
            for (auto it_dot = dots.begin(); it_dot != dots.end(); ++it_dot) {
                double distance = std::sqrt(std::pow(centroid->pos.first - it_dot->pos.first, 2) + std::pow(centroid->pos.second - it_dot->pos.second, 2));
                if ((min_distance == -1 || min_distance > distance) && (it_dot->owner->cluster.size() > 1)) {
                    move_dot = &(*it_dot);
                    min_distance = distance;
                }
            }
            Centroid* prev_owner = move_dot->owner;
            move_dot->owner = &(*centroid);
            auto it = std::find(prev_owner->cluster.begin(), prev_owner->cluster.end(), &*move_dot);
            prev_owner->cluster.erase(it);
            centroid->cluster.push_back(move_dot);
        }
    }
}

void Processor::draw_elements() const noexcept {
    for (const auto& dot : dots) {
        cv::circle(image, cv::Point(dot.pos.first, dot.pos.second), 3, cv::Scalar(200, 250, 200), -1, cv::LINE_AA);
    }
    for (const auto& centroid : clusters) {
        cv::circle(image, cv::Point(centroid.pos.first, centroid.pos.second), 3, cv::Scalar(0, 0, 250), -1, cv::LINE_AA);
    }
    cv::imshow(clust_window_name, image);
}

void Processor::draw_connections() noexcept {
    for (const auto& i : dots) {
        cv::line(image, cv::Point(i.owner->pos.first, i.owner->pos.second), cv::Point(i.pos.first, i.pos.second), cv::Scalar(80, 80, 80), 2, cv::LINE_4);
    }
}

void Processor::print_dots_stats() const noexcept {
    for (const auto& dot : dots) {
        dot.print();
    }
}

void Processor::print_cluster_stats() const noexcept {
    for (const auto& centoid : clusters) {
        centoid.print();
    }
}