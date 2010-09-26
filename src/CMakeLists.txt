

SET(CMAKE_INCLUDE_PATH "${CMAKE_INCLUDE_PATH}:../common")

SET (SRCCOMMON ../common/simulation.cpp ../common/synapse.cpp)

add_subdirectory(dtns)
add_subdirectory(sandbox)
