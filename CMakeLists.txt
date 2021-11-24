cmake_minimum_required(VERSION 3.20)
project(HelloMuduo)

set(CMAKE_CXX_STANDARD 14)

include_directories(/home/edward/C++/Muduo/build/release-install-cpp11/include)
link_directories(/home/edward/C++/Muduo/build/release-install-cpp11/lib)

add_executable(HelloMuduo Server/muduo_server.cpp)

target_link_libraries(HelloMuduo muduo_net muduo_base pthread)
