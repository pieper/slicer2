package require vtk
package require vtkSlicerBase

# Image pipeline

vtkImageReader reader
  reader ReleaseDataFlagOff
  reader SetDataByteOrderToLittleEndian
  reader SetDataExtent 0 63 0 63 1 93
  reader SetFilePrefix ${VTK_DATA_ROOT}/Data/headsq/quarter
  reader SetDataMask 0x7fff

vtkImageMagnify mag
  mag SetInput [reader GetOutput]
  mag SetMagnificationFactors 4 4 1

vtkLookupTable lut1
  lut1 Build

vtkIndirectLookupTable iLut
  iLut SetLookupTable lut1
  iLut SetLevel $level
  iLut SetWindow $window
  iLut SetLowerThreshold 1000
  iLut Build

vtkImageMapToRGBA map
  map SetInput [mag GetOutput]
  map SetLookupTable iLut

vtkImageViewer viewer
viewer SetInput [map GetOutput]
viewer SetZSlice 0 
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source [file join [file dirname [info script]] WindowLevelInterface.tcl]







