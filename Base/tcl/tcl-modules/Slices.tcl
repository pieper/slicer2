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
# FILE:        Slices.tcl
# PROCEDURES:  
#   SlicesBuildGUI
#==========================================================================auto=

proc SlicesInit {} {
	global Module Slice

	# Define Tabs
	set m Slices
	set Module($m,row1List) "Help Controls"
	set Module($m,row1Name) "{Help} {Controls}"
	set Module($m,row1,tab) Controls

    # Module Summary Info
    set Module($m,overview) "Display of the 3 slices."

	# Define Procedures
	set Module($m,procGUI) SlicesBuildGUI

	# Define Dependencies
	set Module($m,depend) ""

	# Set version info
	lappend Module(versions) [ParseCVSInfo $m \
		{$Revision: 1.15 $} {$Date: 2002/01/26 23:34:32 $}]

	# Props
	set Slice(prefix) slice
	set Slice(ext) .tif
}

#-------------------------------------------------------------------------------
# .PROC SlicesBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SlicesBuildGUI {} {
	global Slice Module Gui

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Controls
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Use these slice controls when the <B>View Mode</B> is set to 3D.
<BR>
The <B>Active Slice</B> is the slice you last clicked on with the mouse.
<BR>
<B>TIP</B></BR>
The AxiSlice, SagSlice, CorSlice, and OrigSlice orientations
produces slices relative to the originally scanned volume.
The other options produces slices at arbitrary orientations in millimeter space.
"
	regsub -all "\n" $help { } help
	MainHelpApplyTags Slices $help
	MainHelpBuildGUI Slices

	#-------------------------------------------
	# Controls frame
	#-------------------------------------------
	set fControls $Module(Slices,fControls)
	set f $fControls

	# Controls->Slice$s frames
	#-------------------------------------------
	foreach s $Slice(idList) {

		frame $f.fSlice$s -bg $Gui(activeWorkspace)
		pack $f.fSlice$s -side top -pady $Gui(pad) -expand 1 -fill both

		MainSlicesBuildControls $s $f.fSlice$s
	}

	frame $f.fActive -bg $Gui(activeWorkspace)
	frame $f.fSave   -bg $Gui(activeWorkspace)
	pack $f.fActive $f.fSave -side top -pady $Gui(pad) -expand 1 -fill x

	#-------------------------------------------
	# Active frame
	#-------------------------------------------
	set f $fControls.fActive

	eval {label $f.lActive -text "Active Slice:"} $Gui(WLA)
	pack $f.lActive -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

	foreach s $Slice(idList) text "Red Yellow Green" width "4 7 6" {
		eval {radiobutton $f.r$s -width $width -indicatoron 0\
			-text "$text" -value "$s" -variable Slice(activeID) \
			-command "MainSlicesSetActive"} $Gui(WCA) {-selectcolor $Gui(slice$s)}
		pack $f.r$s -side left -fill x -anchor e
	}

	#-------------------------------------------
	# Save frame
	#-------------------------------------------
	set f $fControls.fSave

	eval {button $f.bSave -text "Save Active" -width 12 \
		-command "MainSlicesSave"} $Gui(WBA)
	eval {entry $f.eSave -textvariable Slice(prefix)} $Gui(WEA)
	bind $f.eSave <Return> {MainSlicesSavePopup}
	pack $f.bSave -side left -padx 3
	pack $f.eSave -side left -padx 2 -expand 1 -fill x
}

