#
# Source files: DO NOT EDIT THIS FILE: edit CMakeListsLocal.txt instead.
#
#

SET (LOCAL_LIBRARY_BASENAME vtkQueryAtlas)

INCLUDE (${VTKQUERYATLAS_SOURCE_DIR}/cxx/CMakeListsLocal.txt)

# --------------------------------------------------------------------------
# You probably do not need to modify anything below this line

#
# Create the vtkQueryAtlas C++ library
#
# (Doesn't work here because of a CMake bug)
# ADD_LIBRARY (vtkQueryAtlas ${LOCAL_MODULE_SRCS})

#
# Create the vtkQueryAtlas Tcl library
#

IF (VTK_WRAP_TCL AND VTKQUERYATLAS_WRAP_TCL)

  VTK_WRAP_TCL (vtkQueryAtlasTCL QueryAtlasTCL_SRCS ${LOCAL_MODULE_SRCS})
  ADD_LIBRARY (vtkQueryAtlasTCL ${QueryAtlasTCL_SRCS})
  TARGET_LINK_LIBRARIES (vtkQueryAtlasTCL 
                         vtkQueryAtlas
                         vtkIOTCL 
                         vtkGraphicsTCL 
                         vtkImagingTCL 
                         vtkFilteringTCL 
                         vtkCommonTCL
                         ${TCL_LIBRARY})
  IF (VTK_USE_HYBRID)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasTCL vtkHybridTCL)
  ENDIF (VTK_USE_HYBRID)

  IF (VTK_USE_PARALLEL)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasTCL vtkParallelTCL)
  ENDIF (VTK_USE_PARALLEL)

  IF (VTK_USE_PATENTED)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasTCL vtkPatentedTCL)
  ENDIF (VTK_USE_PATENTED)

  IF (VTK_USE_RENDERING)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasTCL vtkRenderingTCL)
  ENDIF (VTK_USE_RENDERING)

ENDIF (VTK_WRAP_TCL AND VTKQUERYATLAS_WRAP_TCL)

#
# Create the vtkQueryAtlas Python library
#

IF (VTK_WRAP_PYTHON AND VTKQUERYATLAS_WRAP_PYTHON)

  VTK_WRAP_PYTHON (vtkQueryAtlasPython QueryAtlasPython_SRCS ${LOCAL_MODULE_SRCS})

  IF (APPLE)
    ADD_LIBRARY (vtkQueryAtlasPython MODULE ${QueryAtlasPython_SRCS})
  ELSE (APPLE)
    ADD_LIBRARY (vtkQueryAtlasPython SHARED ${QueryAtlasPython_SRCS})
  ENDIF (APPLE)

  IF (WIN32)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasPython 
                           vtkQueryAtlas
                           vtkIOPython
                           vtkGraphicsPython
                           vtkImagingPython
                           vtkFilteringPython
                           vtkCommonPython
                           debug ${PYTHON_DEBUG_LIBRARY} 
                           optimized ${PYTHON_LIBRARY})
  ELSE (WIN32)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasPython 
                           vtkQueryAtlasPython 
                           vtkIOPython
                           vtkGraphicsPython
                           vtkImagingPython
                           vtkFilteringPython
                           vtkCommonPython
                           ${PYTHON_LIBRARY})
  ENDIF (WIN32)

  IF (VTK_USE_HYBRID)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasPython vtkHybridPython)
  ENDIF (VTK_USE_HYBRID)

  IF (VTK_USE_PARALLEL)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasPython vtkParallelPython)
  ENDIF (VTK_USE_PARALLEL)

  IF (VTK_USE_PATENTED)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasPython vtkPatentedPython)
  ENDIF (VTK_USE_PATENTED)

  IF (VTK_USE_RENDERING)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasPython vtkRenderingPython)
  ENDIF (VTK_USE_RENDERING)

ENDIF (VTK_WRAP_PYTHON AND VTKQUERYATLAS_WRAP_PYTHON)

#
# Create the vtkQueryAtlas Java library
#

IF (VTK_WRAP_JAVA AND VTKQUERYATLAS_WRAP_JAVA)

  VTK_WRAP_JAVA (vtkQueryAtlasJava QueryAtlasJava_SRCS ${LOCAL_MODULE_SRCS})
  ADD_LIBRARY (vtkQueryAtlasJava SHARED ${QueryAtlasJava_SRCS})
  TARGET_LINK_LIBRARIES (vtkQueryAtlasJava 
                         vtkQueryAtlas
                         vtkIOJava
                         vtkGraphicsJava
                         vtkImagingJava 
                         vtkFilteringJava
                         vtkCommonJava)
  IF (VTK_USE_HYBRID)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasJava vtkHybridJava)
  ENDIF (VTK_USE_HYBRID)

  IF (VTK_USE_PARALLEL)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasJava vtkParallelJava)
  ENDIF (VTK_USE_PARALLEL)

  IF (VTK_USE_PATENTED)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasJava vtkPatentedJava)
  ENDIF (VTK_USE_PATENTED)

  IF (VTK_USE_RENDERING)
    TARGET_LINK_LIBRARIES (vtkQueryAtlasJava vtkRenderingJava)
  ENDIF (VTK_USE_RENDERING)

ENDIF (VTK_WRAP_JAVA AND VTKQUERYATLAS_WRAP_JAVA)

#
# Link to VTK and local
#

TARGET_LINK_LIBRARIES (
  vtkQueryAtlas
  vtkIO
  vtkGraphics
  vtkImaging 
  vtkFiltering
  vtkCommon
)

IF (VTK_USE_HYBRID)
  TARGET_LINK_LIBRARIES (vtkQueryAtlas vtkHybrid)
ENDIF (VTK_USE_HYBRID)

IF (VTK_USE_PARALLEL)
  TARGET_LINK_LIBRARIES (vtkQueryAtlas vtkParallel)
ENDIF (VTK_USE_PARALLEL)

IF (VTK_USE_PATENTED)
  TARGET_LINK_LIBRARIES (vtkQueryAtlas vtkPatented)
ENDIF (VTK_USE_PATENTED)

IF (VTK_USE_RENDERING)
  TARGET_LINK_LIBRARIES (vtkQueryAtlas vtkRendering)
ENDIF (VTK_USE_RENDERING)
