cmake_minimum_required( VERSION 2.8 )
project( IsThingPresent )

find_package( OpenCV REQUIRED )
find_package(Boost COMPONENTS system program_options REQUIRED)
find_package( Curses REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${CURSES_INCLUDE_DIR} )

add_executable( IsThingPresent IsThingPresent.cpp)

target_link_libraries( IsThingPresent ${OpenCV_LIBS} )
target_link_libraries( IsThingPresent ${Boost_SYSTEM_LIBRARY} )
target_link_libraries( IsThingPresent ${Boost_PROGRAM_OPTIONS_LIBRARY} )


