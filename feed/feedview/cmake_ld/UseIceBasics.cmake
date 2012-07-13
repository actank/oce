#
# Links to Ice basics defined as "what libOrcaIce uses"
# (and therefore all standard components use)
#

INCLUDE( ${XNS_CMAKE_DIR}/UseZeroC.cmake )
INCLUDE( ${XNS_CMAKE_DIR}/UseIce.cmake )

IF( NOT XNS_OS_WIN )

    LINK_LIBRARIES( IceStorm IceBox IceGrid )

ELSE ( NOT XNS_OS_WIN )

    # windows... have to link to different libs depending on build type
    LINK_LIBRARIES( optimized IceStorm  debug IceStormd 
                    optimized IceBox    debug IceBoxd
                    optimized IceGrid   debug IceGridd )

ENDIF( NOT XNS_OS_WIN )