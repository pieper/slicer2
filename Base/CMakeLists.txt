
#
# Slicer loadable vtk project for vtk4
# - adapted from the vtkMy example
#
# - sp 2002-04-19
# - updated based on slicer developer feedback with
# help form MH.  2002-05-02
# 
#


#
# Set the project name.
#

PROJECT (VTKSLICER)

#
# Assume everything is set up correctly for build.
#
# If some requirements are not met, set it to 0.
# In that case, the build won't be done.
#

SET (VTKSLICER_CAN_BUILD 1)

#
# Load CMake commands that you probably should not modify.
#

INCLUDE (${VTKSLICER_SOURCE_DIR}/CMakeOptions.cmake)

#
# If everything is OK, then go into the sub directories and build.
#

IF (VTKSLICER_CAN_BUILD)

  #
  # Here is where you can list the sub-directories holding your local 
  # classes. Sorting classes by 'package' type like VTK does (Common, 
  # Rendering, Filtering, Imaging, IO, etc.) is a good thing and prevents 
  # numerous dependencies problems.
  #

  SUBDIRS (
    # sp - put everything in Core for now
    Core
  )

  #
  # You can put your include path(s) here
  #

  INCLUDE_DIRECTORIES (${VTKSLICER_SOURCE_DIR}/Core)

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

ENDIF (VTKSLICER_CAN_BUILD)
