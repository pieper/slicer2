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
# FILE:        EdIdentifyIslands.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   EdIdentifyIslandsInit
#   EdIdentifyIslandsBuildGUI
#   EdIdentifyIslandsEnter
#   EdIdentifyIslandsNoThreshold
#   EdIdentifyIslandsApply
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC EdIdentifyIslandsInit
# .END
#-------------------------------------------------------------------------------
proc EdIdentifyIslandsInit {} {
	global Ed Gui

	set e EdIdentifyIslands
	set Ed($e,name)      "Identify Islands"
	set Ed($e,desc)      "Assign a unique label to each island."
	set Ed($e,rank)      5
	set Ed($e,procGUI)   EdIdentifyIslandsBuildGUI
	set Ed($e,procEnter) EdIdentifyIslandsEnter

	# Required
	set Ed($e,scope) Single 
	set Ed($e,input) Working

	set Ed($e,fgMin)   $Gui(minShort)
	set Ed($e,fgMax)   $Gui(maxShort)
	set Ed($e,inputLabel) 0
}

#-------------------------------------------------------------------------------
# .PROC EdIdentifyIslandsBuildGUI
# .END
#-------------------------------------------------------------------------------
proc EdIdentifyIslandsBuildGUI {} {
	global Ed Gui Label

	#-------------------------------------------
	# IdentifyIslands frame
	#-------------------------------------------
	set f $Ed(EdIdentifyIslands,frame)

	frame $f.fInput   -bg $Gui(activeWorkspace)
	frame $f.fScope   -bg $Gui(activeWorkspace)
	frame $f.fGrid    -bg $Gui(activeWorkspace)
	frame $f.fApply   -bg $Gui(activeWorkspace)
	pack \
		$f.fInput $f.fScope \
		$f.fGrid $f.fApply \
		-side top -pady $Gui(pad) -fill x

	EdBuildScopeGUI $Ed(EdIdentifyIslands,frame).fScope Ed(EdIdentifyIslands,scope) Multi
	EdBuildInputGUI $Ed(EdIdentifyIslands,frame).fInput Ed(EdIdentifyIslands,input)

	#-------------------------------------------
	# IdentifyIslands->Grid frame
	#-------------------------------------------
	set f $Ed(EdIdentifyIslands,frame).fGrid

	# Label of Sea
	set c {button $f.bOutput -text "Label of the sea:" \
		-command "ShowLabels" $Gui(WBA)}; eval [subst $c]
	set c {entry $f.eOutput -width 6 \
		-textvariable Label(label) $Gui(WEA)}; eval [subst $c]
	bind $f.eOutput <Return>   "LabelsFindLabel"
	bind $f.eOutput <FocusOut> "LabelsFindLabel"
	set c {entry $f.eName -width 14 \
		-textvariable Label(name) $Gui(WEA) \
		-bg $Gui(activeWorkspace) -state disabled}; eval [subst $c]
	grid $f.bOutput $f.eOutput $f.eName -padx 2 -pady $Gui(pad)
	grid $f.eOutput $f.eName -sticky w

	lappend Label(colorWidgetList) $f.eName

	# No Threshold
	set c {button $f.bNo -text "No Threshold" \
		-command "EdIdentifyIslandsNoThreshold" $Gui(WBA)}
		eval [subst $c]

	# Min Threshold
	set c {label $f.lMinFore -text "Min Threshold: " $Gui(WLA)}
		eval [subst $c]
	set c {entry $f.eMinFore -width 6 \
		-textvariable Ed(EdIdentifyIslands,fgMin) $Gui(WEA)}; eval [subst $c]
	grid $f.lMinFore $f.eMinFore $f.bNo -padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.eMinFore $f.bNo -sticky w
	grid $f.bNo -rowspan 2

	# Max Threshold
	set c {label $f.lMaxFore -text "Max Threshold: " $Gui(WLA)}
		eval [subst $c]
	set c {entry $f.eMaxFore -width 6 \
		-textvariable Ed(EdIdentifyIslands,fgMax) $Gui(WEA)}; eval [subst $c]
	grid $f.lMaxFore $f.eMaxFore -padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.eMaxFore -sticky w

	#-------------------------------------------
	# IdentifyIslands->Apply frame
	#-------------------------------------------
	set f $Ed(EdIdentifyIslands,frame).fApply

#	set c {button $f.bApply -text "Apply" \
#		-command "EdIdentifyIslandsApply" $Gui(WBA) -width 8}; eval [subst $c]
#	pack $f.bApply -side top -padx $Gui(pad) -pady 2

    set c {label $f.lApply1 -text "Apply by clicking on the" \
		$Gui(WLA)}; eval [subst $c]
    set c {label $f.lApply2 -text "'sea' which contains the 'islands'." \
		$Gui(WLA)}; eval [subst $c]
	pack $f.lApply1 $f.lApply2 -side top -pady 0 -padx 0

}

#-------------------------------------------------------------------------------
# .PROC EdIdentifyIslandsEnter
# .END
#-------------------------------------------------------------------------------
proc EdIdentifyIslandsEnter {} {
	global Ed

	# Color the label value 
	LabelsColorWidgets
}

#-------------------------------------------------------------------------------
# .PROC EdIdentifyIslandsNoThreshold
# .END
#-------------------------------------------------------------------------------
proc EdIdentifyIslandsNoThreshold {} {
	global Ed Gui
	
	set Ed(EdIdentifyIslands,fgMin)  $Gui(minShort)
	set Ed(EdIdentifyIslands,fgMax)  $Gui(maxShort)
}

#-------------------------------------------------------------------------------
# .PROC EdIdentifyIslandsApply
# .END
#-------------------------------------------------------------------------------
proc EdIdentifyIslandsApply {} {
	global Ed Volume Label Gui

	set e EdIdentifyIslands
	set v [EditorGetInputID $Ed($e,input)]

	# Validate input
	if {[ValidateInt $Label(label)] == 0} {
		tk_messageBox -message "Label Of The Sea is not an integer."
		return
	}
	if {[ValidateInt $Ed($e,fgMin)] == 0} {
		tk_messageBox -message "Min Threshold is not an integer."
		return
	}
	if {[ValidateInt $Ed($e,fgMax)] == 0} {
		tk_messageBox -message "Max Threshold is not an integer."
		return
	}

	EdSetupBeforeApplyEffect $v $Ed($e,scope) Native

	set Gui(progressText) "IdentifyIslands in [Volume($v,node) GetName]"
	
	# Determine the input label
	set Label(label) $Ed($e,inputLabel)
	LabelsFindLabel
	
	set bg        $Label(label)
	set fgMin     $Ed($e,fgMin)
	set fgMax     $Ed($e,fgMax)
	Ed(editor)    IdentifyIslands $bg $fgMin $fgMax
	Ed(editor)    SetInput ""
	Ed(editor)    UseInputOff

	EdUpdateAfterApplyEffect $v
}

