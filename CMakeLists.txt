cmake_minimum_required(VERSION 3.29)
project(ReverseProxy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ReverseProxy src/main.cpp
        src/HotReloadConfigurer/HotReloadConfigurer.cpp)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED CONFIG)
find_package(spdlog)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/HotReloadConfigurer
)

target_link_libraries(ReverseProxy
        Boost::system
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)
