
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREAD ON)

SET(BOOST_ROOT /data/home/yuwei.mu/xce/ )
SET(BOOST_INCLUDEDIR /data/home/yuwei.mu/xce/ )
SET(BOOST_LIBRARYDIR /data/home/yuwei.mu/xce/ )
SET(Boost_INCLUDE_DIRS /data/home/yuwei.mu/xce/ )

FIND_PACKAGE( Boost COMPONENTS date_time regex thread system)

MESSAGE ( STATUS "*** Boost_FOUND " ${Boost_FOUND} )
MESSAGE ( STATUS "*** Boost_INCLUDE_DIRS " ${Boost_INCLUDE_DIRS} )
MESSAGE ( STATUS "*** Boost_LIBRARIES " ${Boost_LIBRARIES} )
MESSAGE ( STATUS "*** Boost_LIBRARY_DIRS " ${Boost_LIBRARY_DIRS} )


MESSAGE ( STATUS "*** boost_include " ${boost_include} )

FOREACH ( boost_include ${Boost_INCLUDE_DIRS} )
	# INCLUDE_DIRECTORIES( ${boost_include} )
ENDFOREACH ( boost_include )

FOREACH ( boost_library_dir ${Boost_LIBRARY_DIRS} )
	LINK_DIRECTORIES( ${boost_library_dir} )
ENDFOREACH ( boost_library_dir )

FOREACH ( boost_library ${Boost_LIBRARIES} )
	LINK_LIBRARIES( ${boost_library} )
ENDFOREACH ( boost_library )

