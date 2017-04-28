cmake_minimum_required(VERSION 3.7)
project(InternetApplications)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES l7-Server.c)
add_executable(InternetApplications ${SOURCE_FILES})