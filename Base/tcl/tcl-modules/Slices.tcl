#=auto==========================================================================
# Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
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

	# Define Procedures
	set Module($m,procGUI) SlicesBuildGUI

	# Define Dependencies
	set Module($m,depend) ""

	# Set version info
	lappend Module(versions) [ParseCVSInfo $m \
		{$Revision: 1.12 $} {$Date: 2000/02/28 17:56:19 $}]

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

