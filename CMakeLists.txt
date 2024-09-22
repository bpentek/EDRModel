cmake_minimum_required(VERSION 3.10)
project(EDRModel)

# find GSL lib
find_package(GSL REQUIRED)
include_directories(${GSL_INCLUDE_DIRS} include)

# adding executables
add_executable(generate_distance_histogram src/distance/distance.c src/distance/generate_distance_histogram.c)
add_executable(generate_network src/network/network.c src/distance/distance.c src/network/generate_network.c)

# linking GSL & math library
target_link_libraries(generate_distance_histogram ${GSL_LIBRARIES} m)
target_link_libraries(generate_network ${GSL_LIBRARIES} m)
