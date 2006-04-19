# Test

ADD_TEST("SLICERBASE.TestSlicerBase" "${SLICER_EXECUTABLE}" 
  --agree_to_license --no-tkcon --script ${CTEST_SLICER_HOME}/Base/Testing/testSlicerBase.tcl 
  --exec exit)

