catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

vtkImageThresholdBeyond th
th SetInput [reader GetOutput]
th SetReplaceIn 1
th SetReplaceOut 1 
th SetInValue 1000 
th SetOutValue 2000 
th ThresholdBetween 1000 2000

vtkImageViewer viewer
viewer SetInput [th GetOutput]
viewer SetZSlice 22
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl







