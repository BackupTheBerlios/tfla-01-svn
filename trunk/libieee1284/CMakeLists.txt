# (c) 2009, Bernhard Walle <bernhard.walle@gmx.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

PROJECT(libieee1284 C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# include the current working directory
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

# always print compiler warnings
IF (CMAKE_HOST_UNIX)
    ADD_DEFINITIONS(-Wall -W -Wno-unused-parameter)
ENDIF (CMAKE_HOST_UNIX)

IF (MSVC)
    # MSVC++ ignores all exception specifications except '()' [i.e. no exception]
    # we can safely ignore that warning when compiling on MS Windows
    ADD_DEFINITIONS("/w44290")
ENDIF (MSVC)


SET(libieee1284_SRCS
    src/access_io.c
    src/access_lpt.c
    src/access_ppdev.c
    src/conf.c
    src/debug.c
    src/default.c
    src/delay.c
    src/detect.c
    src/deviceid.c
    src/interface.c
    src/ports.c
    src/state.c
)

SET(libieee1284_test_SRCS
    tests/test.c
)

# build sources, moc'd sources, and rcc'd sources
ADD_LIBRARY(
    ieee1284 STATIC
    ${libieee1284_SRCS}
)

ADD_EXECUTABLE(test
    ${libieee1284_test_SRCS}
)
TARGET_LINK_LIBRARIES(test ieee1284)
ADD_DEPENDENCIES(test ieee1284)

# vim: set sw=4 ts=4 et fdm=marker:
