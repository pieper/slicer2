#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use and copy (but not distribute) this
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
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   SlicesBuildGUI
#==========================================================================auto=

proc SlicesInit {} {
	global Module

	# Define Tabs
	set m Slices
	set Module($m,row1List) "Help Controls"
	set Module($m,row1Name) "{Help} {Controls}"
	set Module($m,row1,tab) Controls

	# Define Procedures
	set Module($m,procGUI) SlicesBuildGUI
}

#-------------------------------------------------------------------------------
# .PROC SlicesBuildGUI
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
"
	regsub -all "\n" $help {} help
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
}
