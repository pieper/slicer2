proc DTMRICalculateScalarsInit {} {

    global DTMRI

    #------------------------------------
    # Variables for producing scalar volumes
    #------------------------------------

    # math op to produce scalars from DTMRIs
    set DTMRI(scalars,operation) Trace
    set DTMRI(scalars,operationList) [list Trace Determinant \
                      RelativeAnisotropy FractionalAnisotropy Mode LinearMeasure \
                      PlanarMeasure SphericalMeasure MaxEigenvalue \
                      MiddleEigenvalue MinEigenvalue ColorByOrientation \
                                     ColorByMode  D11 D22 D33]

    set DTMRI(scalars,operationList,tooltip) "Produce a scalar volume from DTMRI data.\nTrace, Determinant, Anisotropy, and Eigenvalues produce grayscale volumes,\nwhile Orientation produces a 3-component (Color) volume that is best viewed in the 3D window."

    # how much to scale the output floats by
    set DTMRI(scalars,scaleFactor) 10000
    set DTMRI(scalars,scaleFactor,tooltip) \
    "Multiplicative factor applied to output images for better viewing."
    
    # whether to compute vol from ROI or whole DTMRI volume
    set DTMRI(scalars,ROI) None
    set DTMRI(scalars,ROIList) {None Threshold Mask}
    set DTMRI(scalars,ROIList,tooltips) {"No ROI: derive the scalar volume from the entire DTMRI volume." "Use the thresholded area defined in the ROI tab to mask the DTMRI volume before scalar volume creation." "Use the mask labelmap volume defined in the ROI tab to mask the DTMRI volume before scalar volume creation."}




}



################################################################
#  Procedures used to derive scalar volumes from DTMRI data
################################################################

#-------------------------------------------------------------------------------
# .PROC DTMRISetOperation
# Set the mathematical operation we should do to produce
# a scalar volume from the DTMRIs
# .ARGS
# str math the name of the operation from list $DTMRI(scalars,operationList)
# .END
#-------------------------------------------------------------------------------
proc DTMRISetOperation {math} {
    global DTMRI

    set DTMRI(scalars,operation) $math
    
    # config menubutton
    $DTMRI(gui,mbMath) config -text $math
}


#-------------------------------------------------------------------------------
# .PROC DTMRIUpdateMathParams
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc DTMRIUpdateMathParams {} {
    global DTMRI


    # Just check that if they requested a 
    # preprocessing step, that we are already
    # doing that step
    
    set mode $DTMRI(scalars,ROI)

    set err "The $mode ROI is not currently being computed.  Please turn this feature on in the ROI tab before creating the volume."

    switch $mode {
        "None" {
        }
        "Threshold" {
            if {$DTMRI(mode,threshold)    == "None"} {
                set DTMRI(scalars,ROI) None
                tk_messageBox -message $err
            }
        }
        "Mask" {
            if {$DTMRI(mode,mask)    == "None"} {
                set DTMRI(scalars,ROI) None
                tk_messageBox -message $err
            }
        }
    }    

}

#-------------------------------------------------------------------------------
# .PROC DTMRICreateEmptyVolume
# Just like DevCreateNewCopiedVolume, but uses a Tensor node
# to copy parameters from instead of a volume node
# Used for scalar output from DTMRI math calculations.
# .ARGS
# int OrigId
# string Description Defaults to empty string
# string VolName Defaults to empty string
# .END
#-------------------------------------------------------------------------------
proc DTMRICreateEmptyVolume {OrigId {Description ""} { VolName ""}} {
    global Volume Tensor

    # Create the node (vtkMrmlVolumeNode class)
    set newvol [MainMrmlAddNode Volume]
    $newvol Copy Tensor($OrigId,node)
    
    # Set the Description and Name
    if {$Description != ""} {
        $newvol SetDescription $Description 
    }
    if {$VolName != ""} {
        $newvol SetName $VolName
    }

    # Create the volume (vtkMrmlDataVolume class)
    set n [$newvol GetID]
    MainVolumesCreate $n

    # This updates all the buttons to say that the
    # Volume List has changed.
    MainUpdateMRML

    return $n
}

#-------------------------------------------------------------------------------
# .PROC DTMRIDoMath
# Called to compute a scalar vol from DTMRIs
# .ARGS
# string operation Defaults to empty string
# .END
#-------------------------------------------------------------------------------
proc DTMRIDoMath {{operation ""}} {
    global DTMRI Gui Tensor


    # if this was called from user input GUI menu
    if {$operation == ""} {
        set operation $DTMRI(scalars,operation) 
    }

    # validate user input
    if {[ValidateFloat $DTMRI(scalars,scaleFactor)] != "1"} {
        tk_messageBox -message \
            "Please enter a number for the scale factor."
        # reset default
        set DTMRI(scalars,scaleFactor) 1000
        return
    }

    # should use DevCreateNewCopiedVolume if have a vol node
    # to copy...
    set t $Tensor(activeID) 
    if {$t == "" || $t == $Tensor(idNone)} {
    tk_messageBox -message \
        "Please select an input DTMRI volume (Active DTMRI)"
        return
    }
    set name [Tensor($t,node) GetName]
    set name ${operation}_$name
    set description "$operation volume derived from DTMRI volume $name"
    set v [DTMRICreateEmptyVolume $t $description $name]

    # find input
    set mode $DTMRI(scalars,ROI)
    
    switch $mode {
        "None" {
            set input [Tensor($t,data) GetOutput]
        }
        "Threshold" {
            set input [DTMRI(vtk,thresh,mask) GetOutput]
        }
        "Mask" {
            set input [DTMRI(vtk,mask,mask) GetOutput]
        }
    }

    #Set up proper scale factor
    #Map result between 1 - 1000
    set rangexx [[[$input GetPointData] GetTensors] GetRange 0]
    set rangeyy [[[$input GetPointData] GetTensors] GetRange 4]
    set rangezz [[[$input GetPointData] GetTensors] GetRange 8]
    
    set maxTrace [expr [lindex $rangexx 1] + [lindex $rangeyy 1] + [lindex $rangezz 1]]
    
    puts "Running oper: $operation"
    puts "Max Trace: $maxTrace"
    
    switch -regexp -- $operation {
        {^(Trace|Determinant|D11|D22|D33|MaxEigenvalue|MiddleEigenvalue|MinEigenvalue)$} {
            set DTMRI(scalars,scaleFactor) [expr 1000.0 / $maxTrace]
        }
        {^(RelativeAnisotropy|FractionalAnisotropy|Mode|LinearMeasure|PlanarMeasure|SphericalMeasure|ColorByOrientation|ColorByMode)$} {
            set DTMRI(scalars,scaleFactor) 1000
        }
    }
    
    puts "DTMR: scale factor $DTMRI(scalars,scaleFactor)"

    # create vtk object to do the operation
    vtkTensorMathematics math
    math SetScaleFactor $DTMRI(scalars,scaleFactor)
    math SetInput 0 $input
    math SetInput 1 $input
    math SetOperationTo$operation
    # color by RAS orientation, not IJK
    if {$operation == "ColorByOrientation"} {
        vtkTransform transform
        DTMRICalculateIJKtoRASRotationMatrix transform $Tensor(activeID)
        math SetTensorRotationMatrix [transform GetMatrix]
        transform Delete
    }
    math AddObserver StartEvent MainStartProgress
    math AddObserver ProgressEvent "MainShowProgress math"
    math AddObserver EndEvent MainEndProgress
    set Gui(progressText) "Creating Volume $operation"

    # put the filter output into a slicer volume
    math Update
    #puts [[math GetOutput] Print]
    Volume($v,vol) SetImageData [math GetOutput]
    MainVolumesUpdate $v
    # tell the node what type of data so MRML file will be okay
    Volume($v,node) SetScalarType [[math GetOutput] GetScalarType]


    math SetInput 0 ""    
    math SetInput 1 ""
    # this is to disconnect the pipeline
    # this object hangs around, so try this trick from Editor.tcl:
    math SetOutput ""
    math Delete
    
    # reset blue bar text
    set Gui(progressText) ""
  
    # Registration
    # put the new volume inside the same transform as the original tensor
    # by inserting it right after that volume in the mrml file
    set nitems [Mrml(dataTree) GetNumberOfItems]
    for {set widx 0} {$widx < $nitems} {incr widx} {
        if { [Mrml(dataTree) GetNthItem $widx] == "Volume($v,node)" } {
            break
        }
    }
    if { $widx < $nitems } {
        Mrml(dataTree) RemoveItem $widx
        Mrml(dataTree) InsertAfterItem Tensor($t,node) Volume($v,node)
        MainUpdateMRML
    }
    
    # display this volume so the user knows something happened
    MainSlicesSetVolumeAll Back $v
    RenderAll
}


