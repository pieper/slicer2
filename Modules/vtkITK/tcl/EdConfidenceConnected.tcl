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
# FILE:        EdConfidenceConnected.tcl
# PROCEDURES:  
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdConfidenceConnectedInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdConfidenceConnectedInit {} {
    global Ed Gui EdConfidenceConnected Volume Slice Fiducials

    set e EdConfidenceConnected
    set Ed($e,name)      "ConfidenceConnected"
    set Ed($e,initials)  "Cc"
    set Ed($e,desc)      "ITK-Based ConfidenceConnected: 3D segmentation"
    set Ed($e,rank)      14;
    set Ed($e,procGUI)   EdConfidenceConnectedBuildGUI
    set Ed($e,procEnter) EdConfidenceConnectedEnter
    set Ed($e,procExit)  EdConfidenceConnectedExit

    # Define Dependencies
    set Ed($e,depend) Fiducials 

    # Required
    set Ed($e,scope)  3D 
    set Ed($e,input)  Original
    set Ed($e,interact) Active

    set EdConfidenceConnected(iterations) 1
    set EdConfidenceConnected(multiplier) 1

    set EdConfidenceConnected(majorVersionTCL) 1
    set EdConfidenceConnected(minorVersionTCL) 0
    set EdConfidenceConnected(dateVersionTCL) "2003-02-23/20:00EST"

    set EdConfidenceConnected(versionTCL) "$EdConfidenceConnected(majorVersionTCL).$EdConfidenceConnected(minorVersionTCL) \t($EdConfidenceConnected(dateVersionTCL))"

    set EdConfidenceConnected(shouldDisplayWarningVersion) 1

}

#-------------------------------------------------------------------------------
# .PROC EdConfidenceConnectedBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdConfidenceConnectedBuildGUI {} {
    global Ed Gui Label Volume EdConfidenceConnected Fiducials Help

    set e EdConfidenceConnected
    #-------------------------------------------
    # ConfidenceConnected frame
    #-------------------------------------------

    set f $Ed(EdConfidenceConnected,frame)

    #copied from EdPhaseWireBuildGUI

    set label ""
    set subframes {Help Basic }
    set buttonText {"Help" "Basic"}
    set tooltips { "Help: We all need it sometimes." \
        "Basic: For Users" }
    set extraFrame 0
    set firstTab Basic

    TabbedFrame EdConfidenceConnected $f $label $subframes $buttonText \
        $tooltips $extraFrame $firstTab

    #-------------------------------------------
    # TabbedFrame->Help frame
    #-------------------------------------------
    set f $Ed(EdConfidenceConnected,frame).fTabbedFrame.fHelp

    frame $f.fWidget -bg $Gui(activeWorkspace)
    pack $f.fWidget -side top -padx 2 -fill both -expand true

    set Ed(EdConfidenceConnected,helpWidget) [HelpWidget $f.fWidget]

    set help "DISCLAIMER: this module is for demonstration only.
Implementation of ITK ConfidenceConnected calculation as slicer editor effect.
See www.itk.org for description of algorithm.
"
    eval $Ed(EdConfidenceConnected,helpWidget) tag configure normal   $Help(tagNormal)

    $Ed(EdConfidenceConnected,helpWidget) insert insert "$help" normal

    #-------------------------------------------
    # TabbedFrame->Basic frame
    #-------------------------------------------
    set f $Ed(EdConfidenceConnected,frame).fTabbedFrame.fBasic
 
    eval {button $f.bSegment -text "Segment" \
          -command "EdConfidenceConnectedSegment"} $Gui(WBA)

    grid $f.bSegment -padx 2 -pady $Gui(pad)

    eval {scale $f.sIterations -from 1 -to 10 \
            -length 220 -variable EdConfidenceConnected(iterations) -resolution 1 } \
            $Gui(WSA) {-sliderlength 22}

    $f.sIterations configure -showvalue 1
    grid $f.sIterations -sticky w

    eval {scale $f.sMultiplier -from 0 -to 5 \
            -length 220 -variable EdConfidenceConnected(multiplier) -resolution 0.1 } \
            $Gui(WSA) {-sliderlength 22}

    $f.sMultiplier configure -showvalue 1
    grid $f.sMultiplier -sticky w

}


#called whenever we enter the ConfidenceConnected tab
proc EdConfidenceConnectedEnter {} {
}


#called whenever we exit the ConfidenceConnected tab
proc EdConfidenceConnectedExit {} {
}


#
# Where the job gets done
#
proc EdConfidenceConnectedSegment {} {
    global Label Fiducials EdConfidenceConnected Volume Ed Gui

    set e EdConfidenceConnected

    set Ed($e,scope)  3D 
    set Ed($e,input)  Original
    set Ed($e,interact) Active   

    set v [EditorGetInputID $Ed($e,input)]

    #
    ## set up the filter
    #
    catch "cc_ccif Delete"
    vtkITKConfidenceConnectedImageFilter cc_ccif
    cc_ccif SetNumberOfIterations $EdConfidenceConnected(iterations)
    cc_ccif SetMultiplier $EdConfidenceConnected(multiplier)
    cc_ccif SetReplaceValue 2

    catch "cc_cast Delete"
    vtkImageCast cc_cast
    cc_cast SetOutputScalarTypeToShort
    cc_cast SetInput [cc_ccif GetOutput]

    catch "m Delete"
    vtkMatrix4x4 m
    eval m DeepCopy [Volume($v,node) GetRasToVtkMatrix]   

    set ids [FiducialsGetPointIdListFromName default]
    foreach id $ids {
        set pt [eval m MultiplyPoint [FiducialsGetPointCoordinates $id] 1]
        set i [expr int([lindex $pt 0])]
        set j [expr int([lindex $pt 1])]
        set k [expr int([lindex $pt 2])]
        cc_ccif AddSeed $i $j $k
        puts "added seed at $i $j $k"
    }


    EdSetupBeforeApplyEffect $v $Ed($e,scope) Native
    Ed(editor)  UseInputOn

    set Gui(progressText) "Confidence Connected"

    MainStartProgress

    Ed(editor) Apply  cc_ccif cc_cast

    MainEndProgress

    Ed(editor)  SetInput ""
    Ed(editor)  UseInputOff

    EdUpdateAfterApplyEffect $v

    set w [EditorGetWorkingID]
    Volume($w,vol) SetImageData [cc_cast GetOutput]
    
    # Update pipeline and GUI
    MainVolumesUpdate $w
    
    # Render
    RenderAll
}

# this is called when the user clicks on the active slice
# we don't want to do anything special in this case
proc EdConfidenceConnectedApply {} {}
