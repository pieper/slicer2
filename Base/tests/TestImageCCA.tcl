package require vtk
package require vtkSlicerBase

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

vtkImageThreshold th
th SetInput [reader GetOutput]
th SetReplaceIn 1
th SetReplaceOut 1 
th SetInValue 1000 
th SetOutValue 2000 
th ThresholdBetween 1000 2000

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







