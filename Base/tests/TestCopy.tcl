package require vtk
package require vtkSlicerBase

# Image pipeline

vtkImageReader reader
  reader ReleaseDataFlagOff
  reader SetDataByteOrderToLittleEndian
  reader SetDataExtent 0 63 0 63 1 93
  reader SetFilePrefix ${VTK_DATA_ROOT}/Data/headsq/quarter
  reader SetDataMask 0x7fff

vtkImageCopy copy
  copy SetInput [reader GetOutput]
  copy ClearOff

vtkImageMagnify mag
  mag SetInput [copy GetOutput]
  mag SetMagnificationFactors 4 4 1

vtkImageViewer viewer
  viewer SetInput [mag GetOutput]
  viewer SetZSlice 22
  viewer SetColorWindow 2000
  viewer SetColorLevel 1000

#make interface
source [file join [file dirname [info script]] WindowLevelInterface.tcl]





