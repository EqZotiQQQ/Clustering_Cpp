#include "headers/Processor.h"

void print_help();

int main(int argc, char* argv[]) {

    RUN_TYPE rt{ RUN_TYPE::LATENCY_FLOW };
    int clusters{ 4 };
    int cols{ 480 };
    int rows{ 320 };
    std::string window_name{ "Clustering" };

#ifdef __linux__
    int rez{ 0 };
    while ((rez = getopt(argc, argv, "m:r:c:k:")) != -1) {
        switch (rez) {
            case 'm': {
                if (std::string(optarg) == "0") {
                    rt = RUN_TYPE::REAL_TIME;
                }
                else if (std::string(optarg) == "1") {
                    rt = RUN_TYPE::LATENCY_FLOW;
                }
                else if (std::string(optarg) == "2") {
                    rt = RUN_TYPE::STATIC;
                }
                else {
                    print_help();
                }
                break;
            }
            case 'r': {
                rows = stoi(std::string(optarg));
                break;
            }
            case 'c': {
                cols = stoi(std::string(optarg));
                break;
            }
            case 'k': {
                clusters = stoi(std::string(optarg));
                break;
            }
            default: {
                print_help();
                break;
            }
        }
    }

#elif _WIN32
    std::vector<std::string> arguments;
    if (argc > 1) {
        arguments.reserve(argc - 1);
        for (int i = 1; i < argc; ++i) {
            arguments.push_back(std::string(argv[i]));
        }
        for (auto i = 0; i < arguments.size(); ++i) {
            if (*arguments[i].substr(0, 1).c_str() != '-') {
                continue;
            }
            const char key = *arguments[i].substr(1, 2).c_str();
            switch (key) {
                case 'm': {
                    std::string key_value_s = arguments[i].substr(3, 4);
                    if (key_value_s == "0") {
                        rt = RUN_TYPE::REAL_TIME;
                    }
                    else if (key_value_s == "1") {
                        rt = RUN_TYPE::LATENCY_FLOW;
                    }
                    else if (key_value_s == "2") {
                        rt = RUN_TYPE::STATIC;
                    }
                    else {
                        print_help();
                    }
                    break;
                }
                case 'r': {
                    std::string key_value_s = arguments[i].substr(3, arguments[i].size() - 1);
                    rows = stoi(key_value_s);
                    break;
                }
                case 'c': {
                    std::string key_value_s = arguments[i].substr(3, arguments[i].size() - 1);
                    cols = stoi(key_value_s);
                    break;
                }
                case 'k': {
                    std::string key_value_s = arguments[i].substr(3, arguments[i].size() - 1);
                    clusters = stoi(key_value_s);
                    break;
                }
                default: {
                    print_help();
                    break;
                }
            }
        }
    }

#endif
    printf("Clustering options: image size: [%d %d], processing mod: %d, number of clusters: %d\n", rows, cols, rt, clusters);
    Processor proc(rows, cols, clusters, rt, window_name);
    proc.launch();
    return 0;
}

void print_help() {
    printf("prog_name [options..]\n");
    printf("options:\n");
    printf("-m=(0|1|2) where 0-user enter data, 1-dots randomly generates, 2-:\n");
    printf("-r=(0..N) where N is a number of rows of image\n");
    printf("-c=(0..N) where N is a number of columns of image\n");
    printf("-k=(0..N) where N is a number of clusters\n");
}