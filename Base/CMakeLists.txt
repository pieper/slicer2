
#
# Slicer loadable vtk project for vtk4
# - adapted from the vtkMy example
#
# - sp 2002-04-19
# - updated based on slicer developer feedback with
# help from MH.  2002-05-02
# 
#

PROJECT (VTKSLICER)

# If some requirements are not met, set it to 0.
# In that case, the build won't be done.
#
OPTION(VTKSLICER_CAN_BUILD "Build slicer: turn off for dashboard manager" ON)

#
# If everything is OK, then go into the sub directories and build.
#

IF (VTKSLICER_CAN_BUILD)

# MH+KH - moved this line into the "CAN_BUILD" IF so that dashboard work
INCLUDE (${VTKSLICER_SOURCE_DIR}/CMakeOptions.cmake)

  #
  # Here is where you can list the sub-directories holding your local 
  # classes. Sorting classes by 'package' type like VTK does (Common, 
  # Rendering, Filtering, Imaging, IO, etc.) is a good thing and prevents 
  # numerous dependencies problems.
  #

  SUBDIRS (
    # sp - put everything in cxx for now
    cxx
  )

  #
  # You can put your include path(s) here
  #

  INCLUDE_DIRECTORIES (${VTKSLICER_SOURCE_DIR}/cxx)

  #
  # Common configuration settings
  #
  # Do not worry about this one.
  #

  CONFIGURE_FILE(
    ${VTKSLICER_SOURCE_DIR}/vtkSlicerConfigure.h.in 
    ${VTKSLICER_BINARY_DIR}/vtkSlicerConfigure.h
  )

  INCLUDE_DIRECTORIES(${VTKSLICER_BINARY_DIR})

# include a test for endianness (used to be done by default)
INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

ENDIF (VTKSLICER_CAN_BUILD)

# Include the standard Dart testing module
INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
