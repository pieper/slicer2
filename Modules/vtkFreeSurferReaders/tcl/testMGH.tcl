#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for 
# research purposes only, provided that (1) the above copyright notice and 
# the following four paragraphs appear on all copies of this software, and 
# (2) that source code to any modifications to this software be made 
# publicly available under terms no more restrictive than those in this 
# License Agreement. Use of this software constitutes acceptance of these 
# terms and conditions.
# 
# 3D Slicer Software has not been reviewed or approved by the Food and 
# Drug Administration, and is for non-clinical, IRB-approved Research Use 
# Only.  In no event shall data or images generated through the use of 3D 
# Slicer Software be used in the provision of patient care.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
# OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
# NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
#
#===============================================================================
# FILE:        testMGH.tcl
# PROCEDURES:  
#==========================================================================auto=
# test script for mgh reader
#
 
package require vtkFreeSurferReaders
package require vtk
package require vtkinteraction
vtkRenderer ren1
vtkRenderWindow renWin
vtkRenderWindowInteractor iren
iren SetRenderWindow renWin
vtkMGHReader reader
reader SetFileName "/projects/birn/freesurfer/recon/MGH-GE15-JJ/mri/flash/flash20.mgh"

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

