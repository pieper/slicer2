catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
#reader SetDataExtent 0 255 0 255 1 60
#reader SetFilePrefix "/home/medvis2/data/leventon/Prostate/masa/110998/002/Inew"
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