
SET( XNS_SLICE_LIB_NAME "OceSlice")
SET( OCESLICE_SRC_DIR "${XNS_ROOT_DIR}/${XNS_SLICE_LIB_NAME}/slice")
SET( OCESLICE_CPP_DIR "${XNS_BUILD_DIR}/tmp/${XNS_SLICE_LIB_NAME}/cpp")
SET( OCESLICE_JAVA_DIR "${XNS_BUILD_DIR}/tmp/${XNS_SLICE_LIB_NAME}/java")
FILE( MAKE_DIRECTORY ${OCESLICE_CPP_DIR} ${OCESLICE_JAVA_DIR} )
#
# Slice
#
FILE( GLOB slice_source RELATIVE ${OCESLICE_SRC_DIR} "${OCESLICE_SRC_DIR}/*.ice" )
SET ( XNS_OCESLICE_SOURCE_FILES ${slice_source} CACHE INTERNAL "OceSlice ice files" FORCE)

#MESSAGE( STATUS "DEBUG: XNS_SLICE_SOURCE_FILES="${slice_source} )
#MESSAGE( STATUS "DEBUG: XNS_SLICE_SOURCE_FILES="${XNS_OCESLICE_SOURCE_FILES} )

#
# C++
#
MESSAGE( STATUS "Planning to build C++ version of Slice interfaces ${XNS_SLICE_LIB_NAME}." )

INCLUDE( ${XNS_CMAKE_DIR}/slice2cpp_rules.cmake )

GENERATE_SLICE2CPP_RULES( ${OCESLICE_SRC_DIR} ${OCESLICE_CPP_DIR} slice_generated_sources
slice_generated_headers ${XNS_OCESLICE_SOURCE_FILES} )
#MESSAGE( STATUS "DEBUG: build_interface_lib_cpp.cmake: slice_generated_headers=${slice_generated_headers}" )
#MESSAGE( STATUS "DEBUG: build_interface_lib_cpp.cmake: slice_generated_sources=${slice_generated_sources}" )
#SET( XNS_SLICE_GENERATED_HEADERS ${slice_generated_headers} CACHE INTERNAL "C++ source files generated from Slice sources" FORCE )

#ADD_LIBRARY( OceSlice ${slice_generated_sources} )


#MESSAGE(STATUS "DEBUG: ${slice_generated_sources}")
GENERATE_ARCHIVE_EACH( OceSlice "${slice_generated_sources}" UtilSlice)
