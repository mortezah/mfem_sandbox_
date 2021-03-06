#This file shows how to link to a MFEM (and PUMI)
#installation using CMake
#it represents a simple 'CMakeLists.txt'
#file for a new project

cmake_minimum_required(VERSION 3.0.0)

project(mfem_clis VERSION 1.0.0 LANGUAGES CXX)

# Starting here are the critical lines:
include(cmake/bob.cmake)

bob_begin_package()


# Let CMake know where to find custom FindFoo.cmake files
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Allow the user to indicate where they installed SCOREC
# via "-DSCOREC_PREFIX=/home/somewhere" when calling `cmake`
set(SCOREC_PREFIX "" CACHE STRING "Directory where SCOREC is installed")

# If SCOREC_PREFIX was specified, only link to that directory,
# i.e. don't link to another installation in /usr/lib by mistake
if (SCOREC_PREFIX)
  find_package(SCOREC 2.1.0 REQUIRED CONFIG PATHS ${SCOREC_PREFIX} NO_DEFAULT_PATH)
else()
# IF SCOREC_PREFIX was not specified, look in typical system directories,
# and also in CMAKE_PREFIX_PATH (environment variable)
  find_package(
      SCOREC #package name, has to be SCOREC
      2.1.0  #version. can be omitted, and will match any installed version
             #greater than or equal to this one, as long as the major number
             #is the same
      REQUIRED #indicate that SCOREC is really needed to compile
      CONFIG   #skip the 'MODULE' search system, save some time and confusion
      )
endif()

if (MFEM_PREFIX)
find_package(MFEM REQUIRED CONFIG PATHS ${MFEM_PREFIX} NO_DEFAULT_PATH)
else()
# IF MFEM_PREFIX was not specified, look in typical system directories,
# and also in CMAKE_PREFIX_PATH (environment variable)
find_package(
    MFEM   #package name, has to be MFEM
    REQUIRED #indicate that MFEM is really needed to compile
    CONFIG   #skip the 'MODULE' search system, save some time and confusion
    )
endif()


find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

#This is core-sim, so bring in Simmetrix !
#(needed by apf_sim and gmi_sim)
if(ENABLE_SIMMETRIX)
  find_package(SimModSuite MODULE REQUIRED)
  add_definitions(-DHAVE_SIMMETRIX)
endif()

message(STATUS "MFEM INCLUDE is ${MFEM_INCLUDE_DIRS} ")
message(STATUS "MFEM LIBRARY is ${MFEM_LIBRARY_DIR} ")

macro(setup_exe exename srcname)
  add_executable(${exename} ${srcname})
  target_link_libraries(${exename} PUBLIC SCOREC::core)
  target_link_libraries(${exename} PUBLIC mfem)
  target_link_libraries(${exename} PUBLIC blas)
  target_link_libraries(${exename} PUBLIC lapack)
  install(TARGETS ${exename} DESTINATION bin)
endmacro(setup_exe)

include_directories(${SIMMODSUITE_INCLUDE_DIR})
include_directories(${MFEM_INCLUDE_DIRS})

## executables that do not use simmetrix go here
setup_exe(mfem_2_vtk mfem_2_vtk.cpp)

if(ENABLE_SIMMETRIX)
## executables that do     use simmetrix go here
  setup_exe(pumi_2_mfem pumi_2_mfem.cpp)
endif()

bob_end_package()
