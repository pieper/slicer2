#=auto=========================================================================
# (c) Copyright 2001 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
# FILE:        EdFastMarching.tcl
# PROCEDURES:  
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdFastMarchingInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdFastMarchingInit {} {
    global Ed Gui EdFastMarching Volume Slice Fiducials

    #puts "EdFastMarchingInit"

    set e EdFastMarching
    set Ed($e,name)      "Fast Marching"
    set Ed($e,initials)  "Fm"
    set Ed($e,desc)      "Fast Marching: 3D segmentation"
    set Ed($e,rank)      14;
    set Ed($e,procGUI)   EdFastMarchingBuildGUI
    set Ed($e,procEnter) EdFastMarchingEnter
    set Ed($e,procExit)  EdFastMarchingExit

    # Define Dependencies
    set Ed($e,depend) Fiducials 
    set EdFastMarching(fastMarchingInitialized) 0

    # Required
    set Ed($e,scope)  3D 
    set Ed($e,input)  Original
    set Ed($e,interact) Active

    set EdFastMarching(nExpand) 10000
    set EdFastMarching(userExpand) 0

}

#-------------------------------------------------------------------------------
# .PROC EdFastMarchingBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdFastMarchingBuildGUI {} {
    global Ed Gui Label Volume EdFastMarching Fiducials Help

    set e EdFastMarching
    #-------------------------------------------
    # FastMarching frame
    #-------------------------------------------

    set f $Ed(EdFastMarching,frame)

    #copied from EdPhaseWireBuildGUI

    set label ""
    set subframes {Help Basic Advanced}
    set buttonText {"Help" "Basic" "Advanced"}
    set tooltips { "Help: We all need it sometimes." \
        "Basic: For Users" \
        "Advanced: For Developers"}
    set extraFrame 0
    set firstTab Basic

    TabbedFrame EdFastMarching $f $label $subframes $buttonText \
        $tooltips $extraFrame $firstTab

    #-------------------------------------------
    # TabbedFrame->Help frame
    #-------------------------------------------
    set f $Ed(EdFastMarching,frame).fTabbedFrame.fHelp

    frame $f.fWidget -bg $Gui(activeWorkspace)
    pack $f.fWidget -side top -padx 2 -fill both -expand true

    set Ed(EdFastMarching,helpWidget) [HelpWidget $f.fWidget]

    set help "DISCLAIMER: this module is for development only!
Eric Pichon <eric@ece.gatech.edu>

Online tutorial available at:
http://users.ece.gatech.edu/~eric/research/slicer

3D segmentation using Partial Differential Equations.

To segment a volume :

- Define some seed points : by creating some fiducials inside (not on the border of) the region of interest. Fiducials can be created by moving the pointer to the desired region and pressing the 'p' key.  See the Fiducial module documentation for more on using fiducials.

- Define a label for the segmented data : by clicking on the 'Label' button. 

- Start expansion of the surface : by clicking on the 'Expand' button.  The volume of the surface will be expanded by the value right of the expand button.  Increase this value to segment a bigger object.


If the expansion did not go far enough. add new seeds in the regions that were not reached and/or press 'Expand' again.

If the expansion went too far ('leaked' out of the region of interest), use the slider to reverse the last expansion to the point before leaking occurred.

When satisfied with the segmentation use other editing modules on the labelmap (morphological operations...) and/or create a model.
"
    eval $Ed(EdFastMarching,helpWidget) tag configure normal   $Help(tagNormal)

    $Ed(EdFastMarching,helpWidget) insert insert "$help" normal

    #-------------------------------------------
    # TabbedFrame->Basic frame
    #-------------------------------------------
    set f $Ed(EdFastMarching,frame).fTabbedFrame.fBasic
 
    frame $f.fGrid     -bg $Gui(activeWorkspace)
    frame $f.fExpand   -bg $Gui(activeWorkspace)
    frame $f.fUserExpand   -bg $Gui(activeWorkspace)

    pack $f.fGrid $f.fExpand $f.fUserExpand \
    -side top -fill x 

    #-------------------------------------------
    # FastMarching->Grid frame
    #-------------------------------------------
    set f $Ed(EdFastMarching,frame).fTabbedFrame.fBasic.fGrid

    # Output label
    eval {button $f.bOutput -text "Label:" \
          -command "ShowLabels EdFastMarchingLabel"} $Gui(WBA)
    eval {entry $f.eOutput -width 6 -textvariable Label(label)} $Gui(WEA)
    bind $f.eOutput <Return>   "EdFastMarchingLabel"
    bind $f.eOutput <FocusOut> "EdFastMarchingLabel"
    eval {entry $f.eName -width 14 -textvariable Label(name)} $Gui(WEA) \
    {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w

    lappend Label(colorWidgetList) $f.eName

    #-------------------------------------------
    # FastMarching->Expand frame
    #-------------------------------------------
    set f $Ed(EdFastMarching,frame).fTabbedFrame.fBasic.fExpand

    # Output label
    eval {button $f.bExpand -text "EXPAND" \
          -command "EdFastMarchingExpand"} $Gui(WBA)
    eval {entry $f.eExpand -width 12 -textvariable EdFastMarching(nExpand)} $Gui(WEA)

    grid $f.bExpand $f.eExpand -padx 2 -pady $Gui(pad)

    set f $Ed(EdFastMarching,frame).fTabbedFrame.fBasic.fUserExpand

    eval {scale $f.sExpand -from 0 -to 100 \
            -length 220 -variable EdFastMarching(userExpand) -resolution 1 \
            -command "EdFastMarchingUserExpand $EdFastMarching(userExpand)"} \
            $Gui(WSA) {-sliderlength 22}

    grid $f.sExpand -sticky w

    #-------------------------------------------
    # TabbedFrame->Advanced frame
    #-------------------------------------------
    set f $Ed(EdFastMarching,frame).fTabbedFrame.fAdvanced

    eval {label $f.lWarning1 -text "Nothing here yet..." } \
    $Gui(WLA)

    pack $f.lWarning1
}

proc EdFastMarchingUserExpand {zero userExpand} {
    global EdFastMarching

    if {$EdFastMarching(fastMarchingInitialized) != 0} {
        set e EdFastMarching

        set Ed($e,scope)  3D 
        set Ed($e,input)  Original
        set Ed($e,interact) Active   

        set v [EditorGetInputID $Ed($e,input)]

        EdSetupBeforeApplyEffect $v $Ed($e,scope) Native
        Ed(editor)  UseInputOn
          
        EdFastMarching(FastMarching) show [expr $userExpand/100.0]

        Ed(editor)  SetInput ""
        Ed(editor)  UseInputOff

        EdUpdateAfterApplyEffect $v
    }
}

proc EdFastMarchingExpand {} {
    global EdFastMarching

    if {[ValidateInt $EdFastMarching(nExpand)] == 0} {
        tk_messageBox -message "Expansion is not an integer !"
        return
    } 
  
    if { $EdFastMarching(nExpand) <= 0 } {
        tk_messageBox -message "Expansion is not positive !"
        return
    }   

    EdFastMarching(FastMarching) setNPointsEvolution $EdFastMarching(nExpand)
    EdFastMarchingSegment

    set EdFastMarching(userExpand) 100
    EdFastMarchingUserExpand 0 $EdFastMarching(userExpand)
}




#called whenever we enter the FastMarching tab
proc EdFastMarchingEnter {} {
    global Ed Label Slice EdFastMarching Fiducials Gui Volumes

    if {$EdFastMarching(fastMarchingInitialized) == 0} {

    set e EdFastMarching

    EdFastMarchingLabel         

    # Make sure we're colored
    LabelsColorWidgets

    set v [EditorGetInputID $Ed($e,input)]
    set depth [Volume($v,vol) GetRangeHigh]

    set dim [[Volume($v,vol) GetOutput] GetWholeExtent]

    set EdFastMarching(fastMarchingInitialized) 1

    set EdFastMarching(fidFiducialList) \
        [ FiducialsCreateFiducialsList "default" "FastMarching-seeds" 0 3 ]

    # create the vtk object 
    vtkFastMarching EdFastMarching(FastMarching) 

    # initialize the object
    EdFastMarching(FastMarching) init \
        [expr [lindex $dim 1] + 1] [expr [lindex $dim 3] + 1] [expr [lindex $dim 5] + 1] $depth

    # Required
    set Ed($e,scope)  3D 
    set Ed($e,input)  Original
    set Ed($e,interact) Active

    EditorActivateUndo 0
    
    EditorClear Working
    
    # need one pass through the execute function
    # to be properly initialized
    set e EdFastMarching

    set Ed($e,scope)  3D 
    set Ed($e,input)  Original
    set Ed($e,interact) Active   

    set v [EditorGetInputID $Ed($e,input)]

    EdSetupBeforeApplyEffect $v $Ed($e,scope) Native
    Ed(editor)  UseInputOn

    set Gui(progressText) "FastMarching: initializing"

    MainStartProgress

    EdFastMarching(FastMarching) Modified
    Ed(editor) Apply EdFastMarching(FastMarching) EdFastMarching(FastMarching)

    MainEndProgress

    Ed(editor)  SetInput ""
    Ed(editor)  UseInputOff

    EdUpdateAfterApplyEffect $v
    }    
    FiducialsSetActiveList "FastMarching-seeds"
}


#called whenever we exit the FastMarching tab
proc EdFastMarchingExit {} {
    global Ed EdFastMarching

    #puts "EdFastMarchingExit"

    EdFastMarching(FastMarching) unInit

    #delete the object
    EdFastMarching(FastMarching) Delete

    FiducialsDeleteList "FastMarching-seeds"

    set EdFastMarching(fastMarchingInitialized) 0

    Slicer BackFilterOff
    Slicer ForeFilterOff
    Slicer ReformatModified
    Slicer Update
}

proc EdFastMarchingLabel {} {
    global Ed

    LabelsFindLabel
}

#
# Where the job gets done
#
proc EdFastMarchingSegment {} {
    global Label Fiducials EdFastMarching Volume Ed Gui

    if {[ValidateInt $Label(label)] == 0} {
    tk_messageBox -message "Output label is not an integer !"
    return
    }

    if {[ValidateInt $Label(label)] <= 0} {
    tk_messageBox -message "Output label is not positive !"
    return
    }
   
    if {[ValidateInt $EdFastMarching(nExpand)] == 0} {
    tk_messageBox -message "Expansion is not an integer !"
    return
    } 
  
    if { $EdFastMarching(nExpand) <= 0 } {
    tk_messageBox -message "Expansion is not positive !"
    return
    }   

    set label   $Label(label)
    EdFastMarching(FastMarching) setActiveLabel $label
 
    set e EdFastMarching

    set Ed($e,scope)  3D 
    set Ed($e,input)  Original
    set Ed($e,interact) Active   

    set v [EditorGetInputID $Ed($e,input)]

    # note: we should probably use GetRasToIjkMatrix here but it does not
    # seem to work (?)

    scan [Volume($v,node) GetRasToVtkMatrix] \
    "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f" \
    m11 m12 m13 m14 \
    m21 m22 m23 m24 \
    m31 m32 m33 m34 \
    m41 m42 m43 m44;

    EdFastMarching(FastMarching) setRAStoIJKmatrix $m11 $m12 $m13 $m14 $m21 $m22 $m23 $m24 $m31 $m32 $m33 $m34 $m41 $m42 $m43 $m44

    set l [FiducialsGetPointIdListFromName "FastMarching-seeds"]

    foreach s $l {

    set coord [FiducialsGetPointCoordinates $s]

    set cr [lindex $coord 0]
    set ca [lindex $coord 1]
    set cs [lindex $coord 2]

    if { [EdFastMarching(FastMarching) addSeed $cr $ca $cs]==0 } {
        FiducialsDeletePoint $EdFastMarching(fidFiducialList) $s
        tk_messageBox -message "Seed $s is outside of the volume.\nIt has therefore been removed."
    }

    }

    set l [FiducialsGetPointIdListFromName "FastMarching-seeds"]

    if { [llength $l]<=0 } {
    tk_messageBox -message "No seeds defined !\n(see help section)"
    return
    }

    EdSetupBeforeApplyEffect $v $Ed($e,scope) Native
    Ed(editor)  UseInputOn

    set Gui(progressText) "FastMarching"

    MainStartProgress

    EdFastMarching(FastMarching) Modified
    Ed(editor) Apply EdFastMarching(FastMarching) EdFastMarching(FastMarching)

    MainEndProgress

    Ed(editor)  SetInput ""
    Ed(editor)  UseInputOff

    EdUpdateAfterApplyEffect $v
}

# this is called when the user clicks on the active slice
# we don't want to do anything special in this case
proc EdFastMarchingApply {} {}
