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
# FILE:        EdErode.tcl
# PROCEDURES:  
#   EdErodeInit
#   EdErodeBuildGUI
#   EdErodeEnter
#   EdErodeApply
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdErodeInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdErodeInit {} {
	global Ed

	set e EdErode
	set Ed($e,name)      "Erode"
	set Ed($e,initials)  "Er"
	set Ed($e,desc)      "Erode: re-label perimeter pixels."
	set Ed($e,rank)      2
	set Ed($e,procGUI)   EdErodeBuildGUI
	set Ed($e,procEnter) EdErodeEnter

	# Required
	set Ed($e,scope) Multi 
	set Ed($e,input) Working

	set Ed($e,multi) Native
	set Ed($e,fill) 0
	set Ed($e,iterations) 1
	set Ed($e,neighbors) 4
}

#-------------------------------------------------------------------------------
# .PROC EdErodeBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdErodeBuildGUI {} {
	global Ed Gui Label

	set e EdErode
	#-------------------------------------------
	# Erode frame
	#-------------------------------------------
	set f $Ed(EdErode,frame)

	frame $f.fInput   -bg $Gui(activeWorkspace)
	frame $f.fScope   -bg $Gui(activeWorkspace)
	frame $f.fMulti   -bg $Gui(activeWorkspace)
	frame $f.fGrid    -bg $Gui(activeWorkspace)
	frame $f.fApply   -bg $Gui(activeWorkspace)
	pack $f.fGrid $f.fInput $f.fScope $f.fMulti $f.fApply \
		-side top -pady $Gui(pad) -fill x

	EdBuildInputGUI $Ed($e,frame).fInput Ed($e,input)
	EdBuildScopeGUI $Ed($e,frame).fScope Ed($e,scope) 
	EdBuildMultiGUI $Ed($e,frame).fMulti Ed($e,multi) 

	#-------------------------------------------
	# Erode->Grid frame
	#-------------------------------------------
	set f $Ed(EdErode,frame).fGrid

	# Fields for background, foreground pixel values
	eval {button $f.bBack -text "Value to Erode:" \
		-command "ShowLabels"} $Gui(WBA)
	eval {entry $f.eBack -width 6 -textvariable Label(label)} $Gui(WEA)
	bind $f.eBack <Return>   "LabelsFindLabel"
	bind $f.eBack <FocusOut> "LabelsFindLabel"
	eval {entry $f.eName -width 6 \
		-textvariable Label(name)} $Gui(WEA) \
		{-bg $Gui(activeWorkspace) -state disabled}
	eval {label $f.lFore -text "Fill value: "} $Gui(WLA)
	eval {entry $f.eFore -width 6 \
		-textvariable Ed(EdErode,fill)} $Gui(WEA)
	eval {label $f.lIter -text "Iterations: "} $Gui(WLA)
	eval {entry $f.eIter -width 6 \
		-textvariable Ed(EdErode,iterations)} $Gui(WEA)
	grid $f.bBack $f.eBack $f.eName -padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.lFore $f.eFore -padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.lIter $f.eIter -padx $Gui(pad) -pady $Gui(pad) -sticky e

	lappend Label(colorWidgetList) $f.eName

	# Neighborhood Size
    eval {label $f.lNeighbor -text "Neighborhood Size: "} $Gui(WLA)
    frame $f.fNeighbor -bg $Gui(activeWorkspace)
    foreach mode "4 8" {
        eval {radiobutton $f.fNeighbor.r$mode \
            -text "$mode" -variable Ed(EdErode,neighbors) -value $mode -width 2 \
            -indicatoron 0} $Gui(WCA)
        pack $f.fNeighbor.r$mode -side left -padx 0
    }
    grid $f.lNeighbor $f.fNeighbor -padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.fNeighbor -sticky w


	#-------------------------------------------
	# Erode->Apply frame
	#-------------------------------------------
	set f $Ed(EdErode,frame).fApply

	eval {button $f.bErode -text "Erode" \
		-command "EdErodeApply Erode"} $Gui(WBA)
	eval {button $f.bDilate -text "Dilate" \
		-command "EdErodeApply Dilate"} $Gui(WBA)
	eval {button $f.bED -text "Erode & Dilate" \
		-command "EdErodeApply ErodeDilate"} $Gui(WBA)
	eval {button $f.bDE -text "Dilate & Erode" \
		-command "EdErodeApply DilateErode"} $Gui(WBA)
	grid $f.bErode  $f.bED -padx $Gui(pad) -pady $Gui(pad)
	grid $f.bDilate $f.bDE -padx $Gui(pad) -pady $Gui(pad)

}

#-------------------------------------------------------------------------------
# .PROC EdErodeEnter
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdErodeEnter {} {
	global Ed

	LabelsColorWidgets
}

#-------------------------------------------------------------------------------
# .PROC EdErodeApply
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EdErodeApply {effect} {
	global Ed Volume Label Gui

	set e EdErode
	set v [EditorGetInputID $Ed($e,input)]

	# Validate input
	if {[ValidateInt $Ed($e,fill)] == 0} {
		tk_messageBox -message "Fill value is not an integer."
		return
	}
	if {[ValidateInt $Ed($e,iterations)] == 0} {
		tk_messageBox -message "Iterations is not an integer."
		return
	}
	if {[ValidateInt $Label(label)] == 0} {
		tk_messageBox -message "Value To Erode is not an integer."
		return
	}

	EdSetupBeforeApplyEffect $v $Ed($e,scope) $Ed($e,multi)

	set Gui(progressText) "$effect [Volume($v,node) GetName]"
	
	set fg         $Label(label)
	set bg         $Ed($e,fill)
	set neighbors  $Ed($e,neighbors)     
	set iterations $Ed($e,iterations)
	Ed(editor)     $effect $fg $bg $neighbors $iterations
	Ed(editor)     SetInput ""
	Ed(editor)     UseInputOff

	EdUpdateAfterApplyEffect $v
}

