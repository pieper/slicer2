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

# get vertical and horizontal edges...
vtkImageSimpleEdge hEdge
hEdge SetKernelToHorizontal
hEdge SetInput [clip GetOutput]

vtkImageSimpleEdge vEdge
vEdge SetKernelToVertical
vEdge SetInput [clip GetOutput]

# Lauren uh the horiz and vert seem to be switched????...

# pipeline
puts "Images loaded and Go!"
vtkImageLiveWire lw
lw SetVerbose 1
lw SetTopEdges [hEdge GetOutput]
lw SetRightEdges [vEdge GetOutput]
# problems with 0 (inf loop?) and 256 (dump core) as coords.
puts "1"
#lw SetStartPoint 253 253
puts "2"
lw SetEndPoint 1 1
puts "3"

puts "---"
lw Update
puts "---"

############################
set points [lw GetContourPoints]
set numPoints [$points GetNumberOfPoints]
puts "numPoints: $numPoints"
puts "bounds: [$points GetBounds]"
############################

##############  junk to imitate slicer. ####################
# have to input overlay (lots of imgs) to draw roi...
#vtkLookupTable l
#vtkIndirectLookupTable lut
#lut DirectOn
#lut SetLowerThreshold 1
#lut SetLookupTable l

vtkLookupTable lut

vtkImageMapToColors mapper
mapper SetOutputFormatToRGBA
mapper SetInput [hEdge GetOutput]
mapper SetLookupTable lut

puts "hEdge extent (should become 2D): [[hEdge GetOutput] GetUpdateExtent]"

vtkImageOverlay overlay
overlay SetInput 0 [mapper GetOutput]
overlay SetInput 1 [mapper GetOutput]
#overlay SetInput 2 [mapper GetOutput]
overlay SetOpacity 1 0.5
#overlay SetOpacity 2 0.5
puts [[overlay GetOutput] GetNumberOfScalarComponents]
overlay Update
puts [[overlay GetOutput] GetNumberOfScalarComponents]
vtkImageDrawROI draw
draw SetInput [overlay GetOutput]

# output is hard to see but who cares.
########################## end imitation slicer stuff ###########

for {set i 0} {$i < $numPoints} {incr i} {
    #puts [$points GetPoint $i]
    scan [$points GetPoint $i] "%d %d %d" x y z
    draw InsertAfterSelectedPoint $x $y
}

# viewer
vtkImageViewer viewer
viewer SetInput [draw GetOutput]
viewer SetZSlice 15
# note: play with the W/L to see dark and light edges.
# make this visible at all!
viewer SetColorWindow 2485
viewer SetColorLevel 91


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







