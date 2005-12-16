#=auto==========================================================================
# (c) Copyright 2003 Massachusetts Institute of Technology (MIT) All Rights Reserved.
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
# FILE:        EdErode.tcl
# PROCEDURES:  
#   EdErodeInit
#   EdErodeBuildGUI
#   EdErodeEnter
#   EdErodeApply
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdErodeInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdErodeInit {} {
    global Ed

    set e EdErode
    set Ed($e,name)      "Erode"
    set Ed($e,initials)  "Er"
    set Ed($e,desc)      "Erode: re-label perimeter pixels."
    set Ed($e,rank)      2
    set Ed($e,procGUI)   EdErodeBuildGUI
    set Ed($e,procEnter) EdErodeEnter

    # Required
    set Ed($e,scope) Multi 
    set Ed($e,input) Working

    set Ed($e,multi) Native
    set Ed($e,fill) 0
    set Ed($e,iterations) 1
    set Ed($e,neighbors) 4
}

#-------------------------------------------------------------------------------
# .PROC EdErodeBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdErodeBuildGUI {} {
    global Ed Gui Label

    set e EdErode
    #-------------------------------------------
    # Erode frame
    #-------------------------------------------
    set f $Ed(EdErode,frame)

    frame $f.fInput   -bg $Gui(activeWorkspace)
    frame $f.fScope   -bg $Gui(activeWorkspace)
    frame $f.fMulti   -bg $Gui(activeWorkspace)
    frame $f.fGrid    -bg $Gui(activeWorkspace)
    frame $f.fApply   -bg $Gui(activeWorkspace)
    pack $f.fGrid $f.fInput $f.fScope $f.fMulti $f.fApply \
        -side top -pady $Gui(pad) -fill x

    EdBuildInputGUI $Ed($e,frame).fInput Ed($e,input)
    EdBuildScopeGUI $Ed($e,frame).fScope Ed($e,scope) 
    EdBuildMultiGUI $Ed($e,frame).fMulti Ed($e,multi) 

    #-------------------------------------------
    # Erode->Grid frame
    #-------------------------------------------
    set f $Ed(EdErode,frame).fGrid

    # Fields for background, foreground pixel values
    eval {button $f.bBack -text "Value to Erode:" \
        -command "ShowLabels"} $Gui(WBA)
    eval {entry $f.eBack -width 6 -textvariable Label(label)} $Gui(WEA)
    bind $f.eBack <Return>   "LabelsFindLabel"
    bind $f.eBack <FocusOut> "LabelsFindLabel"
    eval {entry $f.eName -width 6 \
        -textvariable Label(name)} $Gui(WEA) \
        {-bg $Gui(activeWorkspace) -state disabled}
    eval {label $f.lFore -text "Fill value: "} $Gui(WLA)
    eval {entry $f.eFore -width 6 \
        -textvariable Ed(EdErode,fill)} $Gui(WEA)
    eval {label $f.lIter -text "Iterations: "} $Gui(WLA)
    eval {entry $f.eIter -width 6 \
        -textvariable Ed(EdErode,iterations)} $Gui(WEA)
    grid $f.bBack $f.eBack $f.eName -padx $Gui(pad) -pady $Gui(pad) -sticky e
    grid $f.lFore $f.eFore -padx $Gui(pad) -pady $Gui(pad) -sticky e
    grid $f.lIter $f.eIter -padx $Gui(pad) -pady $Gui(pad) -sticky e

    lappend Label(colorWidgetList) $f.eName

    # Neighborhood Size
    eval {label $f.lNeighbor -text "Neighborhood Size: "} $Gui(WLA)
    frame $f.fNeighbor -bg $Gui(activeWorkspace)
    foreach mode "4 8" {
        eval {radiobutton $f.fNeighbor.r$mode \
            -text "$mode" -variable Ed(EdErode,neighbors) -value $mode -width 2 \
            -indicatoron 0} $Gui(WCA)
        pack $f.fNeighbor.r$mode -side left -padx 0
    }
    grid $f.lNeighbor $f.fNeighbor -padx $Gui(pad) -pady $Gui(pad) -sticky e
    grid $f.fNeighbor -sticky w


    #-------------------------------------------
    # Erode->Apply frame
    #-------------------------------------------
    set f $Ed(EdErode,frame).fApply

    eval {button $f.bErode -text "Erode" \
        -command "EdErodeApply Erode"} $Gui(WBA)
    eval {button $f.bDilate -text "Dilate" \
        -command "EdErodeApply Dilate"} $Gui(WBA)
    eval {button $f.bED -text "Erode & Dilate" \
        -command "EdErodeApply ErodeDilate"} $Gui(WBA)
    eval {button $f.bDE -text "Dilate & Erode" \
        -command "EdErodeApply DilateErode"} $Gui(WBA)
    grid $f.bErode  $f.bED -padx $Gui(pad) -pady $Gui(pad)
    grid $f.bDilate $f.bDE -padx $Gui(pad) -pady $Gui(pad)

}

#-------------------------------------------------------------------------------
# .PROC EdErodeEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdErodeEnter {} {
    global Ed

    LabelsColorWidgets
}

#-------------------------------------------------------------------------------
# .PROC EdErodeApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdErodeApply {effect} {
    global Ed Volume Label Gui

    set e EdErode
    set v [EditorGetInputID $Ed($e,input)]

    # Validate input
    if {[ValidateInt $Ed($e,fill)] == 0} {
        DevErrorWindow "Fill value is not an integer."
        return
    }
    if {[ValidateInt $Ed($e,iterations)] == 0} {
        DevErrorWindow "Iterations is not an integer."
        return
    }
    if {[ValidateInt $Label(label)] == 0} {
        DevErrorWindow "Value To Erode is not an integer."
        return
    }
    if { $Ed($e,iterations) > 1 && $Ed($e,scope) == "3D" } {
        DevErrorWindow "Multiple iterations in 3D scope not supported"
        set Ed($e,iterations) 1
    }

    EdSetupBeforeApplyEffect $v $Ed($e,scope) $Ed($e,multi)

    set Gui(progressText) "$effect [Volume($v,node) GetName]"
    
    set fg         $Label(label)
    set bg         $Ed($e,fill)
    set neighbors  $Ed($e,neighbors)     
    set iterations $Ed($e,iterations)
    Ed(editor)     $effect $fg $bg $neighbors $iterations
    Ed(editor)     SetInput ""
    Ed(editor)     UseInputOff

    EdUpdateAfterApplyEffect $v
}

