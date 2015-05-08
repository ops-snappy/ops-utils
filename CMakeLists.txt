# Copyright (C) 2015 Hewlett-Packard Development Company, L.P.
# All Rights Reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License"); you may
#    not use this file except in compliance with the License. You may obtain
#    a copy of the License at
#
#         http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
#    WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
#    License for the specific language governing permissions and limitations
#    under the License.

cmake_minimum_required (VERSION 2.8)

set (UTILS_LIBS halonutils)
project (${UTILS_LIBS})

include(FindPkgConfig)
pkg_check_modules(SYSTEMD libsystemd)

add_definitions(${SYSTEMD_CFLAGS})
link_directories(${SYTEMD_LIBRARY_DIRS})
include_directories(${SYSTEMD_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/../../include")

# Source files to build halon-core library
set (SOURCES hc-utils.c)

# Rules to build halon-core library.
add_library (${UTILS_LIBS} SHARED ${SOURCES})
target_link_libraries(${UTILS_LIBS} ${SYSTEMD_LIBRARIES})

set(HALONUTILS_VERSION_MAJOR "0")
set(HALONUTILS_VERSION_MINOR "1")
set(HALONUTILS_VERSION_PATCH "0")
set(HALONUTILS_VERSION "${HALONUTILS_VERSION_MAJOR}.${HALONUTILS_VERSION_MINOR}.${HALONUTILS_VERSION_PATCH}")
set_target_properties(${UTILS_LIBS} PROPERTIES VERSION ${HALONUTILS_VERSION})

configure_file(halonutils.pc.in halonutils.pc @ONLY)

# Rules to stage halon-core library and header files.
install(TARGETS ${UTILS_LIBS}
	ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
)

install(FILES hc-utils.h ../../include/slog.h
	DESTINATION include)

install(FILES ${CMAKE_BINARY_DIR}/halonutils.pc DESTINATION lib/pkgconfig)
