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

set resizeExt "0 149 0 149 0 0"
set clipExt "100 109 100 109 0 0"
vtkImageResize resize
  resize SetInput [mag GetOutput]
#  eval resize SetInputClipExtent $clipExt
  eval resize SetOutputWholeExtent $resizeExt
  resize Update
 
vtkImageViewer viewer
viewer SetInput [resize GetOutput]
viewer SetZSlice 0 
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl







