cmake_minimum_required(VERSION 3.10)
project(lab2)
include_directories(inc)
add_executable(lab2 
            main.c 
            time_utils.c
            periodic_signal.c
            controller.c
            input.c
            simulator.c
            stats.c
            scheduling.c
            )
target_link_libraries(lab2 PRIVATE m)