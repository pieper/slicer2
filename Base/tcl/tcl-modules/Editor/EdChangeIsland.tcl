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
# FILE:        EdChangeIsland.tcl
# DATE:        02/22/2000 11:27
# PROCEDURES:  
#   EdChangeIslandInit
#   EdChangeIslandBuildGUI
#   EdChangeIslandEnter
#   EdChangeIslandApply
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC EdChangeIslandInit
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
	set c {button $f.bOutput -text "New Label:" \
		-command "ShowLabels" $Gui(WBA)}; eval [subst $c]
	set c {entry $f.eOutput -width 6 \
		-textvariable Label(label) $Gui(WEA)}; eval [subst $c]
	bind $f.eOutput <Return> "LabelsFindLabel"
	bind $f.eOutput <FocusOut> "LabelsFindLabel"
	set c {entry $f.eName -width 14 \
		-textvariable Label(name) $Gui(WEA) \
		-bg $Gui(activeWorkspace) -state disabled}; eval [subst $c]
	grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
	grid $f.eOutput $f.eName -sticky w
	grid $f.eName -columnspan 2

	lappend Label(colorWidgetList) $f.eName

	# Seed
	set c {label $f.lSeed -text "Location:" $Gui(WLA)}; eval [subst $c]
	set c {entry $f.eX -width 4 -textvariable Ed(EdChangeIsland,xSeed) \
		$Gui(WEA)}; eval [subst $c]
	set c {entry $f.eY -width 4 -textvariable Ed(EdChangeIsland,ySeed) \
		$Gui(WEA)}; eval [subst $c]
	set c {entry $f.eZ -width 4 -textvariable Ed(EdChangeIsland,zSeed) \
		$Gui(WEA)}; eval [subst $c]
#	grid $f.lSeed $f.eX $f.eY $f.eZ -padx $Gui(pad) -pady $Gui(pad) -sticky e

	#-------------------------------------------
	# ChangeIsland->Apply frame
	#-------------------------------------------
	set f $Ed(EdChangeIsland,frame).fApply

#	set c {button $f.bApply -text "Apply" \
#		-command "EdChangeIslandApply" $Gui(WBA) -width 8}; eval [subst $c]
#	pack $f.bApply -side top -padx $Gui(pad) -pady 2

    set c {label $f.l -text "Apply by clicking on the island." \
		$Gui(WLA)}; eval [subst $c]
	pack $f.l -side top

}

#-------------------------------------------------------------------------------
# .PROC EdChangeIslandEnter
# .END
#-------------------------------------------------------------------------------
proc EdChangeIslandEnter {} {
	global Ed

	# Color the label value 
	LabelsColorWidgets
}

#-------------------------------------------------------------------------------
# .PROC EdChangeIslandApply
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
		tk_messageBox -message "Please click on the slice with orient = $native."
		return
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

