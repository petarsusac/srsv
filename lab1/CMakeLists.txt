cmake_minimum_required(VERSION 3.10)
project(lab1)
include_directories(inc)
add_executable(lab1 
            main.c 
            input.c
            time_utils.c
            controller.c
            simulator.c
            )
target_link_libraries(lab1 PRIVATE m)