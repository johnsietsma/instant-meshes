cmake_minimum_required (VERSION 2.8.3)

set(PROJECT_NAME InstantMeshes)
project(${PROJECT_NAME})



if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui/ext/glfw")
  message(FATAL_ERROR "The Instant Meshes dependency repositories (NanoGUI, GLFW, etc.) are missing! "
    "You probably did not clone the project with --recursive. It is possible to recover "
    "by calling \"git submodule update --init --recursive\"")
endif()


# Build NanoGUI
set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_SHARED  OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui ext_build/nanogui)
set_property(TARGET nanogui glfw glfw_objects bin2c PROPERTY FOLDER "dependencies")


# Header file directories of dependency libraries
include_directories(
  # Eigen
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui/ext/eigen
  # GLFW
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui/ext/glfw/include
  # Intel Thread Building Blocks
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/tbb/include
  # NanoVG
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui/ext/nanovg/src
  # NanoGUI
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui/include
  # RPLY mesh I/O library
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/rply
  # Half precision type
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/half
  # Pseudorandom number generator
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/pcg32
  # Resource file
  ${CMAKE_CURRENT_BINARY_DIR}
)

# Run bin2c on resource files
set(PNG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/comb.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/delete_stroke.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/loadmesh.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/scare.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/sing_dir.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/sing_pos.png
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/instantmeshes.png)
file(GLOB FRAG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.frag")
file(GLOB VERT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.vert")
file(GLOB GEO_FILES  "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.geo")
set(IM_RESOURCES ${PNG_FILES} ${FRAG_FILES} ${VERT_FILES} ${GEO_FILES})

add_custom_command(
  OUTPUT resources.cpp
  COMMAND bin2c ARGS resources.cpp resources.h ${IM_RESOURCES}
  DEPENDS bin2c ${IM_RESOURCES}
  COMMENT "Running bin2c"
  PRE_BUILD VERBATIM)

if (INSTANT_MESHES_DEV_MODE)
  add_definitions(-DDEV_MODE)
endif()

# Resource file (icons etc.)
set(EXTRA_SOURCE "")
if (APPLE)
  set(EXTRA_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/resources/im.icns")
elseif(WIN32)
  set(EXTRA_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/resources/im.rc")
endif()

add_executable(${PROJECT_NAME} MACOSX_BUNDLE
  resources.cpp
  src/glutil.h src/glutil.cpp
  src/gui_serializer.h
  src/batch.h src/batch.cpp
  src/widgets.h src/widgets.cpp
  src/main.cpp
  src/meshio.h src/meshio.cpp
  src/viewer.h src/viewer.cpp
  src/ply_serializer.h src/ply_serializer.cpp
  ext/rply/rply.c
  ${EXTRA_SOURCE})

target_link_libraries(${PROJECT_NAME} InstantMeshesLib nanogui ${NANOGUI_EXTRA_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Instant Meshes")

if (WIN32)
  # Quench some warnings on MSVC
  if (MSVC)
    set_source_files_properties(ext/rply/rply.c PROPERTIES COMPILE_FLAGS "/wd4127")
  endif()
elseif (APPLE)
  # Build an application bundle on OSX
  set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Instant Meshes")
  set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_GUI_IDENTIFIER "com.im.InstantMeshes")
  set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_ICON_FILE im.icns)
  set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/MacOSXBundleInfo.plist.in)
  set_source_files_properties(resources/im.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
else()
  # Insulate from a few types of ABI changes by statically linking against libgcc and libstdc++
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static-libgcc")
  #set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")
endif()

if (UNIX AND NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND strip $<TARGET_FILE:${PROJECT_NAME}>)
endif()
