link_directories(
    ${GTKMM_LIBRARY_DIRS} ${PLOTMM_LIBRARY_DIRS})

include_directories(
    ${GTKMM_INCLUDE_DIRS} ${PLOTMM_INCLUDE_DIRS})

include(${Boost_INCLUDE_DIR})

add_definitions (-Wall -W)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -funroll-loops -ffast-math")

set (SRC dtns.cpp tuning.cpp simulation.cpp synapse.cpp)
set (SRCSANDBOX sandbox.cpp simulation.cpp synapse.cpp)

add_executable (dtns ${SRC})
add_executable (sandbox ${SRCSANDBOX})

target_link_libraries(dtns ${Boost_LIBRARIES} ncurses)
target_link_libraries(sandbox ${Boost_LIBRARIES} ${GTKMM_LIBRARIES} ${PLOTMM_LIBRARIES})
