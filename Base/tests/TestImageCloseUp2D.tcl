catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
  reader ReleaseDataFlagOff
  reader SetDataByteOrderToLittleEndian
  reader SetDataExtent 0 255 0 255 22 22
  reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
  reader SetDataMask 0x7fff

vtkImageCloseUp2D CloseUp
  CloseUp SetInput [reader GetOutput]
  CloseUp SetMagnification 2
  CloseUp SetRadius 50
  CloseUp SetX 150
  CloseUp SetY 150

vtkImageViewer viewer
  viewer SetInput [CloseUp GetOutput]
  viewer SetZSlice 22
  viewer SetColorWindow 2000
  viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl







