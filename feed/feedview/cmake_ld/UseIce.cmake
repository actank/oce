#
# Links to libIce only.
# see also: UseIceBasics.cmake
#

INCLUDE( ${XNS_CMAKE_DIR}/UseZeroC.cmake )

IF( NOT XNS_OS_WIN )

    LINK_LIBRARIES( Ice IceUtil IceXML IceBox IceStorm IceGrid )
    #LINK_LIBRARIES( Ice IceUtil IceXML Freeze IceBox IceStorm )
    #LINK_LIBRARIES( Ice IceUtil IceXML IceBox )

ELSE ( NOT XNS_OS_WIN )

    # windows... have to link to different libs depending on build type
    LINK_LIBRARIES( optimized Ice       debug Iced
                    optimized IceUtil   debug IceUtild
                    optimized IceXML   debug IceXMLd
                    optimized Freeze   debug Freezed
                    optimized IceBox   debug IceBoxd )

ENDIF( NOT XNS_OS_WIN )
