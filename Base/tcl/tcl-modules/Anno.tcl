#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use, copy, (but NOT distribute) this
# software and its documentation for any NON-COMMERCIAL purpose, provided
# that existing copyright notices are retained verbatim in all copies.
# The authors grant permission to modify this software and its documentation 
# for any NON-COMMERCIAL purpose, provided that such modifications are not 
# distributed without the explicit consent of the authors and that existing
# copyright notices are retained in all copies. Some of the algorithms
# implemented by this software are patented, observe all applicable patent law.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
# DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
# OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
# EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
# 'AS IS' BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
#===============================================================================
# FILE:        Anno.tcl
# DATE:        01/20/2000 09:41
# LAST EDITOR: gering
# PROCEDURES:  
#   AnnoInit
#   AnnoBuildGUI
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC AnnoInit
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
Models are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
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

	set c {label $f.lTitle -text "Visibility" $Gui(WTA)}; eval [subst $c]
	pack $f.lTitle -side top -padx $Gui(pad) -pady $Gui(pad)

	# 3D
	# For "Box Axes Letters Outline", Set the Visibility of the 3D actor
	# and then re-render the 3D scene.

	foreach anno "Box Axes Letters Outline" \
		name "{3D Cube} {3D Axes} {3D Letters} {3D Outline around slices}" {

		set c {checkbutton $f.c${anno} -text "$name" \
			-variable Anno([Uncap $anno]) -indicatoron 0 \
			-command "MainAnnoSetVisibility; Render3D" $Gui(WCA)}
			eval [subst $c]
			 
		pack $f.c${anno} -side top -fill x -padx $Gui(pad) -pady $Gui(pad) 
	} 

	# 2D
	# For "Crosshairs Hashes and Mouse",
	# Set the Visibility of the 2D actor
	# and then re-render the 2D scene.

	foreach anno "Cross Hashes Mouse" \
		name "{Crosshair} {Hash marks} {Slice window text}" {

		set c {checkbutton $f.c${anno} -text "$name" \
			-variable Anno([Uncap $anno]) -indicatoron 0 \
			-command "MainAnnoSetVisibility; RenderSlices" $Gui(WCA)}
			eval [subst $c]
			 
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

	set c {label $f.l -text "Slice Cursor:" $Gui(WLA)}; eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	foreach mode "RAS IJK XY" {
		set c {radiobutton $f.f.r$mode -width 4 \
			-text "$mode" -variable Anno(cursorMode) -value $mode \
			-indicatoron 0 $Gui(WCA)}
			eval [subst $c]
		pack $f.f.r$mode -side left -padx 0 -pady 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

	#-------------------------------------------
	# Mode->Follow frame
	#-------------------------------------------
	set f $fMode.fFollow

	set c {label $f.lTitle -text "Position" $Gui(WTA)}; eval [subst $c]
	pack $f.lTitle -side top -padx $Gui(pad) -pady $Gui(pad)

	set c {checkbutton $f.cAxes -text "Axes follow focal point" \
		-variable Anno(axesFollowFocalPoint) -indicatoron 0 -width 24 \
		-command "MainAnnoUpdateFocalPoint; Render3D" $Gui(WCA)}
		eval [subst $c]
	set c {checkbutton $f.cBox -text "Cube follows focal point" \
		-variable Anno(boxFollowFocalPoint) -indicatoron 0 -width 24 \
		-command "MainAnnoUpdateFocalPoint; Render3D" $Gui(WCA)}
		eval [subst $c]
	pack $f.cAxes $f.cBox \
		-side top -fill x -padx $Gui(pad) -pady $Gui(pad) 

}

