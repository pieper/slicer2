# Test

ADD_TEST("SLICERBASE.TestSlicerBase" "${SLICER_EXECUTABLE}" 
  --agree_to_license
    "${CTEST_SLICER_HOME}/Scripts/ImageRegressionTest.tcl"
    "${CTEST_SLICER_HOME}/Base/Testing/testSlicerBase.tcl"
    -D "${CMAKE_CURRENT_SOURCE_DIR}"
    -V Baseline/testSlicerBase.png)

ADD_TEST("SLICERBASE.TestPlot" "${SLICER_EXECUTABLE}" 
  --agree_to_license
  "${CTEST_SLICER_HOME}/Scripts/ImageRegressionTest.tcl"
  "${CTEST_SLICER_HOME}/Base/Testing/TestPlot.tcl"
  -D "${CMAKE_CURRENT_SOURCE_DIR}"
  -V Baseline/ImagePlot.png)

