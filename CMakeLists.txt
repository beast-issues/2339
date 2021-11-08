cmake_minimum_required(VERSION 3.20)
project(boost_issue_2339)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_executable(boost_issue_2339 main.cpp)
target_link_libraries(boost_issue_2339 Boost::boost Boost::system Boost::thread Threads::Threads OpenSSL::SSL OpenSSL::Crypto)