SET(BUILD_SHARED_LIBS "${SLICERBASE_BUILD_SHARED_LIBS}")
INCLUDE_DIRECTORIES(${SLICERBASE_INCLUDE_DIRS})
LINK_DIRECTORIES(${SLICERBASE_LIBRARY_DIRS})
INCLUDE("${USE_VTK_FILE}")
