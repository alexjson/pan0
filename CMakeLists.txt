cmake_minimum_required(VERSION 2.8)
project( pan0 )
find_package( OpenCV REQUIRED )
set(HEADERS
   include/pan0.h
   include/dataparser.h
   include/imageanalyser.h
   include/imageobject.h
   include/pan0stitcher.h

)

set(SOURCE
 	source/pan0.cpp
 	source/dataparser.cpp
 	source/imageanalyser.cpp
 	source/imageobject.cpp
 	source/pan0stitcher.cpp
)
add_executable( pan0  ${SOURCE} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(pan0 /home/alex/xjobb/c++/libjson/libjson.a)
target_link_libraries( pan0 ${OpenCV_LIBS} )

