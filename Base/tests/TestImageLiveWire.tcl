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

# get vertical and horizontal edges...
vtkImageSimpleEdge hEdge
hEdge SetKernelToHorizontal
hEdge SetInput [reader GetOutput]

vtkImageSimpleEdge vEdge
vEdge SetKernelToVertical
vEdge SetInput [reader GetOutput]

# Lauren uh the horiz and vert seem to be switched????...

# pipeline
vtkImageLiveWire lw
lw SetTopEdges [hEdge GetOutput]
lw SetRightEdges [vEdge GetOutput]
lw SetStartPoint 100 100
lw SetEndPoint 101 110

# viewer
vtkImageViewer viewer
viewer SetInput [lw GetOutput]
viewer SetZSlice 15
# note: play with the W/L to see dark and light edges.
viewer SetColorWindow 2000
viewer SetColorLevel 1000

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
    global FirstPoint
    
    puts "first: $FirstPoint     point:  $x   $y"

    if {$FirstPoint == 1} {
	lw SetStartPoint $x $y
	set FirstPoint 0
    } else {
	lw SetEndPoint $x $y
	set FirstPoint 1
	# now find the shortest path...
    }

}







