
SET( XNS_SLICE_LIB_NAME "DistCacheSlice")
SET( DISTCACHESLICE_SRC_DIR "${XNS_ROOT_DIR}/DistCache/distributedcache/slice")
SET( DISTCACHESLICE_CPP_DIR "${XNS_BUILD_DIR}/tmp/${XNS_SLICE_LIB_NAME}/cpp")
FILE( MAKE_DIRECTORY ${DISTCACHESLICE_CPP_DIR} )
#
# Slice
#
FILE( GLOB slice_source RELATIVE ${DISTCACHESLICE_SRC_DIR} "${DISTCACHESLICE_SRC_DIR}/*.ice" )
SET ( XNS_DISTCACHESLICE_SOURCE_FILES ${slice_source} CACHE INTERNAL "DistCacheSlice ice files" FORCE)

#MESSAGE( STATUS "DEBUG: XNS_SLICE_SOURCE_FILES="${slice_source} )
#MESSAGE( STATUS "DEBUG: XNS_SLICE_SOURCE_FILES="${XNS_UTILSLICE_SOURCE_FILES} )

#
# C++
#
MESSAGE( STATUS "Planning to build C++ version of Slice interfaces ${XNS_SLICE_LIB_NAME}." )

INCLUDE( ${XNS_CMAKE_DIR}/slice2cpp_rules.cmake )

GENERATE_SLICE2CPP_RULES( ${DISTCACHESLICE_SRC_DIR} ${DISTCACHESLICE_CPP_DIR} slice_generated_sources
slice_generated_headers ${XNS_DISTCACHESLICE_SOURCE_FILES} )
#MESSAGE( STATUS "DEBUG: build_interface_lib_cpp.cmake: slice_generated_headers=${slice_generated_headers}" )
#MESSAGE( STATUS "DEBUG: build_interface_lib_cpp.cmake: slice_generated_sources=${slice_generated_sources}" )
#SET( XNS_SLICE_GENERATED_HEADERS ${slice_generated_headers} CACHE INTERNAL "C++ source files generated from Slice sources" FORCE )

ADD_LIBRARY( DistCacheSlice ${slice_generated_sources} )
