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
SET (CMAKE_CXX_FLAGS "-fPIC -Wall -O2 -DNEWARCH -DPOSIX -DPOSIX -DMONGO_HAVE_STDINT")
SET (CMAKE_C_FLAGS "-fPIC -Wall -O2 -DNEWARCH -DPOSIX -DMONGO_HAVE_STDINT")
#SET (CMAKE_CXX_FLAGS "-fPIC -Wall -g -O2 -DNEWARCH -DPOSIX")
MESSAGE ( STATUS "**************** Debug ****************" )

#
# The out-of-source building directory
#
INCLUDE( "../cmake/start.cmake" )
ADD_CUSTOM_TARGET( jars
	COMMAND mvn package WORKING_DIRECTORY ../util/java VERBATIM
	COMMAND mvn package WORKING_DIRECTORY ../client/java/OceAdapter	VERBATIM
	)

ADD_CUSTOM_TARGET( jars-deploy
	COMMAND mvn deploy WORKING_DIRECTORY ../util/java VERBATIM
	COMMAND mvn deploy WORKING_DIRECTORY ../client/java/OceAdapter	VERBATIM
	)

ADD_CUSTOM_TARGET( jars-clean
	COMMAND mvn clean WORKING_DIRECTORY ../util/java VERBATIM
	COMMAND mvn clean WORKING_DIRECTORY ../client/java/OceAdapter	VERBATIM
	)
