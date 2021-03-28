cmake_minimum_required(VERSION 3.15)
project(libcsvxx LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

add_library(libcsvxx INTERFACE)
add_library(libcsvxx::libcsvxx ALIAS libcsvxx)

target_sources(libcsvxx INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/csv.h> $<INSTALL_INTERFACE:>)
set_target_properties(libcsvxx PROPERTIES PUBLIC_HEADER "csv.h")

include(GNUInstallDirs)
install(TARGETS libcsvxx EXPORT libcsvxx PUBLIC_HEADER DESTINATION include/libcsvxx)
install(EXPORT libcsvxx NAMESPACE libcsvxx:: DESTINATION lib/cmake/libcsvxx)
