
INCLUDE_DIRECTORIES("/usr/local/include/jwsmtp-1.32/")
LINK_LIBRARIES("jwsmtp")
AUX_SOURCE_DIRECTORY("../MceMonitor/src" BUILD_MceMonitorManager_SRC_CPP_SOURCE )
AUX_SOURCE_DIRECTORY("../MceMonitor/src/items" BUILD_MceMonitorItems_SRC_CPP_SOURCE )
SET( BUILD_MceMonitor_SRC_CPP_SOURCE ${BUILD_MceMonitorManager_SRC_CPP_SOURCE} ${BUILD_MceMonitorItems_SRC_CPP_SOURCE} )
ADD_LIBRARY( MceMonitor SHARED EXCLUDE_FROM_ALL ${BUILD_MceMonitor_SRC_CPP_SOURCE})
