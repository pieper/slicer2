FILE(GLOB_RECURSE tclfiles "${CMAKE_CURRENT_SOURCE_DIR}/Tcl/*.tcl")
FOREACH(tclfile ${tclfiles})
  STRING(REGEX REPLACE ".*/" "" dtmritestname "${tclfile}")
  STRING(REGEX REPLACE ".*\\\\" "" dtmritestname "${dtmritestname}")
  ADD_TEST("${MODULE_NAME}.${dtmritestname}" "${SLICER_EXECUTABLE}"
    --agree_to_license
    ${VTK_SOURCE_DIR}/Common/Testing/Tcl/rtImageTest.tcl 
    ${tclfile}
    -D ${CMAKE_CURRENT_SOURCE_DIR}
    -V Baseline/${dtmritestname}.png)
ENDFOREACH(tclfile)

SUBDIRS (
  Cxx
  )
