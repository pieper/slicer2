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
# FILE:        Colors.tcl
# DATE:        01/20/2000 09:41
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
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC ColorsInit
#
# Initialize global variables.
# .END
#-------------------------------------------------------------------------------
proc ColorsInit {} {
	global Color Gui Module

	# Define Tabs
	set m Colors
	set Module($m,row1List) "Help Colors"
	set Module($m,row1Name) "Help {Edit Colors}"
	set Module($m,row1,tab) Colors

	# Define Procedures
	set Module($m,procGUI)  ColorsBuildGUI
	set Module($m,procMRML) ColorsUpdateMRML
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

	set Color(fColorList) [ScrolledListbox $f.list 1 1 -height 5 -width 15]
	bind $Color(fColorList) <ButtonRelease-1> {ColorsSelectColor}

	set c {entry $f.eName -textvariable Color(name) -width 18 \
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

	set Color(fLabelList) [ScrolledListbox $f.list 1 1 -height 5 -width 6]
		bind $Color(fLabelList) <ButtonRelease-1> "ColorsSelectLabel"

	set c {entry $f.eName -textvariable Color(label) -width 9 \
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

		set c {entry $f.e${slider} -textvariable Color([Uncap $slider]) \
			-width 3 $Gui(WEA)}; eval [subst $c]
			bind $f.e${slider} <Return>   "ColorsSetColor"
			bind $f.e${slider} <FocusOut> "ColorsSetColor"

		set c {scale $f.s${slider} -from 0.0 -to 1.0 -length 40 \
			-variable Color([Uncap $slider]) -command "ColorsSetColor" \
			-resolution 0.1 $Gui(WSA) -sliderlength 15} 
			eval [subst $c]
		set Color(s$slider) $f.s$slider

		grid $f.l${slider} $f.e${slider} $f.s${slider} \
			-pady 1 -padx 1 -sticky e
	}
	$f.sPower config -from 0 -to 100 -resolution 1 


	#-------------------------------------------
	# Colors->Bot->Apply frame
	#-------------------------------------------
	set f $fColors.fBot.fApply

	set c {button $f.bApply -text "Update" -width 7 \
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
	global Color

	MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC ColorsUpdateMRML
#
# This routine is called when the MRML tree changes
# .END
#-------------------------------------------------------------------------------
proc ColorsUpdateMRML {} {
	global Color

	if {$Color(idList) == ""} {
		return
	}
	
	ColorsDisplayColors
	ColorsSelectColor $Color(activeID)
	ColorsSelectLabel 0
}

#-------------------------------------------------------------------------------
# .PROC ColorsDisplayColors
# .END
#-------------------------------------------------------------------------------
proc ColorsDisplayColors {} {
	global Color Mrml

	# Clear old
	$Color(fColorList) delete 0 end

	# Append new
    set tree Mrml(colorTree) 
    set node [$tree InitColorTraversal]
    while {$node != ""} {
		$Color(fColorList) insert end [$node GetName]
        set node [$tree GetNextColor]
	}
}

#-------------------------------------------------------------------------------
# .PROC ColorsSelectColor
# .END
#-------------------------------------------------------------------------------
proc ColorsSelectColor {{i ""}} {
	global Color Mrml

	if {$i == ""} {
		set i [$Color(fColorList) curselection]
	}
	if {$i == ""} {return}
	$Color(fColorList) selection set $i $i
	set c [lindex $Color(idList) $i]

	MainColorsSetActive $c

	ColorsColorSample
	ColorsDisplayLabels
}

#-------------------------------------------------------------------------------
# .PROC ColorsSetColor
# .END
#-------------------------------------------------------------------------------
proc ColorsSetColor {{value ""}} {
	global Color

	# Set the new color from the GUI into the node
	set c $Color(activeID)
	if {$c == ""} {return}

	Color($c,node) SetDiffuseColor $Color(red) $Color(green) $Color(blue)
	foreach param "Ambient Diffuse Specular Power" {
		Color($c,node) Set$param $Color([Uncap $param])
	}

	# Draw Sample
	ColorsColorSample
}

#-------------------------------------------------------------------------------
# .PROC ColorsAddColor
#
# .END
#-------------------------------------------------------------------------------
proc ColorsAddColor {} {
	global Color Gui

	if {[MainColorsAddColor $Color(name) $Color(diffuseColor) \
		$Color(ambient) $Color(diffuse) $Color(specular) $Color(power)] != ""} {
		MainUpdateMRML
	}
}

#-------------------------------------------------------------------------------
# .PROC ColorsDeleteColor
# .END
#-------------------------------------------------------------------------------
proc ColorsDeleteColor {} {
	global Color

	MainMrmlDeleteNode Color $Color(activeID)
	RenderAll
}

#-------------------------------------------------------------------------------
# .PROC ColorsColorSample
# .END
#-------------------------------------------------------------------------------
proc ColorsColorSample {} {
	global Color
	
	foreach slider "Red Green Blue" {
		$Color(s$slider) config -troughcolor \
		[MakeColorNormalized "$Color(red) $Color(green) $Color(blue)"]
	}
}

#-------------------------------------------------------------------------------
# .PROC ColorsDisplayLabels
# .END
#-------------------------------------------------------------------------------
proc ColorsDisplayLabels {} {
	global Color

	# Clear old
	$Color(fLabelList) delete 0 end

	# Append new
	set c $Color(activeID)
	if {$c == ""} {return}

	set Color(label) ""
	foreach label [Color($c,node) GetLabels] {
		$Color(fLabelList) insert end $label
	}
}

#-------------------------------------------------------------------------------
# .PROC ColorsSelectLabel
# .END
#-------------------------------------------------------------------------------
proc ColorsSelectLabel {{i ""}} {
	global Color

	if {$i == ""} {
		set i [$Color(fLabelList) curselection]
	}
	if {$i == ""} {return}
	$Color(fLabelList) selection set $i $i
	
	set c $Color(activeID)
	if {$c == ""} {
		set Color(label) ""
	} else {
		set labels [Color($c,node) GetLabels]
		set Color(label) [lindex $labels $i]
	}
}

#-------------------------------------------------------------------------------
# .PROC ColorsAddLabel
#
# returns 1 on success, else 0
# .END
#-------------------------------------------------------------------------------
proc ColorsAddLabel {} {
	global Color Color Gui

	# Convert to integer
	set index [MainColorsAddLabel $Color(activeID) $Color(label)]

	ColorsDisplayLabels
	ColorsSelectLabel $index
	return 1
}

#-------------------------------------------------------------------------------
# .PROC ColorsDeleteLabel
# .END
#-------------------------------------------------------------------------------
proc ColorsDeleteLabel {} {
	global Color Color

	MainColorsDeleteLabel $Color(activeID) $Color(label)

	ColorsDisplayLabels
	ColorsSelectLabel 0
}
