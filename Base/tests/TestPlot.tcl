catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
reader ReleaseDataFlagOff
reader SetDataByteOrderToLittleEndian
reader SetDataExtent 0 255 0 255 1 93
reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
reader SetDataMask 0x7fff

set nBins 200

vtkLookupTable lut1
  lut1 Build

vtkImageAccumulateDiscrete accum
  accum SetInput [reader GetOutput]
  accum Update
 
vtkImageBimodalAnalysis bi
  bi SetInput [accum GetOutput]
  bi Update
  set clipExt [bi GetClipExtent]
  set signalRange [bi GetSignalRange]
  set window [bi GetWindow]
  set level  [bi GetLevel]
  set min [bi GetMin]
  set max [bi GetMax]

vtkImageResize resize
  resize SetInput [accum GetOutput]
  eval resize SetInputClipExtent $clipExt
  set resizeExt "0 [expr $nBins-1] 0 0 0 0"
  eval resize SetOutputWholeExtent $resizeExt
  resize Update
 
vtkIndirectLookupTable iLut
  iLut SetLookupTable lut1
  iLut SetLevel $level
  iLut SetWindow $window
  iLut SetLowerThreshold 2000
  iLut SetUpperThreshold 1000
  iLut Build

vtkImagePlot plot
  plot SetInput [resize GetOutput]
  plot SetHeight 150
  plot SetDataRange 0 [lindex $signalRange 1]
  plot SetDataDomain $min $max
  plot SetLookupTable iLut
  plot SetColor 1 1 1
  plot Update
  set v 123
  set b [plot MapScalarToBin $v]
  puts "v=$v --> b=$b"
  set v [plot MapBinToScalar $b]
  puts "b=$b --> v=$v"


vtkImageViewer viewer
viewer SetInput [plot GetOutput]
viewer SetZSlice 0 
viewer SetColorWindow 2000
viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl







