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
# FILE:        Anno.tcl
# PROCEDURES:  
#   AnnoInit
#   AnnoBuildGUI
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC AnnoInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc AnnoInit {} {
    global Anno Module

    # Define Tabs
    set m Anno
    set Module($m,row1List) "Help Visibility Mode"
    set Module($m,row1Name) "Help Visibility Mode"
    set Module($m,row1,tab) Visibility

    # Module Summary Info
    set Module($m,overview) "Show/hide/change slice window text and 3d box."

    # Define Procedures
    set Module($m,procGUI) AnnoBuildGUI

    # Define Dependencies
    set Module($m,depend) ""

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.19 $} {$Date: 2003/03/19 19:16:30 $}]
}

#-------------------------------------------------------------------------------
# .PROC AnnoBuildGUI
#
# Set up 2 frames: The Info frame and the frame to set visibility of 
# annotations.
# .END
#-------------------------------------------------------------------------------
proc AnnoBuildGUI {} {
    global Module Gui Anno View Slice

    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Visibility
    #   Vis
    #      cBox
    #     cAxes
    #     cLetters
    #     cOutline
    #     cCross
    #     cHashes
    #     cMouse
    # Mode
    #   Coords
    #   Follow
    #-------------------------------------------

    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    set help "
This module allows the user to control the text displayed on the slices
and in 3D, as well as the 3D box, etc.

<BR>
The <B>Visibility</B> tab allows you to turn off and on the annotation.
Press a button in to make its corresponding type of annotation visible.
Note that there is 3D annotation and 2D annotation for the slice windows.

<BR> The <B>Mode</B> tab controls the type of coordinates and pixel
values displayed on the 2D slices, as well as the 3D behavior of the
cube and axes.  
"
    regsub -all "\n" $help { } help
    MainHelpApplyTags Anno $help
    MainHelpBuildGUI Anno

    #-------------------------------------------
    # Visibility frame
    #-------------------------------------------
    set fVisibility $Module(Anno,fVisibility)
    set f $fVisibility

    # Frames
    frame $f.fVis -bg $Gui(activeWorkspace)
    pack $f.fVis -side top 

    #-------------------------------------------
    # Anno->Vis frame
    #-------------------------------------------
    set f $fVisibility.fVis

    eval {label $f.lTitle -text "Visibility"} $Gui(WTA)
    pack $f.lTitle -side top -padx $Gui(pad) -pady $Gui(pad)

    # 3D
    # For "Box Axes Letters Outline", Set the Visibility of the 3D actor
    # and then re-render the 3D scene.

    foreach anno "Box Axes Letters Outline" \
        name "{3D Cube} {3D Axes} {3D Letters} {3D Outline around slices}" {

        eval {checkbutton $f.c${anno} -text "$name" \
            -variable Anno([Uncap $anno]) -indicatoron 0 \
            -command "MainAnnoSetVisibility; Render3D"} $Gui(WCA)
             
        pack $f.c${anno} -side top -fill x -padx $Gui(pad) -pady $Gui(pad) 
    } 

    # 2D
    # For "Crosshairs Hashes and Mouse",
    # Set the Visibility of the 2D actor
    # and then re-render the 2D scene.

    foreach anno "Cross Hashes Mouse" \
        name "{Crosshair} {Hash marks} {Slice window text}" {

        eval {checkbutton $f.c${anno} -text "$name" \
            -variable Anno([Uncap $anno]) -indicatoron 0 \
            -command "MainAnnoSetVisibility; RenderSlices"} $Gui(WCA)
             
        pack $f.c${anno} -side top -fill x -padx $Gui(pad) -pady $Gui(pad) 
    }


    #-------------------------------------------
    # Mode frame
    #-------------------------------------------
    set fMode $Module(Anno,fMode)
    set f $fMode

    # Frames
    frame $f.fCoords -bg $Gui(activeWorkspace)
    frame $f.fPrecision -bg $Gui(activeWorkspace)
    frame $f.fFollow -bg $Gui(activeWorkspace)
    pack $f.fCoords $f.fPrecision $f.fFollow -side top -pady $Gui(pad)

    #-------------------------------------------
    # Mode->Coords frame
    #-------------------------------------------
    set f $fMode.fCoords

    set tip1 "Display of coordinates on 2D slices:\n"
    eval {label $f.l -text "Slice Cursor:"} $Gui(WLA)
    frame $f.f -bg $Gui(activeWorkspace)
    foreach mode "RAS IJK XY" \
        tip {"RAS coordinates" "array indices into volume" \
        "2D slice coordinates"} {
        eval {radiobutton $f.f.r$mode -width 4 \
            -text "$mode" -variable Anno(cursorMode) -value $mode \
            -indicatoron 0} $Gui(WCA)
        pack $f.f.r$mode -side left -padx 0 -pady 0
        TooltipAdd $f.f.r$mode "$tip1 $tip"
    }
    pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

    #-------------------------------------------
    # Mode->Precision frame
    #-------------------------------------------
    set f $fMode.fPrecision

    set tip1 "Display of pixel values on 2D slices:\n"
    eval {label $f.l -text "Pixel Display:"} $Gui(WLA)
    frame $f.f -bg $Gui(activeWorkspace)
    foreach button "int flt full" \
        mode "%.f %6.2f %f" \
        text "1 1.00 full" \
        tip {"integer display" "floating point" \
        "full: all decimal places shown"} \
        {
        eval {radiobutton $f.f.r$button -width 4 \
            -text "$text" -variable Anno(pixelDispFormat) \
            -value $mode \
            -indicatoron 0} $Gui(WCA)
        pack $f.f.r$button -side left -padx 0 -pady 0
        TooltipAdd $f.f.r$button "$tip1 $tip"
        
    }
    pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

    #-------------------------------------------
    # Mode->Follow frame
    #-------------------------------------------
    set f $fMode.fFollow

    eval {label $f.lTitle -text "Position"} $Gui(WTA)
    pack $f.lTitle -side top -padx $Gui(pad) -pady $Gui(pad)

    eval {checkbutton $f.cAxes -text "Axes follow focal point" \
        -variable Anno(axesFollowFocalPoint) -indicatoron 0 -width 24 \
        -command "MainAnnoUpdateFocalPoint; Render3D"} $Gui(WCA)
    eval {checkbutton $f.cBox -text "Cube follows focal point" \
        -variable Anno(boxFollowFocalPoint) -indicatoron 0 -width 24 \
        -command "MainAnnoUpdateFocalPoint; Render3D"} $Gui(WCA)
    pack $f.cAxes $f.cBox \
        -side top -fill x -padx $Gui(pad) -pady $Gui(pad) 

}

