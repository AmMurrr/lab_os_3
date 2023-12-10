cmake_minimum_required (VERSION 3.22)
project(lab_os_3)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(child child.c)
add_executable(main main.c)
