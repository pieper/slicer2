catch {load vtktcl}
source vtkImageInclude.tcl

# Image pipeline

vtkImageReader reader
  reader ReleaseDataFlagOff
  reader SetDataByteOrderToLittleEndian
  reader SetDataExtent 0 255 0 255 22 22
  reader SetFilePrefix "../../../vtkdata/fullHead/headsq"
  reader SetDataMask 0x7fff
  set a [ reader GetDataSpacing]
  reader SetDataSpacing 1 2 0
  set b [lindex $a 0]
  set c [lindex $a 1]
  set b "$b [ expr 2 * $c ]"
  set b "$b  0"
  puts $b



vtkImageCast ImCast
  ImCast SetOutputScalarTypeToUnsignedChar
  ImCast SetInput [reader GetOutput] 

vtkImageAppendComponents AppCom
  AppCom SetInput1 [ ImCast GetOutput]
  AppCom SetInput2 [ ImCast GetOutput]

vtkImageAppendComponents AppCom2
  AppCom2 SetInput1 [ ImCast GetOutput]
  AppCom2 SetInput2 [ AppCom GetOutput]


vtkImageCrossHair2D CrossH
  CrossH SetInput [AppCom2 GetOutput]
  CrossH SetCursor 50 50  
  CrossH ShowCursorOn
  CrossH IntersectCrossOff
  CrossH SetMagnification 2
#  CrossH BullsEyeOn
  CrossH SetBullsEyeWidth 2
  CrossH SetCursorColor 0.3 1 1

vtkImageViewer viewer
  viewer SetInput [CrossH GetOutput]
  viewer SetZSlice 22
  viewer SetColorWindow 2000
  viewer SetColorLevel 1000

#make interface
source WindowLevelInterface.tcl







