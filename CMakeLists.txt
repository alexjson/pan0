#C++ CMakeLists.txt
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project(MyProject)
find_package( OpenCV REQUIRED )
set(BOOSTCOMPONENTS
	date_time
	graph
	timer
)
find_package(Boost 1.45.0 COMPONENTS ${BOOSTCOMPONENTS} REQUIRED) 
include_directories(${MyProject_SOURCE_DIR}/include)
include_directories(${MyProject_SOURCE_DIR}/source)
include_directories(${MyProject_SOURCE_DIR}/libjson)

set(HEADERS
   include/pan0.h
   include/dataparser.h
   include/imageanalyser.h
   include/imageobject.h
   include/pan0stitcher.h
   include/bfsvertexvisitor.h
   include/utils.h


)

set(SOURCE
 	source/pan0.cpp
 	source/dataparser.cpp
 	source/imageanalyser.cpp
 	source/imageobject.cpp
 	source/pan0stitcher.cpp
 	source/bfsvertexvisitor.cpp
 	source/utils.cpp
)
add_executable( pan0  ${SOURCE} ${HEADER_FILES})


TARGET_LINK_LIBRARIES(pan0 ${MyProject_SOURCE_DIR}/libjson/libjson.a)
TARGET_LINK_LIBRARIES( pan0 ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES( pan0 ${Boost_LIBRARIES})
target_link_libraries(pan0)