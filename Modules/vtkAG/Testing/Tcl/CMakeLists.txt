SET(TCL_TESTS
  testRigidRegistration   
   )

FOREACH(tclfile ${TCL_TESTS})
  ADD_TEST("${MODULE_NAME}.${tclfile}" "${SLICER_EXECUTABLE}"
    --batch
    "${CTEST_SLICER_HOME}/Scripts/ImageRegressionTest.tcl"
    "${CMAKE_CURRENT_SOURCE_DIR}/${tclfile}.tcl"
    -D "${CMAKE_CURRENT_SOURCE_DIR}")
ENDFOREACH(tclfile)

