catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

vtkImageThresholdBeyond thbeyond
thbeyond SetInput [reader GetOutput]
thbeyond SetReplaceIn 1
thbeyond SetReplaceOut 1 
thbeyond SetInValue 1000 
thbeyond SetOutValue 2000 
thbeyond ThresholdBetween 1000 2000

vtkImageCCA cca
cca SetInput [thbeyond GetOutput]
cca SetSeed 100 100 100
cca SetOutputLabel 100
cca SetBackground 155
#cca SetMinSize
#cca SetMinForeground
#cca SetMaxForeground 
cca SetLargestIslandSize 15000


vtkImageViewer viewer
viewer SetInput [cca GetOutput]
viewer SetZSlice 22
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl







