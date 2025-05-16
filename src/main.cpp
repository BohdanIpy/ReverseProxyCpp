#include <iostream>
#include <config.h>
#include <filesystem>
#include <fstream>

#include "JsonParser.hpp"

using Json = nlohmann::json;
namespace fs = std::filesystem;


int main()
{
    std::cout << "Version: " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << std::endl;
    std::cout << "Exec path:" << EXECUTING_PATH << std::endl;

    fs::path config_file = fs::path(EXECUTING_PATH) / "config_files/example_scheme.json";

    Json json;
    try {
        std::ifstream file(config_file);
        if (!file.is_open())
            throw std::runtime_error("Could not open config file");
        json = Json::parse(file);
    }
    catch (json::parse_error& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    //auto res = json.get<std::pair<std::vector<models::ListeningStream>, std::vector<models::NetworkService>>>();

    return 0;
}