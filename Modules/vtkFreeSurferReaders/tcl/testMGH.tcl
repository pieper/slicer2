# test script for mgh reader
# oct 7/04

package require vtkFreeSurferReaders
package require vtk
package require vtkinteraction
vtkRenderer ren1
vtkRenderWindow renWin
vtkRenderWindowInteractor iren
iren SetRenderWindow renWin
vtkMGHReader reader
reader SetFileName "/projects/birn/nicole/slicer2/flash20.mgh"

    # Create transfer functions for opacity and color
    vtkPiecewiseFunction VolRend(opacityTransferFunction)
    vtkColorTransferFunction VolRend(colorTransferFunction)
    vtkPiecewiseFunction VolRend(GradientOpacityTRansferFunction)

    # Create properties, mappers, volume actors, and ray cast function
    vtkVolumeProperty VolRend(volumeProperty)
    VolRend(volumeProperty) SetColor VolRend(colorTransferFunction)
    VolRend(volumeProperty) SetScalarOpacity VolRend(opacityTransferFunction)
    VolRend(volumeProperty) SetGradientOpacity VolRend(GradientOpacityTRansferFunction)

    vtkVolumeRayCastCompositeFunction  VolRend(compositeFunction)
    vtkVolumeRayCastMIPFunction VolRend(mipFunction)
    vtkVolumeRayCastIsosurfaceFunction VolRend(isosurfaceFunction)

    vtkVolumeRayCastMapper VolRend(raycastvolumeMapper)
    vtkVolumeTextureMapper2D VolRend(texturevolumeMapper)

    # output of the mgh vol
#    VolRend(volumeMapper) SetInput [reader GetOutput]

    VolRend(raycastvolumeMapper) SetVolumeRayCastFunction VolRend(compositeFunction)

    vtkVolume VolRend(volume)
#    VolRend(volume) SetMapper VolRend(volumeMapper)
    VolRend(volume) SetProperty VolRend(volumeProperty)

    vtkImageCast VolRend(imageCast)


ren1 AddVolume VolRend(volume)
ren1 SetBackground 1 1 1
renWin SetSize 600 600
renWin Render
    
proc TkCheckAbort {} {
  set foo [renWin GetEventPending]
  if {$foo != 0} {renWin SetAbortRender 1}
}
renWin AddObserver AbortCheckEvent {TkCheckAbort}

iren AddObserver UserEvent {wm deiconify .vtkInteract}
iren Initialize

wm withdraw .

