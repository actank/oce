INCLUDE_DIRECTORIES("${XNS_ROOT_DIR}/third-party/gtest")
INCLUDE_DIRECTORIES("${XNS_ROOT_DIR}/third-party/gtest/include")
FILE( GLOB BUILD_SRC_CPP_SOURCE "${XNS_ROOT_DIR}/third-party/gtest/src/*")

ADD_LIBRARY("GoogleTest" ${BUILD_SRC_CPP_SOURCE})
