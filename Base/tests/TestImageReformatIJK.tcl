catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

vtkImageReformatIJK ref
ref SetInput [reader GetOutput]
ref SetInputOrderString SI
ref SetOutputOrderString SI
ref SetSlice 22
# this function is not getting wrapped (why?)
#ref ComputeTransform
ref ComputeTransform2
ref ComputeOutputExtent
ref Update

vtkImageViewer viewer
viewer SetInput [ref GetOutput]
#viewer SetZSlice 0
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl

set ijk "10 10 10"
puts "ijk = $ijk"
eval ref SetIJKPoint $ijk
puts "xy = [ref GetXYPoint]"

