

INCLUDE( "${XNS_CMAKE_DIR}/project_path.cmake" )
SET(EXECUTABLE_OUTPUT_PATH ${XNS_BUILD_DIR} )
SET(LIBRARY_OUTPUT_PATH ${XNS_BUILD_DIR} )
INCLUDE( ${XNS_CMAKE_DIR}/misc.cmake )
INCLUDE( ${XNS_CMAKE_DIR}/UseIce.cmake )
INCLUDE( ${XNS_CMAKE_DIR}/UseBoost.cmake )
INCLUDE( ${XNS_CMAKE_DIR}/slice2cpp_rules.cmake )
INCLUDE( ${XNS_CMAKE_DIR}/protobuf_rules.cmake )

INCLUDE_DIRECTORIES( 
  "/data/home/yuwei.mu/xce/"
	"/usr/src/kernels/2.6.9-78.EL-smp-x86_64/include"
  ${XNS_ROOT_DIR}
	${ICE_HOME}/include
	"/usr/local/xce-dev/include"
	"/usr/local/xce/include"
	${XNS_ROOT_DIR}/feed
	${XNS_BUILD_DIR}/tmp/UtilSlice/cpp
	${XNS_ROOT_DIR}/UtilCxx/src
	${XNS_ROOT_DIR}/TalkUtil/src
	${XNS_ROOT_DIR}/IceExt/src
	${XNS_ROOT_DIR}/DbCxxPool/src
	${XNS_BUILD_DIR}/tmp/OceSlice/cpp
	${XNS_ROOT_DIR}/OceCommon/src
	${XNS_ROOT_DIR}/OceCxxAdapter/src
	${XNS_ROOT_DIR}/Asio/src
	${XNS_ROOT_DIR}/feed
	${XNS_ROOT_DIR}/TopicClients/src
	${XNS_ROOT_DIR}/depends/mysql++-3.0.6/lib
	"/opt/curl/include"
	"/usr/include/mysql++"
	"/usr/include/mysql"
	"/usr/include/"
	"/usr/local/include/"
	"/usr/local/include/"
	"/usr/local/distcache/include"
	"/usr/local/distcache-dev/include"
	"/usr/local/distcache-dev/include/c-client-src"
	"/usr/local/distcache-util/include"
	${XNS_ROOT_DIR}/third-party/google-breakpad/src
	${XNS_BUILD_DIR}/tmp/OceProto/cc
	${XNS_BUILD_DIR}/tmp/DistCacheProto/cc
	${XNS_BUILD_DIR}/tmp/DistCacheSlice/cpp
	${XNS_BUILD_DIR}/tmp/SocialGraphProto/cc
	)

LINK_DIRECTORIES( /opt/Ice-3.3.1/lib64 /usr/lib64/nptl /usr/local/xce-dev/lib64 /usr/local/xce/lib64 /usr/local/xce/lib /opt/curl/lib ${XNS_ROOT_DIR}/depends/mysql++-3.0.6/	/opt/XiaoNei/mysqlpp_lib64/ 
    ~/jsoncpp-src-0.5.0/libs/linux-gcc-4.1.2    ~/3rdlibs/lib
	/usr/local/distcache-dev/lib
	/usr/local/distcache/lib
	/usr/local/distcache-util/lib
  )
LINK_LIBRARIES( pthread rt log4cplus mysqlpp crypto ssl  )

