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
# FILE:        EdSaveIsland.tcl
# DATE:        02/22/2000 11:27
# PROCEDURES:  
#   EdSaveIslandInit
#   EdSaveIslandBuildGUI
#   EdSaveIslandApply
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdSaveIslandInit
# .END
#-------------------------------------------------------------------------------
proc EdSaveIslandInit {} {
	global Ed Gui

	set e EdSaveIsland
	set Ed($e,name)      "Save Island"
	set Ed($e,initials)  "SI"
	set Ed($e,desc)      "Save Island: remove all but one island."
	set Ed($e,rank)      7
	set Ed($e,procGUI)   EdSaveIslandBuildGUI

	# Required
	set Ed($e,scope) Single 
	set Ed($e,input) Working

	set Ed($e,xSeed) 0
	set Ed($e,ySeed) 0
	set Ed($e,zSeed) 0
}

#-------------------------------------------------------------------------------
# .PROC EdSaveIslandBuildGUI
# .END
#-------------------------------------------------------------------------------
proc EdSaveIslandBuildGUI {} {
	global Ed Gui

	#-------------------------------------------
	# SaveIsland frame
	#-------------------------------------------
	set f $Ed(EdSaveIsland,frame)

	frame $f.fInput   -bg $Gui(activeWorkspace)
	frame $f.fScope   -bg $Gui(activeWorkspace)
	frame $f.fGrid    -bg $Gui(activeWorkspace)
	frame $f.fApply   -bg $Gui(activeWorkspace)
	pack $f.fGrid $f.fInput $f.fScope $f.fApply \
		-side top -pady $Gui(pad) -fill x

	EdBuildScopeGUI $Ed(EdSaveIsland,frame).fScope Ed(EdSaveIsland,scope) Multi
	EdBuildInputGUI $Ed(EdSaveIsland,frame).fInput Ed(EdSaveIsland,input)

	#-------------------------------------------
	# SaveIsland->Grid frame
	#-------------------------------------------
	set f $Ed(EdSaveIsland,frame).fGrid

	# Seed
	eval {label $f.lSeed -text "Location:"} $Gui(WLA)
	eval {entry $f.eX -width 4 -textvariable Ed(EdSaveIsland,xSeed)} $Gui(WEA)
	eval {entry $f.eY -width 4 -textvariable Ed(EdSaveIsland,ySeed)} $Gui(WEA)
	eval {entry $f.eZ -width 4 -textvariable Ed(EdSaveIsland,zSeed)} $Gui(WEA)
#	grid $f.lSeed $f.eX $f.eY $f.eZ -padx $Gui(pad) -pady $Gui(pad) -sticky e

	#-------------------------------------------
	# SaveIsland->Apply frame
	#-------------------------------------------
	set f $Ed(EdSaveIsland,frame).fApply

#	eval {button $f.bApply -text "Apply" \
#		-command "EdSaveIslandApply"} $Gui(WBA) {-width 8}
#	pack $f.bApply -side top -padx $Gui(pad) -pady 2

    eval {label $f.l -text "Apply by clicking on the island."} $Gui(WLA)
	pack $f.l -side top

}

#-------------------------------------------------------------------------------
# .PROC EdSaveIslandApply
# .END
#-------------------------------------------------------------------------------
proc EdSaveIslandApply {} {
	global Ed Volume Gui

	set e EdSaveIsland
	set v [EditorGetInputID $Ed($e,input)]

	EdSetupBeforeApplyEffect $v $Ed($e,scope) Native

	# Only apply to native slices
	if {[set native [EdIsNativeSlice]] != ""} {
		tk_messageBox -message "Please click on the slice with orient = $native."
		return
	}

	set Gui(progressText) "Save Island in [Volume($v,node) GetName]"
	
	set x       $Ed($e,xSeed)
	set y       $Ed($e,ySeed)
	set z       $Ed($e,zSeed)
	Ed(editor)  SaveIsland $x $y $z
	Ed(editor)  SetInput ""
	Ed(editor)  UseInputOff

	EdUpdateAfterApplyEffect $v
}

