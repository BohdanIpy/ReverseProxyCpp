//
// Created by geth on 5/16/25.
//

#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace models {
    struct backend {
        std::string url;
        int port;
        int max_client;
    };

    class NetworkService {
    public:
        int number;
        std::string path_prefix;
        std::string change_path;
        std::vector<backend> backends;

    private:


    };
}

#endif //NETWORKSERVICE_H
