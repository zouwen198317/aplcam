cmake_minimum_required (VERSION 2.8)
project(aplcam)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# set(CMAKE_C_FLAGS "-ggdb -Wall")
# set(CMAKE_CXX_FLAGS "-ggdb -Wall")

#set(CMAKE_BUILD_TYPE Debug )
#set(CMAKE_C_FLAGS_DEBUG, "-Wall -ggdb")
#set(CMAKE_CXX_FLAGS_DEBUG, "-Wall -ggdb")
#
#set(CMAKE_C_FLAGS_RELEASE, "-Werror")
#set(CMAKE_CXX_FLAGS_RELEASE, "-Werror")

find_package( OpenCV REQUIRED core imgproc calib3d features2d highgui )
#find_package( PCL 1.2 REQUIRED )
#find_package( Ceres REQUIRED )

# Packages supported by local Find*.cmake files
find_package( GSL REQUIRED )
find_package( Eigen3 REQUIRED )
find_package( TCLAP REQUIRED )

## As suggested, always build a local copy of GTest

#
#
#
## KyotoCabinet stuff
#find_path(KyotoCabinet_INCLUDE_DIR kcdb.h
#  HINTS ${PC_KyotoCabinet_INCLUDEDIR} ${PC_KyotoCabinet_INCLUDE_DIRS})
#
#find_library(KyotoCabinet_LIBRARY NAMES libkyotocabinet kyotocabinet
#  HINTS ${PC_KyotoCabinet_LIBDIR} ${PC_KyotoCabinet_LIBRARY_DIRS})
#
#set(KyotoCabinet_LIBRARIES ${KyotoCabinet_LIBRARY})
#set(KyotoCabinet_INCLUDE_DIRS ${KyotoCabinet_INCLUDE_DIR})
#
find_package( Boost REQUIRED COMPONENTS thread system filesystem )
set( Boost_USE_MULTITHREADED ON )

add_definitions( -std=c++11 -DEIGEN_DONT_VECTORIZE -DEIGEN_DONT_ALIGN )

option( USE_TBB "Use TBB" ON )
if( USE_TBB )
  add_definitions( -DUSE_TBB )   # -DTBB_USE_DEBUG=1
endif()

# option( USE_APRILTAGS "Use AprilTags" ON )
# if( USE_APRILTAGS )
#   #set( apriltags_SOURCE_DIR "/home/aaron/workspace/vision_exp/apriltags")
#   message("Find_package apriltags in CMakeLists.txt")
#   find_package( Apriltags REQUIRED )

## In the transition to Conan, force use of Apriltags for now...
add_definitions( -DUSE_APRILTAGS )

#
#   #add_subdirectory( "${aplcam_SOURCE_DIR}/third_party/apriltags" )
#
#   # find_path( APRILTAGS_INCLUDE_DIR AprilTags/TagDetection.h
#   #   PATHS ${aplcam_SOURCE_DIR}/third_party/apriltags/ PARENT_SCOPE )
#   # set( APRILTAGS_LIBS ${aplcam_BINARY_DIR}/third_party/apriltags/libapriltags.a )
#   # set( APRILTAGS_LIBS ${APRILTAGS_LIBS} PARENT_SCOPE )
#   #
#   # include_directories( ${APRILTAGS_INCLUDE_DIR} )
# endif()


include_directories(
  ${CONAN_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
  ${CERES_INCLUDE_DIRS} )



# set( aplcam_LIBS ${aplcam_LIBS} PARENT_SCOPE )
#
# set( tclap_INCLUDE_DIR "${aplcam_SOURCE_DIR}/third_party/tclap/include" PARENT_SCOPE)


add_subdirectory(lib)
add_subdirectory(tools)
