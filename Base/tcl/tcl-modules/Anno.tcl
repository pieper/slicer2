#=auto==========================================================================
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

	# Define Procedures
	set Module($m,procGUI) AnnoBuildGUI

	# Define Dependencies
	set Module($m,depend) ""

	# Set version info
	lappend Module(versions) [ParseCVSInfo $m \
		{$Revision: 1.12 $} {$Date: 2001/02/19 17:53:27 $}]
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
	#	  cBox
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
Press a button in to make its corresponding type of annotation visible.
Note that there is 3D annotation and 2D annotation for the slice windows.
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
	frame $f.fFollow -bg $Gui(activeWorkspace)
	pack $f.fCoords $f.fFollow -side top -pady $Gui(pad)

	#-------------------------------------------
	# Mode->Coords frame
	#-------------------------------------------
	set f $fMode.fCoords

	eval {label $f.l -text "Slice Cursor:"} $Gui(WLA)
	frame $f.f -bg $Gui(activeWorkspace)
	foreach mode "RAS IJK XY" {
		eval {radiobutton $f.f.r$mode -width 4 \
			-text "$mode" -variable Anno(cursorMode) -value $mode \
			-indicatoron 0} $Gui(WCA)
		pack $f.f.r$mode -side left -padx 0 -pady 0
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

