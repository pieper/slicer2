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
clip SetOutputWholeExtent 0 255 0 255 10 10
clip ClipDataOn
clip ReleaseDataFlagOff

# pipeline
puts "Images loaded and Go!"
vtkImageLiveWire lw
lw SetVerbose 1

foreach dir {0 1 2 3} name {Up Down Left Right} {
    vtkImageLiveWireEdgeWeights lwedge$dir
    lwedge$dir SetInput [clip GetOutput]
    lwedge$dir SetEdgeDirection $dir
    lwedge$dir Update

    # set livewire's 4 edge inputs
    lw Set${name}Edges [lwedge$dir GetOutput]

    puts "update ext [[lwedge$dir GetOutput] GetUpdateExtent]"
    puts "whole ext [[lwedge$dir GetOutput] GetWholeExtent]"
}

# also set livewire's original image input.
lw SetOriginalImage [clip GetOutput]

# problems with 0 (inf loop?) and 256 (dump core) as coords.
puts "1"
lw SetStartPoint 253 253
puts "2"
lw SetEndPoint 1 1
puts "3"

puts "out update ext [[lw GetOutput] GetUpdateExtent]"
puts "out whole ext [[lw GetOutput] GetWholeExtent]"
puts "in update ext [[lw GetInput] GetUpdateExtent]"
puts "in whole ext [[lw GetInput] GetWholeExtent]"

# Update lw so we can grab the points info
puts "---"
lw Update
puts "---"

############################
set points [lw GetContourPoints]
set numPoints [$points GetNumberOfPoints]
puts "numPoints: $numPoints"
puts "bounds: [$points GetBounds]"
############################

# viewer
vtkImageViewer viewer
viewer SetInput [lw GetOutput]
viewer SetZSlice 15
viewer SetColorWindow 10
viewer SetColorLevel 0

# Gui
toplevel .top
frame .top.f
vtkTkImageViewerWidget .top.f.v -width 256 -height 256 -iv viewer
pack .top.f.v
pack .top.f -fill both -expand t
BindTkImageViewer .top.f.v

#make interface
source WindowLevelInterface.tcl

# bindings
bind .top.f.v <Button-1> {addPoint %x %y}

# stupid way
set FirstPoint 1

# find shortest path.....
proc addPoint { x y } {

    # now find the shortest path...
    puts "-----------------------------------------------"
    puts "going, $x $y"
    #lw SetEndPoint $x $y
    puts "-----------------------------------------------"

}








