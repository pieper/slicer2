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

vtkImageThreshold thfast
thfast SetInput [mag GetOutput]
thfast SetReplaceIn 1
thfast SetReplaceOut 1
thfast SetUpperThreshold 2000
thfast SetLowerThreshold 1000
thfast SetOutValue 0
thfast SetInValue 1000

vtkImageLabelOutline lout
lout SetInput [thfast GetOutput]

vtkImageViewer viewer
viewer SetInput [lout GetOutput]
viewer SetZSlice 22
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl







