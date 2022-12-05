cmake_minimum_required(VERSION 3.0.2)
project(anything)

## Compile as C++11
add_compile_options(-std=c++14)

find_package(OpenCV REQUIRED)

include_directories(
  ${OpenCV_INCLUDE_DIRS}
)

## create executable file named "main"
add_executable(main
  main.cpp)

target_link_libraries(main
  ${OpenCV_LIBRARIES}
)

message("\n\n*** OpenCV ***")
message(WARNING "version ${OpenCV_VERSION}")
message(WARNING "install path ${OpenCV_INSTALL_PATH}") 
message(WARNING "config path ${OpenCV_CONFIG_PATH}") # look at the output of this message
message(WARNING "include dirs ${OpenCV_INCLUDE_DIRS}")
message(WARNING "libs ${OpenCV_LIBS}")
