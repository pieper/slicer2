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
# FILE:        Select.tcl
# Contains procedures that support the selection of geometry in Slicer
# PROCEDURES:  
#   SelectInit
#   SelectBuildVTK
#   SelectEnter
#   SelectExit
#   SelectBuildGUI
#   SelectUpdateGUI
# SelectInit
# SelectBuildVTK
# SelectPick
# SelectModelGUI
# SelectModel
# SelectPickModel
# SelectGetPick
# SelectModelMenu

#-------------------------------------------------------------------------------
# .PROC SelectInit
# Initialize global Select variables
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SelectInit {} {
	global Selected Module

	set m Select
	# set Module($m,procVTK) SelectBuildVTK
	# set Module($m,procGUI) SelectBuildGUI

	lappend Module(procGUI) SelectBuildGUI
	lappend Module(procVTK) SelectBuildVTK
	set Selected(Model) ""
	}

#-------------------------------------------------------------------------------
# .PROC SelectBuildVTK
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SelectBuildVTK {} {
	global Select

	vtkFastCellPicker Select(picker)
#	vtkCellPicker Select(picker)
		Select(picker) SetTolerance 0.001
		Select(picker) PickFromListOff
	}

#-------------------------------------------------------------------------------
# .PROC SelectBuildGUI
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SelectBuildGUI {} {
	global SelectEventMgr Gui

	array set SelectEventMgr [subst { \
		$Gui(fViewWin),<Enter> {focus %W} \
		$Gui(fViewWin),<Control-1> {addGlyphPoint %W %x %y} \
		$Gui(fViewWin),<KeyPress-p> {addGlyphPoint %W %x %y} \
		$Gui(fViewWin),<KeyPress-c> {ExtractComponent %W %x %y} \
		$Gui(fSl0Win),<KeyPress-p> {addGlyphPoint2D %W 0 %x %y} \
		$Gui(fSl1Win),<KeyPress-p> {addGlyphPoint2D %W 1 %x %y} \
		$Gui(fSl2Win),<KeyPress-p> {addGlyphPoint2D %W 2 %x %y} \
		$Gui(fViewWin),<Shift-Control-1> {selGlyphPoint %W %x %y} \
		$Gui(fViewWin),<Control-2> {selGlyphPoint %W %x %y} \
		$Gui(fViewWin),<KeyPress-q> {selGlyphPoint %W %x %y} \
		$Gui(fViewWin),<Control-3> {delGlyphPoint %W %x %y} \
		$Gui(fViewWin),<KeyPress-d> {delGlyphPoint %W %x %y} \
		$Gui(fViewWin),<Control-B1-Motion> {set noop 0} \
		$Gui(fViewWin),<Control-B2-Motion> {set noop 0} \
		$Gui(fViewWin),<Control-B3-Motion> {set noop 0} } ]

	set SelectEventMgr1 ""
	lappend SelectEventMgr1 {$Gui(fViewWin) <KeyPress-x> \
		{ if { [SelectPick Select(picker) %W %x %y] != 0 } \
			  { eval MainSlicesAllOffsetToPoint $Select(xyz) } } }
	lappend SelectEventMgr1 {$Gui(fSl0Win) <KeyPress-x> \
		{ if { [SelectPick2D %W %x %y] != 0 } \
			  { eval MainSlicesAllOffsetToPoint $Select(xyz) } } }
	lappend SelectEventMgr1 {$Gui(fSl1Win) <KeyPress-x> \
		{ if { [SelectPick2D %W %x %y] != 0 } \
			  { eval MainSlicesAllOffsetToPoint $Select(xyz) } } }
	lappend SelectEventMgr1 {$Gui(fSl2Win) <KeyPress-x> \
		{ if { [SelectPick2D %W %x %y] != 0 } \
			  { eval MainSlicesAllOffsetToPoint $Select(xyz) } } }
	# puts $SelectEventMgr1
	pushEventManager $SelectEventMgr1
	}

#-------------------------------------------------------------------------------
# .PROC SelectRefreshVTK
#
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SelectRefreshGUI {} {
	}

#-------------------------------------------------------------------------------
# .PROC SelectPick
# Invoke the picker for a given widget, location, and renderer
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SelectPick { picker widget x y } {
	global Select Fiducials

	# Note: y coordinate must be transformed to account for
	# origin in upper left.
	set y1 [expr [lindex [$widget configure -height] 4] - $y - 1]
	set renderer [SelectPickRenderer $widget $x $y1]
	if { $renderer == "" } {
		return 0
	} elseif { [$picker Pick $x $y1 0 $renderer] == 0 || \
				[$picker GetCellId] < 0 } {
		return 0
	} else {
		set Select(actor) [$picker GetActor]
		set Select(xyz) [$picker GetPickPosition]
		set Select(cellId) [$picker GetCellId]
		#
		# This part handles the fact that picking a point
		# should return the point XYZ, not the picked XYZ.
		#
		foreach fid $Fiducials(idList) {
		  if { $Select(actor) == "Fiducials($fid,actor)" } {
		    set pid [FiducialsPointIdFromGlyphCellId $fid $Select(cellId)]
		    set Select(xyz) [FiducialsWorldPointXYZ $fid $pid]
		    }
		}
		return 1
	}
}

proc SelectPickRenderer { widget x y } {
	set rWin [$widget GetRenderWindow]
	set winWidth [lindex [$rWin GetSize] 0]
	set winHeight [lindex [$rWin GetSize] 1]
	set rList [$rWin GetRenderers]
	set retRen ""
	$rList InitTraversal
	for { set thisR [$rList GetNextItem] } { $thisR != "" } \
		{ set thisR [$rList GetNextItem] } {
		set vPort [$thisR GetViewport]
		set minX [expr [lindex $vPort 0] * $winWidth]
		set maxX [expr [lindex $vPort 2] * $winWidth]
		set minY [expr [lindex $vPort 1] * $winHeight]
		set maxY [expr [lindex $vPort 3] * $winHeight]
		if { $x>=$minX && $x<=$maxX && $y>=$minY && $y<=$maxY } {
			set retRen $thisR
			}
		}
	return $retRen
	}

proc SelectPickable { group value } {
	# group is one of: Anno, Models, Slices, Points
}

proc SelectPick2D { widget x y } {
	global Select Interactor

	set s $Interactor(s)
	if { $s != "" } {
		scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs yz x y
		Slicer SetReformatPoint $s $x $y
		scan [Slicer GetWldPoint] "%g %g %g" xRas yRas zRas
		set Select(xyz) "$xRas $yRas $zRas"
		return 1
	} else {
		return 0
		}
	}

#-------------------------------------------------------------------------------
# .PROC SelectModelOn
# Set up handlers and pickability for Model selection
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SelectModelOn { picker widget x y renderer } {
	global Select
}
