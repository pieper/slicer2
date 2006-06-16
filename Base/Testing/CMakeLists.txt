# Test
SET(TESTS_BASE_SRCS
  testSlicerBase
  testDataProvenance
  TestPlot
  TestImageCCA
  )

FOREACH(test ${TESTS_BASE_SRCS})
  ADD_TEST("SLICERBASE.${test}" "${SLICER_EXECUTABLE}" 
    --batch
    "${CTEST_SLICER_HOME}/Scripts/ImageRegressionTest.tcl"
    "${CTEST_SLICER_HOME}/Base/Testing/${test}.tcl"
    -D "${CMAKE_CURRENT_SOURCE_DIR}"
    -V "Baseline/${test}.png")
ENDFOREACH(test)

SET(TCL_TESTS   
  TestMeasureVol
  TestMaskStat
   )                    

FOREACH(tclfile ${TCL_TESTS})
  ADD_TEST("SLICERBASE.${tclfile}" "${SLICER_EXECUTABLE}"
    --batch
    "${CTEST_SLICER_HOME}/Scripts/ImageRegressionTest.tcl"
    "${CTEST_SLICER_HOME}/Base/Testing/${tclfile}.tcl"     
    -D "${CMAKE_CURRENT_SOURCE_DIR}")
ENDFOREACH(tclfile)     
