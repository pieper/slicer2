catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

vtkImageThresholdBeyond thresh
thresh SetReplaceIn 1
thresh SetReplaceOut 1
# output 1's for the label we want
thresh SetInValue 1
thresh SetOutValue 0
thresh SetInput [reader GetOutput]
reader Delete
# values to grab:
thresh ThresholdBetween 750 920

vtkImageBandedDistanceMap distmap
distmap SetInput [thresh GetOutput]
thresh Delete
distmap SetBackground 0
distmap SetForeground 1



distmap SetMaximumDistanceToCompute 20

#exit

#distmap Update



vtkImageViewer viewer
viewer SetInput [distmap GetOutput]
#viewer SetInput [thresh GetOutput]
viewer SetZSlice 22
viewer SetColorWindow 50
viewer SetColorLevel 0

#make interface
source WindowLevelInterface.tcl