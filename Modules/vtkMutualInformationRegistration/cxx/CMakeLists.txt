SET (LOCAL_LIBRARY_BASENAME vtkMutualInformationRegistration)

INCLUDE (${VTKMUTUALINFORMATIONREGISTRATION_SOURCE_DIR}/cxx/CMakeListsLocal.txt)

# --------------------------------------------------------------------------
# You probably do not need to modify anything below this line

#
# Create the vtkMutualInformationRegistration C++ library
#
# (Doesn't work here because of a CMake bug)
# ADD_LIBRARY (vtkMutualInformationRegistration LOCAL_MODULE_SRCS)

#
# Create the vtkMutualInformationRegistration Tcl library
#

IF (VTK_WRAP_TCL AND VTKMUTUALINFORMATIONREGISTRATION_WRAP_TCL)

  VTK_WRAP_TCL (vtkMutualInformationRegistrationTCL MutualInformationRegistrationTCL_SRCS LOCAL_MODULE_SRCS)
  ADD_LIBRARY (vtkMutualInformationRegistrationTCL MutualInformationRegistrationTCL_SRCS)
  TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationTCL 
                         vtkIOTCL 
                         vtkGraphicsTCL 
                         vtkImagingTCL 
                         vtkFilteringTCL 
                         vtkCommonTCL
                         ${TCL_LIBRARY})
  IF (VTK_USE_HYBRID)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationTCL vtkHybridTCL)
  ENDIF (VTK_USE_HYBRID)

  IF (VTK_USE_PARALLEL)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationTCL vtkParallelTCL)
  ENDIF (VTK_USE_PARALLEL)

  IF (VTK_USE_PATENTED)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationTCL vtkPatentedTCL)
  ENDIF (VTK_USE_PATENTED)

  IF (VTK_USE_RENDERING)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationTCL vtkRenderingTCL)
  ENDIF (VTK_USE_RENDERING)

ENDIF (VTK_WRAP_TCL AND VTKMUTUALINFORMATIONREGISTRATION_WRAP_TCL)

#
# Create the vtkMutualInformationRegistration Python library
#

IF (VTK_WRAP_PYTHON AND VTKMUTUALINFORMATIONREGISTRATION_WRAP_PYTHON)

  VTK_WRAP_PYTHON (vtkMutualInformationRegistrationPython MutualInformationRegistrationPython_SRCS LOCAL_MODULE_SRCS)

  IF (APPLE)
    ADD_LIBRARY (vtkMutualInformationRegistrationPython MODULE MutualInformationRegistrationPython_SRCS)
  ELSE (APPLE)
    ADD_LIBRARY (vtkMutualInformationRegistrationPython SHARED MutualInformationRegistrationPython_SRCS)
  ENDIF (APPLE)

  IF (WIN32)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationPython 
                           vtkIOPython
                           vtkGraphicsPython
                           vtkImagingPython
                           vtkFilteringPython
                           vtkCommonPython
                           debug ${PYTHON_DEBUG_LIBRARY} 
                           optimized ${PYTHON_LIBRARY})
  ELSE (WIN32)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationPython 
                           vtkIOPython
                           vtkGraphicsPython
                           vtkImagingPython
                           vtkFilteringPython
                           vtkCommonPython
                           ${PYTHON_LIBRARY})
  ENDIF (WIN32)

  IF (VTK_USE_HYBRID)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationPython vtkHybridPython)
  ENDIF (VTK_USE_HYBRID)

  IF (VTK_USE_PARALLEL)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationPython vtkParallelPython)
  ENDIF (VTK_USE_PARALLEL)

  IF (VTK_USE_PATENTED)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationPython vtkPatentedPython)
  ENDIF (VTK_USE_PATENTED)

  IF (VTK_USE_RENDERING)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationPython vtkRenderingPython)
  ENDIF (VTK_USE_RENDERING)

ENDIF (VTK_WRAP_PYTHON AND VTKMUTUALINFORMATIONREGISTRATION_WRAP_PYTHON)

#
# Create the vtkMutualInformationRegistration Java library
#

IF (VTK_WRAP_JAVA AND VTKMUTUALINFORMATIONREGISTRATION_WRAP_JAVA)

  VTK_WRAP_JAVA (vtkMutualInformationRegistrationJava MutualInformationRegistrationJava_SRCS LOCAL_MODULE_SRCS)
  ADD_LIBRARY (vtkMutualInformationRegistrationJava SHARED MutualInformationRegistrationJava_SRCS)
  TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationJava 
                         vtkIOJava
                         vtkGraphicsJava
                         vtkImagingJava 
                         vtkFilteringJava
                         vtkCommonJava)
  IF (VTK_USE_HYBRID)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationJava vtkHybridJava)
  ENDIF (VTK_USE_HYBRID)

  IF (VTK_USE_PARALLEL)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationJava vtkParallelJava)
  ENDIF (VTK_USE_PARALLEL)

  IF (VTK_USE_PATENTED)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationJava vtkPatentedJava)
  ENDIF (VTK_USE_PATENTED)

  IF (VTK_USE_RENDERING)
    TARGET_LINK_LIBRARIES (vtkMutualInformationRegistrationJava vtkRenderingJava)
  ENDIF (VTK_USE_RENDERING)

ENDIF (VTK_WRAP_JAVA AND VTKMUTUALINFORMATIONREGISTRATION_WRAP_JAVA)

#
# Link to VTK and local
#

LINK_LIBRARIES (
  vtkMutualInformationRegistration
  vtkIO
  vtkGraphics
  vtkImaging 
  vtkFiltering
  vtkCommon
)

IF (VTK_USE_HYBRID)
  LINK_LIBRARIES (vtkHybrid)
ENDIF (VTK_USE_HYBRID)

IF (VTK_USE_PARALLEL)
  LINK_LIBRARIES (vtkParallel)
ENDIF (VTK_USE_PARALLEL)

IF (VTK_USE_PATENTED)
  LINK_LIBRARIES (vtkPatented)
ENDIF (VTK_USE_PATENTED)

IF (VTK_USE_RENDERING)
  LINK_LIBRARIES (vtkRendering)
ENDIF (VTK_USE_RENDERING)
