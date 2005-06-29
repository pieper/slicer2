FILE(GLOB_RECURSE tclfiles "${CMAKE_CURRENT_SOURCE_DIR}/Tcl/*.tcl")
FOREACH(tclfile ${tclfiles})
  STRING(REGEX REPLACE ".*/" "" dtmritestname "${tclfile}")
  STRING(REGEX REPLACE ".*\\\\" "" dtmritestname "${dtmritestname}")
  ADD_TEST("${MODULE_NAME}.${dtmritestname}" "${SLICER_EXECUTABLE}" --agree_to_license --no-tkcon --script ${tclfile} --exec exit)
ENDFOREACH(tclfile)

SUBDIRS (
  Cxx
  )
