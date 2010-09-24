cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project (DTNS CXX)


find_package(Boost 1.43.0 COMPONENTS date_time thread iostreams system program_options)
include(${Boost_INCLUDE_DIR})

add_definitions (-Wall -W)
set (SRC dtns.cpp tuning.cpp simulation.cpp synapse.cpp)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -funroll-loops -ffast-math")
add_executable (dtns ${SRC})
target_link_libraries(dtns ${Boost_LIBRARIES} ncurses)
