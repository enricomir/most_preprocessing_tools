cmake_minimum_required(VERSION 2.8)
project( MOST )
find_package( OpenCV REQUIRED )

message(${OpenCV_LIBS})
message(${OpenCV_INCLUDE_DIRS})

find_library(GEOS_C geos_c)
find_path(GEOS_INC geos_c.h)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

include_directories( ${OpenCV_INCLUDE_DIRS} "./inc")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(segmenter src/segmenter_main.cpp)
target_link_libraries(segmenter ${OpenCV_LIBS})

add_executable(simplifier src/simplifier_main.cpp)
target_link_libraries(simplifier ${OpenCV_LIBS})

add_executable(frame_extractor src/frame_extractor_main.cpp)
target_link_libraries(frame_extractor ${OpenCV_LIBS})

add_executable(rotator src/rotator_main.cpp)
target_link_libraries(rotator ${OpenCV_LIBS})

add_executable(hsv src/hsvtest.cpp)
target_link_libraries(hsv ${OpenCV_LIBS})

add_executable(auto_segmenter src/auto_segmenter.cpp)
target_link_libraries(auto_segmenter ${OpenCV_LIBS})	

add_executable(vcp src/vcp_main.cpp)
target_link_libraries(vcp ${OpenCV_LIBS})	

add_executable(smoke_remover src/smoke_remover.cpp)
target_link_libraries(smoke_remover ${OpenCV_LIBS})	

add_executable(terminal src/terminal.cpp src/polygon.cpp)
target_link_libraries(terminal ${OpenCV_LIBS} ${GEOS_C})	

##############GEOS########################

add_executable(geom src/geos_main.cpp src/polygon.cpp)
target_link_libraries(geom ${GEOS_C})

add_executable(twkt_converter src/twkt_converter.cpp src/polygon.cpp)
target_link_libraries(twkt_converter ${GEOS_C})

add_executable(msimp src/multi_simplifier.cpp src/polygon.cpp src/simplifier.cpp)
target_link_libraries(msimp ${GEOS_C})

add_executable(ga src/ga_main.cpp src/polygon.cpp src/ga.cpp)
target_link_libraries(ga ${GEOS_C})

add_executable(aligner src/aligner_main.cpp src/polygon.cpp src/aligner.cpp)
target_link_libraries(aligner ${GEOS_C})
