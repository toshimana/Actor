CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT( ActorTest )

# GMock
FIND_PATH( GMOCK_ROOT gmock/gmock.hpp
				/usr/local/include )
SET( GMOCK_SRCS ${GMOCK_ROOT}/gmock-gtest-all.cc ${GMOCK_ROOT}/gmock_main.cc )
INCLUDE_DIRECTORIES( ${GMOCK_ROOT} )

# Test
INCLUDE_DIRECTORIES( "../src" )

FILE( GLOB TESTS "*.cpp" )
ADD_EXECUTABLE( ActorTest ${TESTS} ${GMOCK_SRCS} )

