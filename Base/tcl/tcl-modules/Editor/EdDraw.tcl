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
# FILE:        EdDraw.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   EdDrawInit
#   EdDrawBuildGUI
#   EdDrawEnter
#   EdDrawExit
#   EdDrawUpdate
#   EdDrawApply
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC EdDrawInit
# .END
#-------------------------------------------------------------------------------
proc EdDrawInit {} {
	global Ed

	set e EdDraw
	set Ed($e,name)      "Draw"
	set Ed($e,desc)      "Draw on the image using a brush."
	set Ed($e,rank)      4
	set Ed($e,procGUI)   EdDrawBuildGUI
	set Ed($e,procEnter) EdDrawEnter
	set Ed($e,procExit)  EdDrawExit

	# Required
	set Ed($e,scope) Single 
	set Ed($e,input) Working

	set Ed($e,mode)   Draw
	set Ed($e,radius) 0
	set Ed($e,shape)  Polygon
	set Ed($e,render) Active
}

#-------------------------------------------------------------------------------
# .PROC EdDrawBuildGUI
# .END
#-------------------------------------------------------------------------------
proc EdDrawBuildGUI {} {
	global Ed Gui Label

	#-------------------------------------------
	# Draw frame
	#-------------------------------------------
	set f $Ed(EdDraw,frame)

	frame $f.fRender  -bg $Gui(activeWorkspace)
	frame $f.fMode    -bg $Gui(activeWorkspace)
	frame $f.fGrid    -bg $Gui(activeWorkspace)
	frame $f.fBtns    -bg $Gui(activeWorkspace)
	frame $f.fApply   -bg $Gui(activeWorkspace)
	pack $f.fRender $f.fMode $f.fGrid $f.fBtns $f.fApply \
		-side top -pady $Gui(pad) -fill x

	EdBuildRenderGUI $Ed(EdDraw,frame).fRender Ed(EdDraw,render)

	#-------------------------------------------
	# Draw->Mode frame
	#-------------------------------------------
	set f $Ed(EdDraw,frame).fMode

	frame $f.fMode -bg $Gui(activeWorkspace)
	set c {label $f.fMode.lMode -text "Mode:" $Gui(WLA)}; eval [subst $c]
	pack $f.fMode.lMode -side left -padx 0 -pady 0
	foreach mode "Draw Select Move" {
		set c {radiobutton $f.fMode.r$mode \
			-text "$mode" -variable Ed(EdDraw,mode) -value $mode \
			-width [expr [string length $mode] +1] \
			-indicatoron 0 \
			 $Gui(WCA)}; eval [subst $c]
		pack $f.fMode.r$mode -side left -padx 0 -pady 0
	}
	pack $f.fMode -side top -pady 5 -padx 0 

	#-------------------------------------------
	# Draw->Grid frame
	#-------------------------------------------
	set f $Ed(EdDraw,frame).fGrid

	# Output label
	set c {button $f.bOutput -text "Output:" \
		-command "ShowLabels EdDrawLabel" $Gui(WBA)}; eval [subst $c]
	set c {entry $f.eOutput -width 6 \
		-textvariable Label(label) $Gui(WEA)}; eval [subst $c]
	bind $f.eOutput <Return>   "EdDrawLabel"
	bind $f.eOutput <FocusOut> "EdDrawLabel"
	set c {entry $f.eName -width 14 \
		-textvariable Label(name) $Gui(WEA) \
		-bg $Gui(activeWorkspace) -state disabled}; eval [subst $c]
	grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
	grid $f.eOutput $f.eName -sticky w

	lappend Label(colorWidgetList) $f.eName

	# Radius
	set c {label $f.lRadius -text "Point Radius:" $Gui(WLA)}
		eval [subst $c]
	set c {entry $f.eRadius -width 6 \
		-textvariable Ed(EdDraw,radius) $Gui(WEA)}; eval [subst $c]
		bind $f.eRadius <Return> "EdDrawUpdate SetRadius; RenderActive"
	grid $f.lRadius $f.eRadius -padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.eRadius -sticky w

	#-------------------------------------------
	# Draw->Btns frame
	#-------------------------------------------
	set f $Ed(EdDraw,frame).fBtns

	set c {button $f.bSelectAll -text "Select All" \
		-command "EdDrawUpdate SelectAll; RenderActive" \
		$Gui(WBA) -width 16}; eval [subst $c]
	set c {button $f.bDeselectAll -text "Deselect All" \
		-command "EdDrawUpdate DeselectAll; RenderActive" \
		$Gui(WBA) -width 16}; eval [subst $c]
	set c {button $f.bDeleteSel -text "Delete Selected" \
		-command "EdDrawUpdate DeleteSelected; RenderActive" \
		$Gui(WBA) -width 16}; eval [subst $c]
	set c {button $f.bDeleteAll -text "Delete All" \
		-command "EdDrawUpdate DeleteAll; RenderActive" \
		$Gui(WBA) -width 16}; eval [subst $c]

	grid $f.bSelectAll $f.bDeselectAll  -padx $Gui(pad) -pady $Gui(pad)
	grid $f.bDeleteSel $f.bDeleteAll    -padx $Gui(pad) -pady $Gui(pad)


	#-------------------------------------------
	# Draw->Apply frame
	#-------------------------------------------
	set f $Ed(EdDraw,frame).fApply

	frame $f.f -bg $Gui(activeWorkspace)
	set c {label $f.f.l -text "Shape:" $Gui(WLA)}; eval [subst $c]
	pack $f.f.l -side left -padx $Gui(pad)

	foreach shape "Polygon Lines Points" {
		set c {radiobutton $f.f.r$shape -width [expr [string length $shape]+1] \
			-text "$shape" -variable Ed(EdDraw,shape) -value $shape \
			-command "EdDrawUpdate SetShape; RenderActive" \
			-indicatoron 0 $Gui(WCA)}
			eval [subst $c]
		pack $f.f.r$shape -side left 
	}

	set c {button $f.bApply -text "Apply" \
		-command "EdDrawApply" $Gui(WBA) -width 8}; eval [subst $c]
	pack $f.f $f.bApply -side top -padx $Gui(pad) -pady 2


}

#-------------------------------------------------------------------------------
# .PROC EdDrawEnter
# .END
#-------------------------------------------------------------------------------
proc EdDrawEnter {} {
	global Ed

	LabelsColorWidgets
}

#-------------------------------------------------------------------------------
# .PROC EdDrawExit
# .END
#-------------------------------------------------------------------------------
proc EdDrawExit {} {

	# Delete points
	EdDrawUpdate DeleteAll
	RenderActive
}

#-------------------------------------------------------------------------------
# .PROC EdDrawLabel
# .END
#-------------------------------------------------------------------------------
proc EdDrawLabel {} {
	global Label

	LabelsFindLabel

	set color [Color($Label(activeID),node) GetDiffuseColor]
	eval Slicer DrawSetColor $color
}

#-------------------------------------------------------------------------------
# .PROC EdDrawUpdate
# .END
#-------------------------------------------------------------------------------
proc EdDrawUpdate {type} {
	global Ed Volume Label Gui

	set e EdDraw
	
	switch $type {
		NextMode {
			switch $Ed($e,mode) {
			"Draw" {
				set Ed($e,mode) Select
			}
			"Select" {
				set Ed($e,mode) Move
			}
			"Move" {
				set Ed($e,mode) Draw
			}
			}
		}
		SelectAll {
			Slicer DrawSelectAll
			set Ed($e,mode) Move
		}
		DeselectAll {
			Slicer DrawDeselectAll
			set Ed($e,mode) Select
		}
		DeleteSelected {
			Slicer DrawDeleteSelected
			set Ed($e,mode) Draw
		}
		DeleteAll {
			Slicer DrawDeleteAll
			set Ed($e,mode) Draw
		}
		SetRadius {
			Slicer DrawSetRadius $Ed($e,radius)
			set Ed($e,radius) [Slicer DrawGetRadius]
		}
		SetShape {
			Slicer DrawSetShapeTo$Ed($e,shape)
			set Ed($e,shape) [Slicer GetShapeString]
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC EdDrawApply
# .END
#-------------------------------------------------------------------------------
proc EdDrawApply {} {
	global Ed Volume Label Gui

	set e EdDraw
	set v [EditorGetInputID $Ed($e,input)]
 
	# Validate input
	if {[ValidateInt $Label(label)] == 0} {
		tk_messageBox -message "Output label is not an integer."
		return
	}
	if {[ValidateInt $Ed($e,radius)] == 0} {
		tk_messageBox -message "Point Radius is not an integer."
		return
	}

	EdSetupBeforeApplyEffect $Ed($e,scope) $v

	# Only draw on native slices
	set outOrder [Ed(editor) GetOutputSliceOrder]
	set inOrder  [Ed(editor) GetInputSliceOrder]
	# Output order is one of IS, LR, PA
	if {$inOrder == "RL"} {set inOrder LR}
	if {$inOrder == "AP"} {set inOrder PA}
	if {$inOrder == "SI"} {set inOrder IS}
	if {$outOrder != $inOrder} {
		if {$inOrder == "LR"} {
			set native SagSlice
		}
		if {$inOrder == "PA"} {
			set native CorSlice
		}
		if {$inOrder == "IS"} {
			set native AxiSlice
		}
		tk_messageBox -message "Please draw on the slice with orient = $native."
		return
	}

	set Gui(progressText) "Draw on [Volume($v,node) GetName]"
	
	set label    $Label(label)
	set radius   $Ed($e,radius)
	set shape    $Ed($e,shape)
	set points   [Slicer DrawGetPoints]
	Ed(editor)   Draw $label $points $radius $shape

	# Dump points
#	set n [$points GetNumberOfPoints]
#	for {set i 0} {$i < $n} {incr i} {
#		puts [$points GetPoint $i]
#	}

	Ed(editor)   SetInput ""
	Ed(editor)   UseInputOff

	EdUpdateAfterApplyEffect $v $Ed($e,render)

	# Reset mode
	EdDrawUpdate DeselectAll
}

