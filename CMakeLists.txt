cmake_minimum_required(VERSION 3.10)

project(Coldforce VERSION 1.0 LANGUAGES C)

file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE HEADERS inc/*.h)
file(GLOB_RECURSE HEADERS inc/coldforce/core/*.h)


add_library(coldforce_static STATIC ${SOURCES})
add_library(coldforce_shared SHARED ${SOURCES})

target_include_directories(coldforce_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(coldforce_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set_target_properties(coldforce_static PROPERTIES C_STANDARD 99)
set_target_properties(coldforce_shared PROPERTIES C_STANDARD 99)
install(TARGETS coldforce_static coldforce_shared
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib)
install(DIRECTORY inc/ DESTINATION include)

configure_file(coldforce.pc.in coldforce.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/coldforce.pc DESTINATION lib/pkgconfig)

# add_subdirectory(examples)
