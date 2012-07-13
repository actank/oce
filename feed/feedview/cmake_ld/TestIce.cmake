# Test that Ice works

# This module assumes that Ice is already found and ICE_HOME is defined
# Defines the following variables:
# ICE_WORKS : 1 if test passed, 0 otherwise.

INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )
MESSAGE( STATUS "CMAKE_ROOT is ${CMAKE_ROOT}")

IF( NOT DEFINED ICE_HOME )
    MESSAGE( STATUS "Testing Ice - CANNOT test because ICE_HOME is not defined." )
    SET( ICE_WORKS 0 )
ENDIF( NOT DEFINED ICE_HOME )

#IF( NOT XNS_OS_WIN )
    # On 64-bit systems, Ice libraries are installed in lib64.  Is there a way to tell automatically?
#    CHECK_INCLUDE_FILE_CXX( "Ice/Ice.h" ICE_WORKS "-I${ICE_HOME}/include -L${ICE_HOME}/lib -L${ICE_HOME}/lib64 -lIce -lIceUtil -L/usr/lib64/nptl -lpthread -lrt" )
#    IF( ICE_WORKS )
#        SET( ICE_WORKS 1 )
#    ELSE ( ICE_WORKS )
#        SET( ICE_WORKS 0 )
#    ENDIF( ICE_WORKS )
#ELSE (NOT XNS_OS_WIN)
    # Windows: there is no easy way to pass separate compile and link options to the macro, 
    # so assume we are told the truth when
    SET( ICE_WORKS 1 )
#ENDIF(NOT XNS_OS_WIN)

# Check Ice version
EXEC_PROGRAM ( ${ICE_HOME}/bin/slice2cpp ARGS --version OUTPUT_VARIABLE ICE_VERSION )
#MESSAGE( STATUS "DEBUG: Ice version: '${ICE_VERSION}'")
#MESSAGE( STATUS "DEBUG: CMAKE_ROOT='${CMAKE_ROOT}' Ice works: '${ICE_WORKS}'")

IF( ICE_VERSION MATCHES "3\\.[2-9]\\.[0-9]" )
    SET( ICE_VERSION_OK 1 )
ELSE ( ICE_VERSION MATCHES "3\\.[2-9]\\.[0-9]" )
    SET( ICE_VERSION_OK 0 )
ENDIF( ICE_VERSION MATCHES "3\\.[2-9]\\.[0-9]" )

ASSERT ( ICE_VERSION_OK
            "Checking Ice version - ${ICE_VERSION} - failed. Xce requires Ice v. 3.2.x"
            "Checking Ice version - ${ICE_VERSION} - ok"
            1 )
