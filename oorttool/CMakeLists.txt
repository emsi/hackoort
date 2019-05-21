cmake_minimum_required(VERSION 2.6)

set(oorttool_SRCS oorttool.c)

# oorttool
add_executable(oorttool ${oorttool_SRCS})
target_link_libraries(oorttool ${hackoor_LIBS} hackoort)

install(TARGETS oorttool DESTINATION bin)
