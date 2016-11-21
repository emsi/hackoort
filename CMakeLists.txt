


cmake_minimum_required(VERSION 2.6)

project(hackoort)

find_package(PkgConfig REQUIRED)

include_directories(include)

set(hackoort_SRCS src/libhackoort.c)

# gattlib support
pkg_search_module(GATTLIB REQUIRED gattlib)
include_directories(${GATTLIB_INCLUDE_DIRS})
list(APPEND hackoort_LIBS ${GATTLIB_LIBRARIES})

# Added Glib support
pkg_search_module(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
list(APPEND hackoort_LIBS ${GLIB_LIBRARIES})

# libhackoort
add_library(hackoort SHARED ${hackoort_SRCS})
target_link_libraries(hackoort ${hackoort_LIBS})


# hackort tool
add_subdirectory(oorttool)

# Packaging
#
set(CPACK_PACKAGE_INSTALL_DIRECTORY /usr CACHE STRING "Install directory (default: /usr).")
set(CPACK_PACKAGE_VERSION 0.1)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Library and tool to interact with OORT Light Bulbs")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_PACKAGE_ARCHITECTURE}")

#
# Debian package
#
set(CPACK_GENERATOR "DEB;ZIP")

# Detect platform architecture to use it for the Debian package
execute_process(COMMAND dpkg --print-architecture OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE OUTPUT_QUIET)

set(CPACK_DEBIAN_PACKAGE_DEPENDS "gattlib")

# List of file to install
#
configure_file(hackoort.pc.in hackoort.pc @ONLY)
install(TARGETS hackoort LIBRARY DESTINATION lib)
install(FILES include/hackoort.h DESTINATION include)
install(FILES ${PROJECT_BINARY_DIR}/hackoort.pc DESTINATION lib/pkgconfig)

include(CPack)
