cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project (DTNS CXX)

find_package(Boost 1.44.0 COMPONENTS thread iostreams filesystem system)
include(${Boost_INCLUDE_DIR})

add_definitions (-Wall -W)
set (SRC dtns.cpp simulation.cpp synapse.cpp GLE.cpp)
add_executable (dtns ${SRC})
target_link_libraries(dtns ${Boost_LIBRARIES})
