catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 22 22
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

vtkLookupTable lut1
  lut1 Build

vtkIndirectLookupTable iLut
  iLut SetLookupTable lut1
  iLut SetLevel $level
  iLut SetWindow $window
  iLut SetLowerThreshold 1000
  iLut Build

vtkImageMapToRGBA map
  map SetInput [reader GetOutput]
  map SetLookupTable iLut

vtkImageViewer viewer
viewer SetInput [map GetOutput]
viewer SetZSlice 0 
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl







