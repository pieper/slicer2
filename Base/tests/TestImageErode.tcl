catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

vtkImageThresholdFast thfast
thfast SetInput [reader GetOutput]
thfast SetReplaceIn 1
thfast SetReplaceOut 1
thfast SetUpperThreshold 2000
thfast SetLowerThreshold 1000
thfast SetOutValue 0
thfast SetInValue 1000

vtkImageErode erode
erode SetInput [thfast GetOutput]
erode SetBackground 1000
erode SetForeground 0

vtkImageViewer viewer
viewer SetInput [erode GetOutput]
viewer SetZSlice 22
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl







