cmake_minimum_required(VERSION 3.29)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)

project(ReverseProxy VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message(STATUS "Project version: ${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTING_PATH ${CMAKE_CURRENT_SOURCE_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_executable(ReverseProxy src/main.cpp
        src/HotReloadConfigurer/HotReloadConfigurer.cpp
        src/Models/NetworkService.cpp
        src/Models/ListeningStream.cpp
        include/JsonParser/JsonParser.h
        src/Models/HttpSession.cpp
)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED CONFIG)
find_package(spdlog)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/HotReloadConfigurer
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Models
    ${CMAKE_CURRENT_SOURCE_DIR}/include/JsonParser
    ${CMAKE_CURRENT_BINARY_DIR}
)

message(STATUS "included")

target_link_libraries(ReverseProxy
        Boost::system
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)
