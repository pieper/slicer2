package require vtk
package require vtkSlicerBase

# Image pipeline

vtkImageReader reader
  reader ReleaseDataFlagOff
  reader SetDataByteOrderToLittleEndian
  reader SetDataExtent 0 63 0 63 1 93
  reader SetFilePrefix ${VTK_DATA_ROOT}/Data/headsq/quarter
  reader SetDataMask 0x7fff

vtkImageThreshold th
th SetInput [reader GetOutput]
th SetReplaceIn 1
th SetReplaceOut 1
th ThresholdBetween -1000 1000
th SetOutValue 0
th SetInValue 2000

vtkImageOverlay over
over SetInput 0 [reader GetOutput]
over SetInput 1 [th GetOutput]
over SetOpacity 1 0.5

vtkImageViewer viewer
viewer SetInput [over GetOutput]
viewer SetZSlice 22
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source [file join [file dirname [info script]] WindowLevelInterface.tcl]

