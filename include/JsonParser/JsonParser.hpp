#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <vector>

#include "nlohmann/json.hpp"

#include "ListeningStream.h"
#include "NetworkService.h"

using json = nlohmann::json;

inline void from_json(const json &j, std::pair<std::vector<models::ListeningStream>, std::vector<models::NetworkService>>& data) {
    int f = 0;
    return;
}

#endif // JSONPARSER_H