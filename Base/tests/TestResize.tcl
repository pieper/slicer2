catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 22 22
reader SetFilePrefix "../../../vtk/vtk3.0/vtkdata/fullHead/"
#"../../../vtkdata/fullHead/headsq"

reader SetDataMask 0x7fff

set resizeExt "0 149 0 149 0 0"
set clipExt "100 109 100 109 0 0"
vtkImageResize resize
  resize SetInput [reader GetOutput]
#  eval resize SetInputClipExtent $clipExt
  eval resize SetOutputWholeExtent $resizeExt
  resize Update
 
vtkImageViewer viewer
viewer SetInput [resize GetOutput]
viewer SetZSlice 0 
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl







