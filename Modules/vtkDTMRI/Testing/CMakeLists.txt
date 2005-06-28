FILE(GLOB_RECURSE tclfiles "${CMAKE_CURRENT_SOURCE_DIR}/Tcl/*.tcl")
FOREACH(tclfile ${tclfiles})
  STRING(REGEX REPLACE "/" "_" dtmritestname "${tclfile}")
  ADD_TEST("${CMAKE_PROJECT_NAME}-tcl-${dtmritestname}" "${SLICER_EXECUTABLE}" --agree_to_license --no-tkcon --script ${tclfile} --exec exit)
ENDFOREACH(tclfile)

SUBDIRS (
  Cxx
  )
