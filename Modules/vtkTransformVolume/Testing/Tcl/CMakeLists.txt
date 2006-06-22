SET(TCL_TESTS
  TestTransformVolume
   )

FOREACH(tclfile ${TCL_TESTS})
  ADD_TEST("${MODULE_NAME}.${tclfile}"
  "${CTEST_SLICER_HOME}/Scripts/runvnc" "--Xvnc"
  "${CTEST_XVNC_EXECUTABLE}" "-d" "44" "--not-secure" "--" 
  "${SLICER_EXECUTABLE}" "--batch"
  "${CTEST_SLICER_HOME}/Scripts/ImageRegressionTest.tcl" "${CMAKE_CURRENT_SOURCE_DIR}/${tclfile}.tcl"
   "-D" "${CMAKE_CURRENT_SOURCE_DIR}")
ENDFOREACH(tclfile)

