package require vtk
package require vtkSlicerBase

# Image pipeline

vtkImageReader reader
  reader ReleaseDataFlagOff
  reader SetDataByteOrderToLittleEndian
  reader SetDataExtent 0 63 0 63 1 93
  reader SetFilePrefix ${VTK_DATA_ROOT}/Data/headsq/quarter
  reader SetDataMask 0x7fff

vtkImageGradientMagnitude grad
grad SetInput [reader GetOutput]
grad SetDimensionality 3

vtkImageViewer viewer
viewer SetInput [reader GetOutput]
viewer SetZSlice 22
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl
