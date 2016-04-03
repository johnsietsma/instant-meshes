cmake_minimum_required (VERSION 2.8.3)

set(PROJECT_NAME InstantMeshesLib)
project(${PROJECT_NAME})

option(INSTANT_MESHES_DEV_MODE "Instant meshes developer mode" OFF)

if (MSVC)
  if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    # Disable Eigen vectorization for Windows 32 bit builds (issues with unaligned access segfaults)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DEIGEN_DONT_ALIGN")
  endif()
endif()

# Build TBB
set(TBB_BUILD_STATIC ON CACHE BOOL " " FORCE)
set(TBB_BUILD_SHARED OFF CACHE BOOL " " FORCE)
set(TBB_BUILD_TBBMALLOC OFF CACHE BOOL " " FORCE)
set(TBB_BUILD_TBBMALLOC_PROXY OFF CACHE BOOL " " FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext/tbb ext_build/tbb)
set_property(TARGET tbb_static tbb_def_files PROPERTY FOLDER "dependencies")

# Header file directories of dependency libraries
include_directories(
  # Eigen
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/eigen
  # Intel Thread Building Blocks
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/tbb/include
  # Disjoint set data structure
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/dset
  # Parallel stable sort
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/pss
  # Pseudorandom number generator
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/pcg32
  # Resource file
  ${CMAKE_CURRENT_BINARY_DIR}
)

if (INSTANT_MESHES_DEV_MODE)
  add_definitions(-DDEV_MODE)
endif()

add_library(${PROJECT_NAME}
  src/aabb.h
  src/common.h
  src/normal.h src/normal.cpp
  src/adjacency.h src/adjacency.cpp
  src/meshstats.h src/meshstats.cpp
  src/hierarchy.h src/hierarchy.cpp
  src/extract.h src/extract.cpp
  src/field.h src/field.cpp
  src/bvh.h src/bvh.cpp
  src/subdivide.h src/subdivide.cpp
  src/types.h
  src/reorder.h src/reorder.cpp
  src/serializer.h src/serializer.cpp
  src/smoothcurve.h src/smoothcurve.cpp
  src/cleanup.h src/cleanup.cpp
  src/dedge.h src/dedge.cpp
  ${EXTRA_SOURCE})

target_link_libraries(${PROJECT_NAME} tbb_static ${NANOGUI_EXTRA_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC src/)
