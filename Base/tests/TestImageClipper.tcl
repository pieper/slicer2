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

vtkImageClipper clipper
clipper SetInput [mag GetOutput]
clipper SetOutputWholeExtent 0 150 0 150 0 150 

vtkImageViewer viewer
viewer SetInput [clipper GetOutput]
viewer SetZSlice 22
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source [file join [file dirname [info script]] WindowLevelInterface.tcl]







