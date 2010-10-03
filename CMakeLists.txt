cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
project (DTNS CXX)
SET(BOOST_ROOT "/usr/local/")
find_package(Boost 1.44.0 COMPONENTS date_time thread iostreams system program_options)
find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-2.4)
pkg_check_modules(PLOTMM plotmm)

add_subdirectory(src)
