# Define the tests
SET(TEST_SRCS
  TestInstance
  )

# Add the include paths
#INCLUDE_DIRECTORIES(
#  )

CREATE_TEST_SOURCELIST(TUTests vtkTensorUtilTests.cxx ${TEST_SRCS})
ADD_EXECUTABLE(vtkTensorUtilTests ${TUTests})
TARGET_LINK_LIBRARIES(vtkTensorUtilTests vtkTensorUtil)

# FIXME
SET(VTK_TENSOR_UTIL_TESTS "${EXECUTABLE_OUTPUT_PATH}/vtkTensorUtil")

# Loop over files and create executables
FOREACH(name ${TEST_SRCS})
  ADD_TEST(${name} ${VTK_TENSOR_UTIL_TESTS} ${name})
ENDFOREACH(name)


