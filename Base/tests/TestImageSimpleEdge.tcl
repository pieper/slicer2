catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

#reader SetFilePattern "%s.%03d"
#reader SetDataExtent 0 255 0 255 1 20
#reader SetDataScalarTypeToShort
#reader SetFilePrefix "/home/ai/odonnell/imtest/test_image"

# can do vertical or horizontal edges...
vtkImageSimpleEdge edge
edge SetKernelToHorizontal
#edge SetKernelToVertical
edge SetInput [reader GetOutput]

vtkImageViewer viewer
viewer SetInput [edge GetOutput]
viewer SetZSlice 15
viewer SetColorWindow 2000
viewer SetColorLevel 1000

# note: play with the W/L to see dark and light edges.

#make interface
source WindowLevelInterface.tcl







