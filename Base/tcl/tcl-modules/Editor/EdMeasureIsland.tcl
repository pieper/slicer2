#=auto==========================================================================
# Copyright (c) 2001 Surgical Planning Lab, Brigham and Women's Hospital
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
# FILE:        EdMeasureIsland.tcl
# PROCEDURES:  
#   EdMeasureIslandInit
#   EdMeasureIslandBuildGUI
#   EdMeasureIslandEnter
#   EdMeasureIslandApply
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdMeasureIslandInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdMeasureIslandInit {} {
	global Ed Gui

	set e EdMeasureIsland
	set Ed($e,name)      "Measure Island"
	set Ed($e,initials)  "MI"
	set Ed($e,desc)      "Measure Island size."
	set Ed($e,rank)      8
	set Ed($e,procGUI)   EdMeasureIslandBuildGUI
	set Ed($e,procEnter) EdMeasureIslandEnter

	# Required
	set Ed($e,scope) Single 
	set Ed($e,input) Working

	set Ed($e,inputLabel) 0
	set Ed($e,xSeed)   0
	set Ed($e,ySeed)   0
	set Ed($e,zSeed)   0
	set Ed($e,size)    0
	set Ed($e,largest) 0
}

#-------------------------------------------------------------------------------
# .PROC EdMeasureIslandBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdMeasureIslandBuildGUI {} {
	global Ed Gui Label

	#-------------------------------------------
	# MeasureIsland frame
	#-------------------------------------------
	set f $Ed(EdMeasureIsland,frame)

	frame $f.fInput   -bg $Gui(activeWorkspace)
	frame $f.fScope   -bg $Gui(activeWorkspace)
	frame $f.fGrid    -bg $Gui(activeWorkspace)
	frame $f.fApply   -bg $Gui(activeWorkspace)
	frame $f.fResults -bg $Gui(activeWorkspace)
	pack $f.fGrid $f.fInput $f.fScope $f.fApply $f.fResults \
		-side top -pady $Gui(pad) -fill x

	EdBuildScopeGUI $Ed(EdMeasureIsland,frame).fScope Ed(EdMeasureIsland,scope) Multi
	EdBuildInputGUI $Ed(EdMeasureIsland,frame).fInput Ed(EdMeasureIsland,input)

	#-------------------------------------------
	# MeasureIsland->Grid frame
	#-------------------------------------------
	set f $Ed(EdMeasureIsland,frame).fGrid

	# New label
	eval {button $f.bOutput -text "Island Label:" -command "ShowLabels"} $Gui(WBA)
	eval {entry $f.eOutput -width 6 -textvariable Label(label)} $Gui(WEA)
	bind $f.eOutput <Return> "LabelsFindLabel"
	bind $f.eOutput <FocusOut> "LabelsFindLabel"
	eval {entry $f.eName -width 14 -textvariable Label(name)} $Gui(WEA) \
		{-bg $Gui(activeWorkspace) -state disabled}
	grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
	grid $f.eOutput $f.eName -sticky w
	grid $f.eName -columnspan 2

	lappend Label(colorWidgetList) $f.eName

	# Seed
	eval {label $f.lSeed -text "IJK Location:"} $Gui(WLA)
	eval {entry $f.eX -width 4 -textvariable Ed(EdMeasureIsland,xSeed)} $Gui(WEA)
	eval {entry $f.eY -width 4 -textvariable Ed(EdMeasureIsland,ySeed)} $Gui(WEA)
	eval {entry $f.eZ -width 4 -textvariable Ed(EdMeasureIsland,zSeed)} $Gui(WEA)
#	grid $f.lSeed $f.eX $f.eY $f.eZ -padx $Gui(pad) -pady $Gui(pad) -sticky e

	#-------------------------------------------
	# MeasureIsland->Apply frame
	#-------------------------------------------
	set f $Ed(EdMeasureIsland,frame).fApply

#	eval {button $f.bApply -text "Apply" \
#		-command "EdMeasureIslandApply"} $Gui(WBA) {-width 8}
#	pack $f.bApply -side top -padx $Gui(pad) -pady 2

    eval {label $f.l -text "Apply by clicking on the island."} $Gui(WLA)
	pack $f.l -side top

	#-------------------------------------------
	# MeasureIsland->Results frame
	#-------------------------------------------
	set f $Ed(EdMeasureIsland,frame).fResults

    # Title
	eval {label $f.l -text "Results:"} $Gui(WLA)
	grid $f.l

	# Size
	eval {label $f.lSize -text "Size:"} $Gui(WLA)
	eval {entry $f.eSize -width 9 \
		-textvariable Ed(EdMeasureIsland,size)} $Gui(WEA)
	grid $f.lSize $f.eSize -padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.eSize -sticky w
	
	# Largest
	eval {label $f.lLargest -text "Largest:"} $Gui(WLA)
	eval {entry $f.eLargest -width 9 \
		-textvariable Ed(EdMeasureIsland,largest)} $Gui(WEA)
 	grid $f.lLargest $f.eLargest -padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.eLargest -sticky w
}

#-------------------------------------------------------------------------------
# .PROC EdMeasureIslandEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdMeasureIslandEnter {} {
	global Ed

	# Color the label value 
	LabelsColorWidgets
}

#-------------------------------------------------------------------------------
# .PROC EdMeasureIslandApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdMeasureIslandApply {} {
	global Ed Volume Label Gui

	set e EdMeasureIsland
	set v [EditorGetInputID $Ed($e,input)]

	# Validate input
	if {[ValidateInt $Ed($e,inputLabel)] == 0} {
		tk_messageBox -message "Island Label is not an integer."
		return
	}

	EdSetupBeforeApplyEffect $v $Ed($e,scope) Native

	# Only apply to native slices
	if {[set native [EdIsNativeSlice]] != ""} {
		tk_messageBox -message "Please click on the slice with orient = $native."
		return
	}

	set Gui(progressText) "Measure Island in [Volume($v,node) GetName]"
	
	# Determine the input label
	set Label(label) $Ed($e,inputLabel)
	LabelsFindLabel
	
	set x       $Ed($e,xSeed)
	set y       $Ed($e,ySeed)
	set z       $Ed($e,zSeed)
	Ed(editor)  MeasureIsland $x $y $z
	Ed(editor)  SetInput ""
	Ed(editor)  UseInputOff

	# Show results
	set Ed($e,size)    [Ed(editor) GetIslandSize]
	set Ed($e,largest) [Ed(editor) GetLargestIslandSize]

	EdUpdateAfterApplyEffect $v
}

