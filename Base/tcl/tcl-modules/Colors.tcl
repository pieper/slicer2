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
# FILE:        Colors.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   ColorsInit
#   ColorsBuildGUI
#   ColorsApply
#   ColorsUpdateMRML
#   ColorsDisplayColors
#   ColorsSelectColor
#   ColorsSetColor
#   ColorsAddColor
#   ColorsDeleteColor
#   ColorsColorSample
#   ColorsDisplayLabels
#   ColorsSelectLabel
#   ColorsAddLabel
#   ColorsDeleteLabel
#   ColorsFindLabel
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC ColorsInit
#
# Initialize global variables.
# .END
#-------------------------------------------------------------------------------
proc ColorsInit {} {
	global Colors Gui Module

	# Define Tabs
	set m Colors
	set Module($m,row1List) "Help Colors"
	set Module($m,row1Name) "Help {Edit Colors}"
	set Module($m,row1,tab) Colors

	# Define Procedures
	set Module($m,procGUI)  ColorsBuildGUI
	set Module($m,procMRML) ColorsUpdateMRML

	set Colors(node) ""
	set Colors(nodeIndex) ""
	set Colors(label) 0
}

#-------------------------------------------------------------------------------
# .PROC ColorsBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc ColorsBuildGUI {} {
	global Gui Module Color Colors

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Colors
	#   Top
	#     Colors
	#     Labels
	#   Bot
	#     Attr
	#     Apply
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Models are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Colors $help
	MainHelpBuildGUI Colors

	#-------------------------------------------
	# Colors frame
	#-------------------------------------------
	set fColors $Module(Colors,fColors)
	set f $fColors

	frame $f.fTop -bg $Gui(activeWorkspace)
	frame $f.fBot -bg $Gui(activeWorkspace)
	pack $f.fTop $f.fBot -side top -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Colors->Top
	#-------------------------------------------
	set f $fColors.fTop

	frame $f.fColors -bg $Gui(activeWorkspace) -relief groove -bd 2
	frame $f.fLabels -bg $Gui(activeWorkspace) -relief groove -bd 2
	pack $f.fColors $f.fLabels -side left -padx 2 -pady 1 -fill x

	#-------------------------------------------
	# Colors->Top->Colors frame
	#-------------------------------------------
	set f $fColors.fTop.fColors

	set c {label $f.lTitle -text "Color Name" $Gui(WTA)}; eval [subst $c]

	set Colors(fColorList) [ScrolledListbox $f.list 1 1 -height 5 -width 15]
		bind $Colors(fColorList) <ButtonRelease-1> {ColorsSelectColor}

	set c {entry $f.eName -textvariable Colors(colorName) -width 18 \
		$Gui(WEA)}; eval [subst $c]
	bind $f.eName <Return> "ColorsAddColor"
	
	frame $f.fBtns -bg $Gui(activeWorkspace)
	set c {button $f.fBtns.bAdd -text "Add" -width 4 \
		-command "ColorsAddColor" $Gui(WBA)}; eval [subst $c]
	set c {button $f.fBtns.bDelete -text "Delete" -width 7 \
		-command "ColorsDeleteColor" $Gui(WBA)}; eval [subst $c]
	pack $f.fBtns.bAdd $f.fBtns.bDelete -side left -padx $Gui(pad)

	pack $f.lTitle -side top -pady 2
	pack $f.list $f.eName $f.fBtns -side top -pady $Gui(pad)

	#-------------------------------------------
	# Colors->Top->Labels frame
	#-------------------------------------------
	set f $fColors.fTop.fLabels

	set c {label $f.lTitle -text "Label" $Gui(WTA)}; eval [subst $c]

	set Colors(fLabelList) [ScrolledListbox $f.list 1 1 -height 5 -width 6]
		bind $Colors(fLabelList) <ButtonRelease-1> "ColorsSelectLabel"

	set c {entry $f.eName -textvariable Colors(labelName) -width 9 \
		$Gui(WEA)}; eval [subst $c]
	bind $f.eName <Return> "ColorsAddLabel"

	frame $f.fBtns -bg $Gui(activeWorkspace)
	set c {button $f.fBtns.bAdd -text "Add" -width 4 \
		-command "ColorsAddLabel" $Gui(WBA)}; eval [subst $c]
	set c {button $f.fBtns.bDelete -text "Del" -width 4 \
		-command "ColorsDeleteLabel" $Gui(WBA)}; eval [subst $c]
	pack $f.fBtns.bAdd $f.fBtns.bDelete -side left -padx $Gui(pad)

	pack $f.lTitle -side top -pady 2
	pack $f.list $f.eName $f.fBtns -side top -pady $Gui(pad) 

	#-------------------------------------------
	# Colors->Bot frame
	#-------------------------------------------
	set f $fColors.fBot

	frame $f.fAttr  -bg $Gui(activeWorkspace)
	frame $f.fApply -bg $Gui(activeWorkspace)
	pack $f.fAttr $f.fApply -side left -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Colors->Bot->Attr frame
	#-------------------------------------------
	set f $fColors.fBot.fAttr

	foreach slider "Red Green Blue Ambient Diffuse Specular Power" {

		set c {label $f.l${slider} -text "${slider}" $Gui(WLA)}
			eval [subst $c]

		set c {entry $f.e${slider} -textvariable Colors([Uncap $slider]) \
			-width 3 $Gui(WEA)}; eval [subst $c]
			bind $f.e${slider} <Return>   "ColorsSetColor"
			bind $f.e${slider} <FocusOut> "ColorsSetColor"

		set c {scale $f.s${slider} -from 0.0 -to 1.0 -length 40 \
			-variable Colors([Uncap $slider]) -command "ColorsSetColor" \
			-resolution 0.1 $Gui(WSA) -sliderlength 15} 
			eval [subst $c]
		set Colors(s$slider) $f.s$slider

		grid $f.l${slider} $f.e${slider} $f.s${slider} \
			-pady 1 -padx 1 -sticky e
	}
	$f.sPower config -from 0 -to 100 -resolution 1 


	#-------------------------------------------
	# Colors->Bot->Apply frame
	#-------------------------------------------
	set f $fColors.fBot.fApply

	set c {button $f.bApply -text "Apply" -width 6 \
		-command "ColorsApply; RenderAll" $Gui(WBA)}; eval [subst $c]
	pack $f.bApply -side top -pady $Gui(pad) 
}

#-------------------------------------------------------------------------------
# .PROC ColorsApply
#
# Update all uses of colors in the Slicer to show the current colors
# .END
#-------------------------------------------------------------------------------
proc ColorsApply {} {
	global Colors Color

	MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC ColorsUpdateMRML
#
# This routine is called when the MRML tree changes
# .END
#-------------------------------------------------------------------------------
proc ColorsUpdateMRML {} {
	ColorsDisplayColors
	ColorsSelectColor 0
	ColorsSelectLabel 0
}

#-------------------------------------------------------------------------------
# .PROC ColorsDisplayColors
# .END
#-------------------------------------------------------------------------------
proc ColorsDisplayColors {} {
	global Color Colors

	# Clear old
	$Colors(fColorList) delete 0 end

	# Append new
	set num [MRMLGetNumNodes $Color(dag)]
	for {set n 0} {$n < $num} {incr n} {
		set Colors(node) [MRMLGetNode $Color(dag) $n]
		$Colors(fColorList) insert end [MRMLGetValue $Colors(node) name]
	}
}

#-------------------------------------------------------------------------------
# .PROC ColorsSelectColor
# .END
#-------------------------------------------------------------------------------
proc ColorsSelectColor {{index ""}} {
	global Color Colors

	if {$index == ""} {
		set index [$Colors(fColorList) curselection]
	}
	if {$index == ""} {return}
	$Colors(fColorList) selection set $index $index

	set Colors(nodeIndex) $index
	set Colors(node) [MRMLGetNode $Color(dag) $Colors(nodeIndex)]
	if {$Colors(node) == ""} {return}

	# Show the selection below listbox
	set Colors(colorName) [MRMLGetValue $Colors(node) name]

	# Show the color
	set color [MRMLGetValue $Colors(node) diffuseColor]
	set Colors(red)   [lindex $color 0]
	set Colors(green) [lindex $color 1]
	set Colors(blue)  [lindex $color 2]
	foreach param "ambient diffuse specular power" {
		set Colors($param) [MRMLGetValue $Colors(node) $param]
	}
	
	ColorsColorSample

	ColorsDisplayLabels
}

#-------------------------------------------------------------------------------
# .PROC ColorsSetColor
# .END
#-------------------------------------------------------------------------------
proc ColorsSetColor {{value ""}} {
	global Colors Color

	# Set the new color in the node and update the dag
	set color "$Colors(red) $Colors(green) $Colors(blue)"
	set Colors(node) [MRMLSetValue $Colors(node) diffuseColor $color]
	foreach param "ambient diffuse specular power" {
		set Colors(node) [MRMLSetValue $Colors(node) $param $Colors($param)]
	}
	set Color(dag) [MRMLSetNode $Color(dag) $Colors(nodeIndex) $Colors(node)]

	# Draw Sample
	ColorsColorSample
}

#-------------------------------------------------------------------------------
# .PROC ColorsAddColor
#
# returns 1 on success, else 0
# .END
#-------------------------------------------------------------------------------
proc ColorsAddColor {{diffuseColor ""}} {
	global Colors Color Gui

	MainColorsAddColor $Colors(colorName)

	# Select this node
	set Colors(nodeIndex) [expr [MRMLGetNumNodes $Color(dag)] - 1]

	ColorsDisplayColors
	ColorsSelectColor $Colors(nodeIndex)
	return 1
}

#-------------------------------------------------------------------------------
# .PROC ColorsDeleteColor
# .END
#-------------------------------------------------------------------------------
proc ColorsDeleteColor {} {
	global Color Colors

	MainColorsDeleteColor $Colors(nodeIndex)
	
	ColorsDisplayColors
	ColorsSelectColor 0
}

#-------------------------------------------------------------------------------
# .PROC ColorsColorSample
# .END
#-------------------------------------------------------------------------------
proc ColorsColorSample {} {
	global Colors
	
	foreach slider "Red Green Blue" {
		$Colors(s$slider) config -troughcolor \
		[MakeColorNormalized "$Colors(red) $Colors(green) $Colors(blue)"]
	}
}

#-------------------------------------------------------------------------------
# .PROC ColorsDisplayLabels
# .END
#-------------------------------------------------------------------------------
proc ColorsDisplayLabels {} {
	global Colors

	# Clear old
	$Colors(fLabelList) delete 0 end

	# Append new
	if {$Colors(node) == ""} {return}
	set Colors(labelName) ""
	foreach label [MRMLGetValue $Colors(node) labels] {
		$Colors(fLabelList) insert end $label
	}
}

#-------------------------------------------------------------------------------
# .PROC ColorsSelectLabel
# .END
#-------------------------------------------------------------------------------
proc ColorsSelectLabel {{index ""}} {
	global Colors

	if {$index == ""} {
		set index [$Colors(fLabelList) curselection]
	}
	if {$index == ""} {return}
	$Colors(fLabelList) selection set $index $index
	
	set labels [MRMLGetValue $Colors(node) labels]
	if {$index >= 0 && $index < [llength $labels]} {
		set Colors(label) [lindex $labels $index]
	}

	# Show the selection below listbox
	set Colors(labelName) $Colors(label)
}

#-------------------------------------------------------------------------------
# .PROC ColorsAddLabel
#
# returns 1 on success, else 0
# .END
#-------------------------------------------------------------------------------
proc ColorsAddLabel {} {
	global Colors Color Gui

	# Convert to integer
	set index [MainColorsAddLabel $Colors(nodeIndex) $Colors(labelName)]

	ColorsDisplayLabels
	ColorsSelectLabel $index
	return 1
}

#-------------------------------------------------------------------------------
# .PROC ColorsDeleteLabel
# .END
#-------------------------------------------------------------------------------
proc ColorsDeleteLabel {} {
	global Colors Color

	MainColorsDeleteLabel $Colors(nodeIndex) $Colors(labelName)

	ColorsDisplayLabels
	ColorsSelectLabel 0
}

#-------------------------------------------------------------------------------
# .PROC ColorsFindLabel
#
# Find the color for a label
# .END
#-------------------------------------------------------------------------------
proc ColorsFindLabel {} {
	global Colors

	set result [ColorsGetColorFromLabel $Colors(label)]
	if {[llength $result] != 3} {return}
	set n [lindex $result 0]
	set index [lindex $result 2]

	# Select color and label
	ColorsSelectColor $n
	ColorsSelectLabel $index
}

