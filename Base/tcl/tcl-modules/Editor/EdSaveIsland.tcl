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
# FILE:        EdSaveIsland.tcl
# PROCEDURES:  
#   EdSaveIslandInit
#   EdSaveIslandBuildGUI
#   EdSaveIslandApply
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdSaveIslandInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdSaveIslandInit {} {
    global Ed Gui

    set e EdSaveIsland
    set Ed($e,name)      "Save Island"
    set Ed($e,initials)  "SI"
    set Ed($e,desc)      "Save Island: remove all but one island."
    set Ed($e,rank)      7
    set Ed($e,procGUI)   EdSaveIslandBuildGUI

    # Required
    set Ed($e,scope) Single 
    set Ed($e,input) Working

    set Ed($e,xSeed) 0
    set Ed($e,ySeed) 0
    set Ed($e,zSeed) 0
}

#-------------------------------------------------------------------------------
# .PROC EdSaveIslandBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdSaveIslandBuildGUI {} {
    global Ed Gui

    #-------------------------------------------
    # SaveIsland frame
    #-------------------------------------------
    set f $Ed(EdSaveIsland,frame)

    frame $f.fInput   -bg $Gui(activeWorkspace)
    frame $f.fScope   -bg $Gui(activeWorkspace)
    frame $f.fGrid    -bg $Gui(activeWorkspace)
    frame $f.fApply   -bg $Gui(activeWorkspace)
    pack $f.fGrid $f.fInput $f.fScope $f.fApply \
        -side top -pady $Gui(pad) -fill x

    EdBuildScopeGUI $Ed(EdSaveIsland,frame).fScope Ed(EdSaveIsland,scope) Multi
    EdBuildInputGUI $Ed(EdSaveIsland,frame).fInput Ed(EdSaveIsland,input)

    #-------------------------------------------
    # SaveIsland->Grid frame
    #-------------------------------------------
    set f $Ed(EdSaveIsland,frame).fGrid

    # Seed
    eval {label $f.lSeed -text "Location:"} $Gui(WLA)
    eval {entry $f.eX -width 4 -textvariable Ed(EdSaveIsland,xSeed)} $Gui(WEA)
    eval {entry $f.eY -width 4 -textvariable Ed(EdSaveIsland,ySeed)} $Gui(WEA)
    eval {entry $f.eZ -width 4 -textvariable Ed(EdSaveIsland,zSeed)} $Gui(WEA)
#    grid $f.lSeed $f.eX $f.eY $f.eZ -padx $Gui(pad) -pady $Gui(pad) -sticky e

    #-------------------------------------------
    # SaveIsland->Apply frame
    #-------------------------------------------
    set f $Ed(EdSaveIsland,frame).fApply

#    eval {button $f.bApply -text "Apply" \
#        -command "EdSaveIslandApply"} $Gui(WBA) {-width 8}
#    pack $f.bApply -side top -padx $Gui(pad) -pady 2

    eval {label $f.l -text "Apply by clicking on the island."} $Gui(WLA)
    pack $f.l -side top

}

#-------------------------------------------------------------------------------
# .PROC EdSaveIslandApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdSaveIslandApply {} {
    global Ed Volume Gui

    set e EdSaveIsland
    set v [EditorGetInputID $Ed($e,input)]

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

    set Gui(progressText) "Save Island in [Volume($v,node) GetName]"
    
    set x       $Ed($e,xSeed)
    set y       $Ed($e,ySeed)
    set z       $Ed($e,zSeed)
    Ed(editor)  SaveIsland $x $y $z
    Ed(editor)  SetInput ""
    Ed(editor)  UseInputOff

    EdUpdateAfterApplyEffect $v
}

