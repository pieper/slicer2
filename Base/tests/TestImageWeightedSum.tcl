catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

vtkImageCast cast
cast SetInput [reader GetOutput]
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

