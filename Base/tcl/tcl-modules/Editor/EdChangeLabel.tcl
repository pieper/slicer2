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
# FILE:        EdChangeLabel.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   EdChangeLabelInit
#   EdChangeLabelBuildGUI
#   EdChangeLabelEnter
#   EdChangeLabelApply
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC EdChangeLabelInit
# .END
#-------------------------------------------------------------------------------
proc EdChangeLabelInit {} {
	global Ed

	set e EdChangeLabel
	set Ed($e,name)      "Change Label"
	set Ed($e,desc)      "Change all pixels of a certain label."
	set Ed($e,rank)      3
	set Ed($e,procGUI)   EdChangeLabelBuildGUI
	set Ed($e,procEnter) EdChangeLabelEnter

	# Required
	set Ed($e,scope) 3D 
	set Ed($e,input) Working

	set Ed($e,inputLabel) 0
}

#-------------------------------------------------------------------------------
# .PROC EdChangeLabelBuildGUI
# .END
#-------------------------------------------------------------------------------
proc EdChangeLabelBuildGUI {} {
	global Ed Gui Label

	#-------------------------------------------
	# ChangeLabel frame
	#-------------------------------------------
	set f $Ed(EdChangeLabel,frame)

	frame $f.fInput   -bg $Gui(activeWorkspace)
	frame $f.fScope   -bg $Gui(activeWorkspace)
	frame $f.fGrid    -bg $Gui(activeWorkspace)
	frame $f.fApply   -bg $Gui(activeWorkspace)
	pack \
		$f.fInput $f.fScope \
		$f.fGrid $f.fApply \
		-side top -pady $Gui(pad) -fill x

	EdBuildScopeGUI $Ed(EdChangeLabel,frame).fScope Ed(EdChangeLabel,scope) 
	EdBuildInputGUI $Ed(EdChangeLabel,frame).fInput Ed(EdChangeLabel,input)

	#-------------------------------------------
	# ChangeLabel->Grid frame
	#-------------------------------------------
	set f $Ed(EdChangeLabel,frame).fGrid

	# Input label
	set Ed(fOpChangeLabelGrid) $f
	set c {label $f.lInput -text "Value to change:" $Gui(WLA)}; eval [subst $c]
	set c {entry $f.eInput -width 6 \
		-textvariable Ed(EdChangeLabel,inputLabel) $Gui(WEA)}; eval [subst $c]

	# Output label
	set c {button $f.bOutput -text "Output:" \
		-command "ShowLabels" $Gui(WBA)}; eval [subst $c]
	set c {entry $f.eOutput -width 6 \
		-textvariable Label(label) $Gui(WEA)}; eval [subst $c]
	bind $f.eOutput <Return>   "LabelsFindLabel"
	bind $f.eOutput <FocusOut> "LabelsFindLabel"
	set c {entry $f.eName -width 14 \
		-textvariable Label(name) $Gui(WEA) \
		-bg $Gui(activeWorkspace) -state disabled}; eval [subst $c]

	lappend Label(colorWidgetList) $f.eName

	grid $f.lInput $f.eInput -padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.bOutput $f.eOutput $f.eName -padx $Gui(pad) -pady $Gui(pad) -sticky e

	#-------------------------------------------
	# ChangeLabel->Apply frame
	#-------------------------------------------
	set f $Ed(EdChangeLabel,frame).fApply

	set c {button $f.bApply -text "Apply" \
		-command "EdChangeLabelApply" $Gui(WBA) -width 8}
		eval [subst $c]
    set c {label $f.lApply -text "Also apply by clicking on a label." \
		$Gui(WLA)}; eval [subst $c]
	pack $f.bApply $f.lApply -side top -padx $Gui(pad) -pady 2

}

#-------------------------------------------------------------------------------
# .PROC EdChangeLabelEnter
# .END
#-------------------------------------------------------------------------------
proc EdChangeLabelEnter {} {
	global Ed

	LabelsColorWidgets
}

#-------------------------------------------------------------------------------
# .PROC EdChangeLabelApply
# .END
#-------------------------------------------------------------------------------
proc EdChangeLabelApply {} {
	global Ed Volume Label Gui

	set e EdChangeLabel
	set v [EditorGetInputID $Ed($e,input)]

	EdSetupBeforeApplyEffect $Ed($e,scope) $v

	set Gui(progressText) "Change Label in [Volume($v,node) GetName]"
	
	set fg       $Ed($e,inputLabel)
	set fgNew    $Label(label)
	Ed(editor)   ChangeLabel $fg $fgNew
	Ed(editor)   SetInput ""
	Ed(editor)   UseInputOff

	EdUpdateAfterApplyEffect $v
}

