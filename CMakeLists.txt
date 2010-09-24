cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project (DTNS CXX)

find_package(Boost 1.43.0 COMPONENTS date_time thread iostreams system program_options)
find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-2.4)

add_subdirectory(src)
