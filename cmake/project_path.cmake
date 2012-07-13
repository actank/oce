
INCLUDE( "${XNS_CMAKE_DIR}/Assert.cmake" )

INCLUDE(${CMAKE_ROOT}/Modules/FindJava.cmake)
# MESSAGE( STATUS "DEBUG:   Java runtime ${JAVA_RUNTIME}" )
# MESSAGE( STATUS "DEBUG:   Java archive ${JAVA_ARCHIVE}" )
# MESSAGE( STATUS "DEBUG:   Java compiler ${JAVA_COMPILE}" )
IF( JAVA_RUNTIME )
    SET( JAVA_FOUND 1 )
ENDIF( JAVA_RUNTIME )
ASSERT( JAVA_FOUND "Looking for Java - not found" "Looking for Java - found" )

INCLUDE( ${XNS_CMAKE_DIR}/FindProtobuf.cmake )
ASSERT( PROTOBUF_FOUND "Looking for Google Protobuf - not found" "Looking for Google Protobuf - found" )
#
# Official dependency number 3: ZeroC's Ice
# Find Ice installation
#
IF( DEFINED ICE_HOME )
# Ice home is specified with a command line option or it's already in cache
    MESSAGE( STATUS "Ice location was specified or using cached value: ${ICE_HOME}")
ELSE ( DEFINED ICE_HOME )
# Find Ice installation
INCLUDE( ${XNS_CMAKE_DIR}/FindIce.cmake )
    ASSERT( ICE_FOUND
	    "Looking for Ice - not found. Please install Ice, ** delete CMakeCache.txt **, then re-run CMake."
	    "Looking for Ice - found in ${ICE_HOME}"
	    1 )
    ASSERT( ICEJ_FOUND
	    "Looking for Ice-Java - not found. Please install Ice, ** delete CMakeCache.txt **, then re-run CMake."
	    "Looking for Ice-Java - found in ${ICEJ_HOME}"
	    1 )
ENDIF( DEFINED ICE_HOME )
# MESSAGE( STATUS "DEBUG:   IceJ_HOME ${ICEJ_HOME}" )

# Test Ice installation
INCLUDE( ${XNS_CMAKE_DIR}/TestIce.cmake )
    ASSERT ( ICE_WORKS
	    "Testing Ice - failed. Please check or reinstall it, ** delete CMakeCache.txt **, then re-run CMake."
	    "Testing Ice - ok."
	    1 )


