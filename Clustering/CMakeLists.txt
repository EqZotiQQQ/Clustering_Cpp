cmake_minimum_required(VERSION 3.16)
project(Clustering)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

set(HEADERS
        headers/Centroid.h
        headers/Dot.h
        headers/Processor.h
        )

set(SOURCES
        main.cpp
        src/Processor.cpp
        )

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

add_executable(Clustering ${SOURCES} ${HEADERS})

target_link_libraries( Clustering ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT} )