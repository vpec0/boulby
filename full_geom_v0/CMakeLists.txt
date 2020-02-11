#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(full_geom_v0)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

# ROOT includes
# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
if(EXISTS $ENV{ROOTSYS}/cmake)
  list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS}/cmake)
elseif(EXISTS $ENV{ROOTSYS}/share/root/cmake)
  list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS}/share/root/cmake)
endif()


#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

include_directories(${ROOT_INCLUDE_DIRS})

# Set compilation flags
# to suppress warning message:
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable -Wno-unused-parameter -Wno-unused-private-field -Wno-shadow")


#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(full_geom_v0 full_geom_v0.cc ${sources} ${headers})
target_link_libraries(full_geom_v0 ${Geant4_LIBRARIES})
target_link_libraries(full_geom_v0 ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(FULL_GEOM_V0_SCRIPTS
  init_vis.mac
  run1.mac
  run2.mac
  runTest.mac
  )

foreach(_script ${FULL_GEOM_V0_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

file(GLOB vis_macros RELATIVE ${PROJECT_SOURCE_DIR}/ ${PROJECT_SOURCE_DIR}/vis*.mac)
foreach(_macro ${vis_macros})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_macro}
    ${PROJECT_BINARY_DIR}/${_macro}
    COPYONLY
    )
endforeach()


#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(full_geom_v0 DEPENDS full_geom_v0)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS full_geom_v0 DESTINATION bin)
