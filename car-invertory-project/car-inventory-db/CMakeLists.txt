cmake_minimum_required(VERSION 3.16)

project(car_inventory_db LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(car_inventory
    src/main.cpp
    src/CarInventoryApp.cpp
    src/CarRepository.cpp
    external/sqlite/sqlite3.c
)

target_include_directories(car_inventory PRIVATE
    include
    external/sqlite
)

target_compile_options(car_inventory PRIVATE
    -Wall -Wextra
)