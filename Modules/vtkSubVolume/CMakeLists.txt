#
# Set the project name.
#

# MY -> SUBVOLUME
# my -> SubVolume


PROJECT (VTKSUBVOLUME)

#
# Assume everything is set up correctly for build.
#
# If some requirements are not met, set it to 0.
# In that case, the build won't be done.
#

SET (VTKSUBVOLUME_CAN_BUILD 1)

#
# Load CMake commands that you probably should not modify.
#

INCLUDE (${VTKSUBVOLUME_SOURCE_DIR}/CMakeOptions.cmake)

#
# find the slicer version - you can either set default here, or 
# configure them with CMakeSetup/ccmake/cmake as you wish.
# If you use the cmaker.tcl script in slicer2/Scripts these
# will be filled in automatically with the correct values.
# These are used is your module relies on code in slicerbase.
#
# e.g. c:/pieper/bwh/slicer2/latest/slicer2/Base
SET (VTKSLICERBASE_SOURCE_DIR CACHE PATH
    "should end with slicer2/Base"
)
# e.g. c:/pieper/bwh/slicer2/latest/slicer2/Base/builds/Win32VC7 CACHE PATH
SET (VTKSLICERBASE_BUILD_DIR CACHE PATH
    "should end with slicer2/Base/builds/xxx where xxx is the same build name you are making here"
)
# e.g. c:/pieper/bwh/slicer2/latest/slicer2/Base/builds/Win32VC7/bin/debug/vtkSlicerBase.lib CACHE PATH
SET (VTKSLICERBASE_BUILD_LIB CACHE PATH
    "name of the vtkSlicerBase lib file (.lib or .a)"
)


#
# If everything is OK, then go into the sub directories and build.
#

IF (VTKSUBVOLUME_CAN_BUILD)

  #
  # Here is where you can list the sub-directories holding your local 
  # classes. Sorting classes by 'package' type like VTK does (Common, 
  # Rendering, Filtering, Imaging, IO, etc.) is a good thing and prevents 
  # numerous dependencies problems.
  #

  SUBDIRS (
    cxx
  )

  #
  # You can put your include path(s) here
  #

  INCLUDE_DIRECTORIES (${VTKSUBVOLUME_SOURCE_DIR}/cxx)

  #
  # Build examples too ?
  #

#  OPTION(BUILD_EXAMPLES "Build examples." ON)
#  IF (BUILD_EXAMPLES)
#    SUBDIRS(Examples)
#  ENDIF (BUILD_EXAMPLES)


  #
  # Common configuration settings
  #
  # Do not worry about this one.
  #

  CONFIGURE_FILE(
    ${VTKSUBVOLUME_SOURCE_DIR}/vtkSubVolumeConfigure.h.cin 
    ${VTKSUBVOLUME_BINARY_DIR}/vtkSubVolumeConfigure.h
  )

  INCLUDE_DIRECTORIES(${VTKSUBVOLUME_BINARY_DIR})

ENDIF (VTKSUBVOLUME_CAN_BUILD)
