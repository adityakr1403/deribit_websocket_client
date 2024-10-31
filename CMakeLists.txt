cmake_minimum_required(VERSION 3.29)
project(deribitapp)

set(CMAKE_CXX_STANDARD 17)
find_package(ixwebsocket REQUIRED)

add_executable(deribitapp main.cpp
        DeribitClient.cpp
        DeribitClient.h
)
target_link_libraries(deribitapp ixwebsocket z ssl crypto)
