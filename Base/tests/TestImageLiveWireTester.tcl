catch {load vtktcl}
if { [catch {set VTK_TCL $env(VTK_TCL)}] != 0} { set VTK_TCL "../../examplesTcl" }
if { [catch {set VTK_DATA $env(VTK_DATA)}] != 0} { set VTK_DATA "../../../vtkdata" }
source ../../imaging/examplesTcl/vtkImageInclude.tcl
source ../../imaging/examplesTcl/TkImageViewerInteractor.tcl

# This script tests the livewire stuff...

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

# get just one slice
vtkImageClip clip
clip SetInput [reader GetOutput]
clip SetOutputWholeExtent 0 255 0 255 0 0
clip ClipDataOn
clip ReleaseDataFlagOff

# pipeline
vtkImageLiveWireTester lwt
lwt SetInput [clip GetOutput]
vtkImageLiveWire lw
lwt SetLiveWire lw

puts "---"
lwt Update
puts "---"

# test writing settings to a file
lwt SetSettingsFileName "testfile.txt"
lwt WriteFilterSettings

# viewer
vtkImageViewer viewer
viewer SetInput [lwt GetOutput]
viewer SetZSlice 15

# Gui
toplevel .top
frame .top.f
vtkTkImageViewerWidget .top.f.v -width 256 -height 256 -iv viewer
pack .top.f.v
pack .top.f -fill both -expand t
BindTkImageViewer .top.f.v

#make interface
source WindowLevelInterface.tcl
