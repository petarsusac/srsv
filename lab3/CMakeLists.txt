cmake_minimum_required(VERSION 3.10)
project(lab3)
include_directories(inc)
add_executable(lab3 
            main.c 
            input.c
            time_utils.c
            controller.c
            simulator.c
            stats.c
            )
target_link_libraries(lab3 PRIVATE m)