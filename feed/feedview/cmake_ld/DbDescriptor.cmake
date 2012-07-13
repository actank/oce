
AUX_SOURCE_DIRECTORY("../DbDescriptor/reload" BUILD_reloadDbDescriptor_SRC_CPP_SOURCE )
ADD_EXECUTABLE( reloadDbDescriptor EXCLUDE_FROM_ALL ${BUILD_reloadDbDescriptor_SRC_CPP_SOURCE})
TARGET_LINK_LIBRARIES( reloadDbDescriptor IceExt UtilCxx )

