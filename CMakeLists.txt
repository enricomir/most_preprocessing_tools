cmake_minimum_required(VERSION 2.8)
project( MOST )
find_package( OpenCV REQUIRED )

message(${OpenCV_LIBS})

include_directories( ${OpenCV_INCLUDE_DIRS} "./inc")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Werror -g -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Wextra -Werror -g -fopenmp")

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
target_link_libraries(terminal ${OpenCV_LIBS} ${GEOS})	

##############GEOS########################
find_library(GEOS_C geos_c)
find_path(GEOS_INC geos_c.h)

message(${GEOS})
message(${GEOS_INC})
add_executable(geom src/geos_main.cpp src/polygon.cpp)
target_link_libraries(geom ${GEOS})

add_executable(ga src/ga_main.cpp src/polygon.cpp src/ga.cpp)
target_link_libraries(ga ${GEOS})