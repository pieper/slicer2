package require vtk
package require vtkSlicerBase

# Image pipeline

vtkImageReader reader
  reader ReleaseDataFlagOff
  reader SetDataByteOrderToLittleEndian
  reader SetDataExtent 0 63 0 63 1 93
  reader SetFilePrefix ${VTK_DATA_ROOT}/Data/headsq/quarter
  reader SetDataMask 0x7fff

vtkImageMagnify mag
  mag SetInput [reader GetOutput]
  mag SetMagnificationFactors 4 4 1

vtkImageCast cast
cast SetInput [mag GetOutput]
cast SetOutputScalarTypeToFloat

vtkImageWeightedSum sum
sum SetInput 0 [cast GetOutput]
sum SetInput 1 [cast GetOutput]
puts "set inputs to sum"

vtkImageViewer viewer
viewer SetInput [sum GetOutput]
viewer SetZSlice 22
viewer SetColorWindow 2000
viewer SetColorLevel 1000

puts "grabbed sum's output"

sum SetWeightForInput 0 1
puts "---"
#sum SetWeightForInput 1 2

#make interface
source WindowLevelInterface.tcl

