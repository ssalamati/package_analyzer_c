cmake_minimum_required(VERSION 3.10)
project(PackageAnalyzer)

set(CMAKE_C_STANDARD 99)

find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(package_analyzer main.c package_analyzer.c)

# Ensure the directory containing uthash.h is included
include_directories(/path/to/uthash/header/file)

target_include_directories(package_analyzer PRIVATE ${CURL_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})

target_link_libraries(package_analyzer ${CURL_LIBRARIES} ${ZLIB_LIBRARIES})
