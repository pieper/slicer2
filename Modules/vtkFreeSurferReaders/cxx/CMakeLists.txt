#
# Source files: DO NOT EDIT THIS FILE: edit CMakeListsLocal.txt instead.
#
#

SET (LOCAL_LIBRARY_BASENAME vtkFreeSurferReaders)

INCLUDE (${VTKFREESURFERREADERS_SOURCE_DIR}/cxx/CMakeListsLocal.txt)

# --------------------------------------------------------------------------
# You probably do not need to modify anything below this line

#
# Create the vtkFreeSurferReaders C++ library
#
# (Doesn't work here because of a CMake bug)
# ADD_LIBRARY (vtkFreeSurferReaders ${LOCAL_MODULE_SRCS})

#
# Create the vtkFreeSurferReaders Tcl library
#

IF (VTK_WRAP_TCL AND VTKFREESURFERREADERS_WRAP_TCL)

  VTK_WRAP_TCL (vtkFreeSurferReadersTCL FreeSurferReadersTCL_SRCS ${LOCAL_MODULE_SRCS})
  ADD_LIBRARY (vtkFreeSurferReadersTCL ${FreeSurferReadersTCL_SRCS})
  TARGET_LINK_LIBRARIES (vtkFreeSurferReadersTCL 
                         vtkFreeSurferReaders
                         vtkIOTCL 
                         vtkGraphicsTCL 
                         vtkImagingTCL 
                         vtkFilteringTCL 
                         vtkCommonTCL
                         ${TCL_LIBRARY})
  IF (VTK_USE_HYBRID)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersTCL vtkHybridTCL)
  ENDIF (VTK_USE_HYBRID)

  IF (VTK_USE_PARALLEL)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersTCL vtkParallelTCL)
  ENDIF (VTK_USE_PARALLEL)

  IF (VTK_USE_PATENTED)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersTCL vtkPatentedTCL)
  ENDIF (VTK_USE_PATENTED)

  IF (VTK_USE_RENDERING)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersTCL vtkRenderingTCL)
  ENDIF (VTK_USE_RENDERING)

ENDIF (VTK_WRAP_TCL AND VTKFREESURFERREADERS_WRAP_TCL)

#
# Create the vtkFreeSurferReaders Python library
#

IF (VTK_WRAP_PYTHON AND VTKFREESURFERREADERS_WRAP_PYTHON)

  VTK_WRAP_PYTHON (vtkFreeSurferReadersPython FreeSurferReadersPython_SRCS ${LOCAL_MODULE_SRCS})

  IF (APPLE)
    ADD_LIBRARY (vtkFreeSurferReadersPython MODULE ${FreeSurferReadersPython_SRCS})
  ELSE (APPLE)
    ADD_LIBRARY (vtkFreeSurferReadersPython SHARED ${FreeSurferReadersPython_SRCS})
  ENDIF (APPLE)

  IF (WIN32)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersPython 
                           vtkFreeSurferReaders
                           vtkIOPython
                           vtkGraphicsPython
                           vtkImagingPython
                           vtkFilteringPython
                           vtkCommonPython
                           debug ${PYTHON_DEBUG_LIBRARY} 
                           optimized ${PYTHON_LIBRARY})
  ELSE (WIN32)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersPython 
                           vtkFreeSurferReadersPython 
                           vtkIOPython
                           vtkGraphicsPython
                           vtkImagingPython
                           vtkFilteringPython
                           vtkCommonPython
                           ${PYTHON_LIBRARY})
  ENDIF (WIN32)

  IF (VTK_USE_HYBRID)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersPython vtkHybridPython)
  ENDIF (VTK_USE_HYBRID)

  IF (VTK_USE_PARALLEL)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersPython vtkParallelPython)
  ENDIF (VTK_USE_PARALLEL)

  IF (VTK_USE_PATENTED)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersPython vtkPatentedPython)
  ENDIF (VTK_USE_PATENTED)

  IF (VTK_USE_RENDERING)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersPython vtkRenderingPython)
  ENDIF (VTK_USE_RENDERING)

ENDIF (VTK_WRAP_PYTHON AND VTKFREESURFERREADERS_WRAP_PYTHON)

#
# Create the vtkFreeSurferReaders Java library
#

IF (VTK_WRAP_JAVA AND VTKFREESURFERREADERS_WRAP_JAVA)

  VTK_WRAP_JAVA (vtkFreeSurferReadersJava FreeSurferReadersJava_SRCS ${LOCAL_MODULE_SRCS})
  ADD_LIBRARY (vtkFreeSurferReadersJava SHARED ${FreeSurferReadersJava_SRCS})
  TARGET_LINK_LIBRARIES (vtkFreeSurferReadersJava 
                         vtkFreeSurferReaders
                         vtkIOJava
                         vtkGraphicsJava
                         vtkImagingJava 
                         vtkFilteringJava
                         vtkCommonJava)
  IF (VTK_USE_HYBRID)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersJava vtkHybridJava)
  ENDIF (VTK_USE_HYBRID)

  IF (VTK_USE_PARALLEL)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersJava vtkParallelJava)
  ENDIF (VTK_USE_PARALLEL)

  IF (VTK_USE_PATENTED)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersJava vtkPatentedJava)
  ENDIF (VTK_USE_PATENTED)

  IF (VTK_USE_RENDERING)
    TARGET_LINK_LIBRARIES (vtkFreeSurferReadersJava vtkRenderingJava)
  ENDIF (VTK_USE_RENDERING)

ENDIF (VTK_WRAP_JAVA AND VTKFREESURFERREADERS_WRAP_JAVA)

#
# Link to VTK and local
#

TARGET_LINK_LIBRARIES (
  vtkFreeSurferReaders
  vtkIO
  vtkGraphics
  vtkImaging 
  vtkFiltering
  vtkCommon
)

IF (VTK_USE_HYBRID)
  TARGET_LINK_LIBRARIES (vtkFreeSurferReaders vtkHybrid)
ENDIF (VTK_USE_HYBRID)

IF (VTK_USE_PARALLEL)
  TARGET_LINK_LIBRARIES (vtkFreeSurferReaders vtkParallel)
ENDIF (VTK_USE_PARALLEL)

IF (VTK_USE_PATENTED)
  TARGET_LINK_LIBRARIES (vtkFreeSurferReaders vtkPatented)
ENDIF (VTK_USE_PATENTED)

IF (VTK_USE_RENDERING)
  TARGET_LINK_LIBRARIES (vtkFreeSurferReaders vtkRendering)
ENDIF (VTK_USE_RENDERING)
