# Test script for vtkImageFrameSource.

# load vtk on PCs
catch {load vtktcl}

# user interface command widget (VTK Interactor)
if { [catch {set VTK_TCL $env(VTK_TCL)}] != 0} { set VTK_TCL "../../examplesTcl" }
if { [catch {set VTK_DATA $env(VTK_DATA)}] != 0} { set VTK_DATA "../../../vtkdata" }
source $VTK_TCL/vtkInt.tcl

# make the original window
vtkRenderer ren
vtkRenderWindow renWin
renWin AddRenderer ren
vtkRenderWindowInteractor iren
iren SetRenderWindow renWin

# display a cone for testing
vtkConeSource cone
cone SetResolution 8
vtkPolyDataMapper coneMapper
coneMapper SetInput [cone GetOutput]
vtkActor coneActor
coneActor SetMapper coneMapper
ren AddActor coneActor

# enable user interface interactor
iren SetUserMethod {wm deiconify .vtkInteract}
iren Initialize

# prevent the tk window from showing up then start the event loop
wm withdraw .

# draw the original window
#renWin Render

# here's what we're testing:
vtkImageFrameSource frameSource
# copy the stuff from renWin
frameSource SetRenderWindow renWin

# make the "copy" window
vtkXDisplayWindow copyDisplayWindow
vtkImager copyRen

vtkImageMapper copyMapper
copyMapper SetColorWindow 255
copyMapper SetColorLevel 127.5
copyMapper SetInput [frameSource GetOutput]

vtkActor2D copyActor
copyActor SetMapper copyMapper

copyRen AddActor2D copyActor

# render into a 2D image window
set copyWin [copyDisplayWindow GetImageWindow 0]
$copyWin AddImager copyRen

# move the second window over a little
$copyWin SetPosition 300 300

# Render the original window so that the second one
# will not come up black the first time.
renWin Render

# Render the copy.  Bring up the interactor and paste
# the following in to render many times.
frameSource Modified
#copyMapper Modified
$copyWin Render



