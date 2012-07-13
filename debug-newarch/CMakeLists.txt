#
# Official dependency number 0: CMake.
# Obviously, we must have it if we are here but test version.
#
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )

#
# Enter some information about the project
# This is the only configurable part of the build system
#
# project name
PROJECT( XiaoNeiService )

SET(CMAKE_VERBOSE_MAKEFILE "1")

SET (CMAKE_CXX_COMPILER "g++")
SET (CMAKE_CXX_FLAGS "-fPIC -Wall -g -DNEWARCH")
MESSAGE ( STATUS "**************** Debug ****************" )

#
# The out-of-source building directory
#
INCLUDE( "../cmake/start.cmake" )

