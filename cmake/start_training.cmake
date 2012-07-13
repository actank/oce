ADD_SERVICE_EXEC("training/ObjectCacheDemoCmake/client" "ObjectCacheDemoClient" OceSliceObjectCacheDemo)
ADD_COMPONENT("training/ObjectCacheDemoCmake/impl" "ObjectCacheDemoI" OceSliceObjectCacheDemo) 
ADD_SERVICE_EXEC("training/ObjectCacheDemoCmake/server" "ObjectCacheDemoServer" ObjectCacheDemoI IceExt)

#AUX_SOURCE_DIRECTORY("${XNS_ROOT_DIR}/training/ObjectCacheDemo/client" BUILD_ObjectCacheDemoClient_SRC_CPP_SOURCE )
#ADD_EXECUTABLE( "ObjectCacheDemoClient" EXCLUDE_FROM_ALL ${BUILD_ObjectCacheDemoClient_SRC_CPP_SOURCE} )
#TARGET_LINK_LIBRARIES( "ObjectCacheDemoClient" OceSliceObjectCacheDemo )
