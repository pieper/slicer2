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
# FILE:        EdChangeIsland.tcl
# PROCEDURES:  
#   EdChangeIslandInit
#   EdChangeIslandBuildGUI
#   EdChangeIslandEnter
#   EdChangeIslandApply
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC EdChangeIslandInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdChangeIslandInit {} {
    global Ed Gui

    set e EdChangeIsland
    set Ed($e,name)      "Change Island"
    set Ed($e,initials)  "CI"
    set Ed($e,desc)      "Change Island: re-label an island of pixels."
    set Ed($e,rank)      6
    set Ed($e,procGUI)   EdChangeIslandBuildGUI
    set Ed($e,procEnter) EdChangeIslandEnter

    # Required
    set Ed($e,scope) Single 
    set Ed($e,input) Working

    set Ed($e,render) Active
    set Ed($e,xSeed) 0
    set Ed($e,ySeed) 0
    set Ed($e,zSeed) 0
}

#-------------------------------------------------------------------------------
# .PROC EdChangeIslandBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdChangeIslandBuildGUI {} {
    global Ed Gui Label

    #-------------------------------------------
    # ChangeIsland frame
    #-------------------------------------------
    set f $Ed(EdChangeIsland,frame)

    frame $f.fGrid    -bg $Gui(activeWorkspace)
    frame $f.fInput   -bg $Gui(activeWorkspace)
    frame $f.fScope   -bg $Gui(activeWorkspace)
    frame $f.fRender  -bg $Gui(activeWorkspace)
    frame $f.fApply   -bg $Gui(activeWorkspace)
    pack $f.fGrid $f.fInput $f.fScope $f.fRender $f.fApply \
        -side top -pady $Gui(pad) -fill x

    EdBuildScopeGUI $Ed(EdChangeIsland,frame).fScope Ed(EdChangeIsland,scope) Multi
    EdBuildInputGUI $Ed(EdChangeIsland,frame).fInput Ed(EdChangeIsland,input)
    EdBuildRenderGUI $Ed(EdChangeIsland,frame).fRender Ed(EdChangeIsland,render)

    #-------------------------------------------
    # ChangeIsland->Grid frame
    #-------------------------------------------
    set f $Ed(EdChangeIsland,frame).fGrid

    # New label
    eval {button $f.bOutput -text "New Label:" -command "ShowLabels"} $Gui(WBA)
    eval {entry $f.eOutput -width 6 \
        -textvariable Label(label)} $Gui(WEA)
    bind $f.eOutput <Return> "LabelsFindLabel"
    bind $f.eOutput <FocusOut> "LabelsFindLabel"
    eval {entry $f.eName -width 14 \
        -textvariable Label(name)} $Gui(WEA) \
        {-bg $Gui(activeWorkspace) -state disabled}
    grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
    grid $f.eOutput $f.eName -sticky w
    grid $f.eName -columnspan 2

    lappend Label(colorWidgetList) $f.eName

    # Seed
    eval {label $f.lSeed -text "Location:"} $Gui(WLA)
    eval {entry $f.eX -width 4 -textvariable Ed(EdChangeIsland,xSeed)} $Gui(WEA)
    eval {entry $f.eY -width 4 -textvariable Ed(EdChangeIsland,ySeed)} $Gui(WEA)
    eval {entry $f.eZ -width 4 -textvariable Ed(EdChangeIsland,zSeed)} $Gui(WEA)
#    grid $f.lSeed $f.eX $f.eY $f.eZ -padx $Gui(pad) -pady $Gui(pad) -sticky e

    #-------------------------------------------
    # ChangeIsland->Apply frame
    #-------------------------------------------
    set f $Ed(EdChangeIsland,frame).fApply

#    eval {button $f.bApply -text "Apply" \
#        -command "EdChangeIslandApply"} $Gui(WBA) {-width 8}
#    pack $f.bApply -side top -padx $Gui(pad) -pady 2

    eval {label $f.l -text "Apply by clicking on the island."} $Gui(WLA)
    pack $f.l -side top

}

#-------------------------------------------------------------------------------
# .PROC EdChangeIslandEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdChangeIslandEnter {} {
    global Ed

    # Color the label value 
    LabelsColorWidgets
}

#-------------------------------------------------------------------------------
# .PROC EdChangeIslandApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdChangeIslandApply {} {
    global Ed Volume Label Gui

    set e EdChangeIsland
    set v [EditorGetInputID $Ed($e,input)]

    # Validate input
    if {[ValidateInt $Label(label)] == 0} {
        tk_messageBox -message "New Label is not an integer."
        return
    }

    EdSetupBeforeApplyEffect $v $Ed($e,scope) Native

    # Only apply to native slices
    if {[set native [EdIsNativeSlice]] != ""} {
        # TODO - remove comment if this works
        puts "Warning: check results - now allowed, but fromerly failed with:"
        puts " Please click on the slice with orient = $native."
        puts "-sp 2002-09-30"
        #tk_messageBox -message "Please click on the slice with orient = $native."
        #return
    }

    set Gui(progressText) "Change Island in [Volume($v,node) GetName]"
    
    set fg      $Label(label)
    set x       $Ed($e,xSeed)
    set y       $Ed($e,ySeed)
    set z       $Ed($e,zSeed)
    Ed(editor)  ChangeIsland $fg $x $y $z
    Ed(editor)  SetInput ""
    Ed(editor)  UseInputOff

    EdUpdateAfterApplyEffect $v $Ed($e,render)
}

