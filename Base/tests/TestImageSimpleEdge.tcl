package require vtk
package require vtkSlicerBase

# Image pipeline

vtkImageReader reader
  reader ReleaseDataFlagOff
  reader SetDataByteOrderToLittleEndian
  reader SetDataExtent 0 63 0 63 1 93
  reader SetFilePrefix ${VTK_DATA_ROOT}/Data/headsq/quarter
  reader SetDataMask 0x7fff

# can do vertical or horizontal edges...
vtkImageSimpleEdge edge
edge SetKernelToHorizontal
#edge SetKernelToVertical
edge SetInput [reader GetOutput]

vtkImageViewer viewer
viewer SetInput [edge GetOutput]
viewer SetZSlice 15
viewer SetColorWindow 2000
viewer SetColorLevel 1000

# note: play with the W/L to see dark and light edges.

#make interface
source [file join [file dirname [info script]] WindowLevelInterface.tcl]







