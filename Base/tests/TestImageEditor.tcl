catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

vtkImageThreshold th
  th SetReplaceIn 1
  th SetReplaceOut 0
  th SetInValue 4000
  th ThresholdBetween 1000 2000

vtkImageEditor edit 
  edit SetInput [reader GetOutput]
  edit UseInputOn
  edit SetDimensionTo3D
#  edit SetDimensionToSingle
  edit SetSlice 22 
  edit SetOrderString SI
  edit SetFirstFilter th
  edit SetLastFilter th
  edit Apply
  edit Undo
  edit Apply
 
vtkImageViewer viewer
viewer SetInput [edit GetOutput]
viewer SetZSlice 22
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl







