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

vtkMatrix4x4 m
m SetElement 0 0 -0.16214
m SetElement 0 1 0.914434
m SetElement 0 2 0.370837
m SetElement 0 3 0

m SetElement 1 0 -0.454903
m SetElement 1 1 -0.402761
m SetElement 1 2 0.794258
m SetElement 1 3 0

m SetElement 2 0 0.875656
m SetElement 2 1 -0.0399135
m SetElement 2 2 0.4812830
m SetElement 2 3 0

m SetElement 3 0 0
m SetElement 3 1 0
m SetElement 3 2 0
m SetElement 3 3 1

# RastoIjk
vtkMatrix4x4 m2
m2 SetElement 0 0 -1.06667
m2 SetElement 0 1 0
m2 SetElement 0 2 0
m2 SetElement 0 3 128

m2 SetElement 1 0 0
m2 SetElement 1 1 1.06667
m2 SetElement 1 2 0
m2 SetElement 1 3 106.987

m2 SetElement 2 0 0
m2 SetElement 2 1 0 
m2 SetElement 2 2 -0.5
m2 SetElement 2 3 29.75

m2 SetElement 3 0 0 
m2 SetElement 3 1 0 
m2 SetElement 3 2 0 
m2 SetElement 3 3 1 

vtkImageClipInteractive clipint
clipint SetInput [mag GetOutput]
clipint SetReformatMatrix m
clipint SetWldToIjkMatrix m2
clipint SetClipExtent 30 125 60 185 1 93

vtkImageViewer viewer
viewer SetInput [clipint GetOutput]
viewer SetZSlice 22
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source [file join [file dirname [info script]] WindowLevelInterface.tcl]








