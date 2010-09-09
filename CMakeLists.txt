cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project (DTNS CXX)

find_package(Boost 1.43.0 COMPONENTS date_time thread iostreams filesystem system program_options)
include(${Boost_INCLUDE_DIR})

add_definitions (-Wall -W)
set (SRC dtns.cpp tuning.cpp simulation.cpp synapse.cpp GLE.cpp)
add_executable (dtns ${SRC})
target_link_libraries(dtns ${Boost_LIBRARIES} ncurses)
