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
# FILE:        Labels.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   LabelsInit
#   LabelsBuildVTK
#   LabelsBuildGUI
#   LabelsHideLabels
#   LabelsUnHideLabels
#   LabelsColorSample
#   ShowLabels
#   ShowColors
#   LabelsUpdateMRML
#   LabelsDisplayColors
#   LabelsBrowseColor
#   LabelsSelectColor
#   LabelsLeaveGrid
#   LabelsDisplayLabels
#   LabelsSelectLabelClick
#   LabelsSelectLabel
#   LabelsColorWidgets
#   LabelsGetColorFromLabel
#   LabelsFindLabel
#   LabelsSetOutputLabel
#   LabelsCreateColor
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC LabelsInit
# .END
#-------------------------------------------------------------------------------
proc LabelsInit {} {
	global Module Label 

	lappend Module(procVTK)  LabelsBuildVTK
	lappend Module(procGUI)  LabelsBuildGUI
	lappend Module(procMRML) LabelsUpdateMRML

	set Label(node) ""
	set Label(nameBrowse) ""
	set Label(label) ""
	set Label(name) ""
	set Label(diffuse) "1 1 1"
	set Label(dim) 200
	set Label(gridDim) 8
	set Label(gridCellDim) 8
	set Label(nameNew) NewColor
	set Label(labelNew) 1
	set Label(diffuseNew) "1 1 1"
	set Label(callback) ""
	set Label(hideLabels) 0

	set Label(colorWidgetList) ""

	set Label(sample,red)      0.0
	set Label(sample,green)    0.0
	set Label(sample,blue)     0.0
	set Label(sample,ambient)  0.0
	set Label(sample,diffuse)  0.0
	set Label(sample,specular) 0.0
	set Label(sample,power)    1
}

#-------------------------------------------------------------------------------
# .PROC LabelsBuildVTK
# .END
#-------------------------------------------------------------------------------
proc LabelsBuildVTK {} {
	global Label 

	# Color Selection
	set dim1 [expr $Label(dim) - 1]
	vtkImageCanvasSource2D Label(source)
		Label(source) SetScalarType 3
		Label(source) SetNumberOfScalarComponents 3
		Label(source) SetExtent 0 $dim1 0 $dim1 0 0
		Label(source) AllocateScalars
}

#-------------------------------------------------------------------------------
# .PROC LabelsBuildGUI
# .END
#-------------------------------------------------------------------------------
proc LabelsBuildGUI {} {
	global Gui Label

	#-------------------------------------------
	# Labels Popup Window
	#-------------------------------------------
	set w .wLabels
	set Gui(wLabels) $w
	toplevel $w -class Dialog -bg $Gui(inactiveWorkspace)
	wm title $w "Select a Color"
 	wm iconname $w Dialog
 	wm protocol $w WM_DELETE_WINDOW "wm withdraw $w"
 	wm transient $w .
	wm withdraw $w

	# Frames
	frame $w.fTop -bg $Gui(activeWorkspace) -bd 2 -relief raised
	frame $w.fClose -bg $Gui(inactiveWorkspace)
	pack $w.fTop $w.fClose \
		-side top -pady $Gui(pad) -padx $Gui(pad)

	#-------------------------------------------
	# Labels->Close frame
	#-------------------------------------------
	set f $w.fClose

	set c {button $f.bCancel -text "Cancel" -width 6 \
		-command "wm withdraw $w" $Gui(WBA)}
		eval [subst $c]
	pack $f.bCancel -side left -padx $Gui(pad)

	#-------------------------------------------
	# Labels->Top frame
	#-------------------------------------------
	set f $w.fTop

	frame $f.fInfo -bg $Gui(activeWorkspace)
	frame $f.fData -bg $Gui(activeWorkspace)
	frame $f.fNew  -bg $Gui(activeWorkspace)
	pack $f.fInfo $f.fData $f.fNew -side top -pady 5

	#-------------------------------------------
	# Labels->Top->Info frame
	#-------------------------------------------
	set f $w.fTop.fInfo

	set c {label $f.lInfo -justify left  -text\
		"1.) Click on a color.\n2.) Click on a label value." \
		$Gui(WLA)}; eval [subst $c]
	pack $f.lInfo
	set Label(lInfo) $f.lInfo

	#-------------------------------------------
	# Labels->Top->Data frame
	#-------------------------------------------
	set f $w.fTop.fData

	frame $f.fCanvas -bg $Gui(activeWorkspace)
	frame $f.fList   -bg $Gui(activeWorkspace)
	set Label(fList) $f.fList
	pack $f.fCanvas $f.fList -side left -padx $Gui(pad) -pady $Gui(pad)

	#-------------------------------------------
	# Labels->Top->New frame
	#-------------------------------------------
	set f $w.fTop.fNew

	frame $f.fEntry -bg $Gui(activeWorkspace)
	frame $f.fWheel -bg $Gui(activeWorkspace)
	pack $f.fEntry $f.fWheel -side left -padx $Gui(pad) -pady $Gui(pad)

	#-------------------------------------------
	# Labels->Top->New->Entry frame
	#-------------------------------------------
	set f $w.fTop.fNew.fEntry

	set c {label $f.lTitle -text "Create New Color" $Gui(WTA)}; eval [subst $c]
	set c {label $f.lName -text "Color:" $Gui(WLA)}; eval [subst $c]
	set c {entry $f.eName -textvariable Label(nameNew) -width 15 \
		$Gui(WEA)}; eval [subst $c]
	set c {label $f.lLabel -text "Label:" $Gui(WLA)}; eval [subst $c]
	set c {entry $f.eLabel -textvariable Label(nabelNew) -width 15 \
		$Gui(WEA)}; eval [subst $c]
	set c {button $f.bCreate -text Create -width 7 \
		-command LabelsCreateColor $Gui(WBA)}; eval [subst $c]

	grid $f.lTitle -columnspan 2 -padx $Gui(pad) -pady $Gui(pad)
	grid $f.lName $f.eName -padx $Gui(pad) -pady $Gui(pad)
	grid $f.lLabel $f.eLabel -padx $Gui(pad) -pady $Gui(pad)
	grid $f.bCreate -column 1 -padx $Gui(pad) -pady $Gui(pad)
	
	#-------------------------------------------
	# Labels->Top->New->Wheel frame
	#-------------------------------------------
	set f $w.fTop.fNew.fWheel

	foreach slider "Red Green Blue Ambient Diffuse Specular Power" {

		set c {label $f.l${slider} -text "${slider}" $Gui(WLA)}
			eval [subst $c]

		set c {entry $f.e${slider} -textvariable Label(sample,[Uncap $slider]) \
			-width 3 $Gui(WEA)}; eval [subst $c]
			bind $f.e${slider} <Return>   "LabelsColorSample"
			bind $f.e${slider} <FocusOut> "LabelsColorSample"

		set c {scale $f.s${slider} -from 0.0 -to 1.0 -length 40 \
			-variable Label(sample,[Uncap $slider]) -command "LabelsColorSample" \
			-resolution 0.1 $Gui(WSA) -sliderlength 15} 
			eval [subst $c]
		set Label(s$slider) $f.s$slider

		grid $f.l${slider} $f.e${slider} $f.s${slider} \
			-pady 1 -padx 1 -sticky e
	}

	#-------------------------------------------
	# Labels->Canvas frame
	#-------------------------------------------
	set f $w.fTop.fData.fCanvas

	MakeVTKImageWindow color Label(source)

	vtkTkImageWindowWidget $f.fColor -iw colorWin \
		-width $Label(dim) -height $Label(dim) 
		bind $f.fColor <Expose>   {ExposeTkImageViewer %W %x %y %w %h}
		bind $f.fColor <Motion>   {LabelsBrowseColor %x %y}
		bind $f.fColor <Button-1> {LabelsSelectColor %x %y}
		bind $f.fColor <Leave>    {LabelsLeaveGrid}
	set Label(fColorWin) $f.fColor

	frame $f.fName -bg $Gui(activeWorkspace)
	set c {label $f.fName.lName -text "Color:" $Gui(WTA)}; eval [subst $c]
	set c {entry $f.fName.eName -textvariable Label(nameBrowse) -width 20 \
		$Gui(WEA) -state disabled}; eval [subst $c]
	pack $f.fName.lName $f.fName.eName -side left -padx $Gui(pad) -pady 0

	pack $f.fName $f.fColor -side top  -pady $Gui(pad)

	LabelsDisplayColors

	#-------------------------------------------
	# Labels->List frame
	#-------------------------------------------
	set f $w.fTop.fData.fList

	set c {label $f.lTitle -text "Labels" $Gui(WTA)}; eval [subst $c]

	set Label(fLabelList) [ScrolledListbox $f.list 0 1 -height 11 -width 6]
	bind $Label(fLabelList) <ButtonRelease-1> "LabelsSelectLabelClick"

	pack $f.lTitle $f.list -side top -pady 2

}

#-------------------------------------------------------------------------------
# .PROC LabelsHideLabels
# .END
#-------------------------------------------------------------------------------
proc LabelsHideLabels {} {
	global Label

	$Label(lInfo) config -text \
		"Click on a color."

	pack forget $Label(fList)

	set Label(hideLabels) 1
}

#-------------------------------------------------------------------------------
# .PROC LabelsUnHideLabels
# .END
#-------------------------------------------------------------------------------
proc LabelsUnHideLabels {} {
	global Label

	$Label(lInfo) config -text \
		"1.) Click on a color.\n2.) Click on a label value."

	pack $Label(fList) -side left

	set Label(hideLabels) 0
}

#-------------------------------------------------------------------------------
# .PROC LabelsColorSample
# .END
#-------------------------------------------------------------------------------
proc LabelsColorSample {{scale}} {
	global Label
	
	set Label(diffuseNew) \
		"$Label(sample,red) $Label(sample,green) $Label(sample,blue)"

	foreach slider "Red Green Blue" {
		$Label(s$slider) config -troughcolor \
		[MakeColorNormalized \
			"$Label(sample,red) $Label(sample,green) $Label(sample,blue)"]
	}
}


#-------------------------------------------------------------------------------
# .PROC ShowLabels
# .END
#-------------------------------------------------------------------------------
proc ShowLabels {{callback ""} {x 100} {y 100}} {
	global Gui Label

	# Recreate popup if user killed it
	if {[winfo exists $Gui(wLabels)] == 0} {
		LabelsBuildGUI
	}
	
	set Label(callback) $callback

	LabelsUnHideLabels

	ShowPopup $Gui(wLabels) $x $y
}

#-------------------------------------------------------------------------------
# .PROC ShowColors
# .END
#-------------------------------------------------------------------------------
proc ShowColors {{callback ""} {x 100} {y 100}} {
	global Gui Label

	# Recreate popup if user killed it
	if {[winfo exists $Gui(wLabels)] == 0} {
		LabelsBuildGUI
	}
	
	set Label(callback) $callback
	
	LabelsHideLabels

	ShowPopup $Gui(wLabels) $x $y
}

#-------------------------------------------------------------------------------
# .PROC LabelsUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc LabelsUpdateMRML {} {
	global Label

	# Initialize
	if {$Label(label) == ""} {
		set Label(label) 1
		LabelsFindLabel
	}

	LabelsDisplayColors
	LabelsDisplayLabels
}
 
#-------------------------------------------------------------------------------
# .PROC LabelsDisplayColors
# .END
#-------------------------------------------------------------------------------
proc LabelsDisplayColors {} {
	global Color Label Gui

	# Determine size of color grid

	set dag $Color(dag)
	set numColors [MRMLGetNumNodes $dag]
	set p 2
	while {$numColors > [expr $p * $p]} {incr p}
	set n [expr $Label(dim) / $p]
	set Label(gridDim)     $p
	set Label(gridCellDim) $n

	# Make color grid

	set dim1 [expr $Label(dim) - 1]
	eval Label(source) SetDrawColor 0 0 0
	Label(source) FillBox 0 $dim1 0 $dim1

	for {set y 0} {$y < $p} {incr y} {
		for {set x 0} {$x < $p} {incr x} {
			set i [expr $y*$p+$x]
			if {$i > $numColors} {
				set color "0 0 0"
			} else {
				set node  [MRMLGetNode $dag $i]
				set color [MRMLGetValue $node diffuseColor]
				if {[llength $color] != 3} {
					set color "0 0 0"
				}
			}
			foreach c "r g b" i "0 1 2" {
				set $c [expr [lindex $color $i] * $dim1]
			}
			Label(source) SetDrawColor $r $g $b
			Label(source) FillBox [expr $x*$n] [expr $x*$n+$n-1] \
				[expr $dim1-($y*$n)-$n+1] [expr $dim1-($y*$n)]
		}
	}
}

#-------------------------------------------------------------------------------
# LabelsGetColorNodeFromPosition
#
# x,y are positions in color canvas on [1, $Label(dim)]
#-------------------------------------------------------------------------------
proc LabelsGetColorNodeFromPosition {x y} {
	global Label

	set xGrid [expr $x / $Label(gridCellDim)]
	set yGrid [expr $y / $Label(gridCellDim)]
	set i [expr $Label(gridDim) * $yGrid + $xGrid]
	return $i
}

#-------------------------------------------------------------------------------
# .PROC LabelsBrowseColor
#
# Show the name of the color the mouse is over
# .END
#-------------------------------------------------------------------------------
proc LabelsBrowseColor {x y} {
	global Label Color

	set i [LabelsGetColorNodeFromPosition $x $y]
	set node [MRMLGetNode $Color(dag) $i]

	set Label(nameBrowse) [MRMLGetValue $node name]
}

#-------------------------------------------------------------------------------
# .PROC LabelsSelectColor
#
# Select the color the mouse is over
# .END
#-------------------------------------------------------------------------------
proc LabelsSelectColor {x y} {
	global Label Color

	set i [LabelsGetColorNodeFromPosition $x $y]
	set Label(node) [MRMLGetNode $Color(dag) $i]

	# Show the selection
	set Label(nameBrowse) [MRMLGetValue $Label(node) name]
	set Label(diffuse)    [MRMLGetValue $Label(node) diffuseColor]

	LabelsDisplayLabels

	# If there's only one label, or labels or hidden, select it
	if {[llength [MRMLGetValue $Label(node) labels]] == 1 ||
		$Label(hideLabels) == 1} {
		LabelsSelectLabelClick 0
		return
	}
}

#-------------------------------------------------------------------------------
# .PROC LabelsLeaveGrid
# .END
#-------------------------------------------------------------------------------
proc LabelsLeaveGrid {} {
	global Label
	
	if {$Label(node) != ""} {
		set Label(nameBrowse) [MRMLGetValue $Label(node) name]
	} else {
		set Label(nameBrowse) ""
	}
}

#-------------------------------------------------------------------------------
# .PROC LabelsDisplayLabels
# .END
#-------------------------------------------------------------------------------
proc LabelsDisplayLabels {} {
	global Label

	# Clear old
	$Label(fLabelList) delete 0 end

	# Append new
	if {$Label(node) == ""} {return}

	foreach label [MRMLGetValue $Label(node) labels] {
		$Label(fLabelList) insert end $label
	}
}

#-------------------------------------------------------------------------------
# .PROC LabelsSelectLabelClick
# .END
#-------------------------------------------------------------------------------
proc LabelsSelectLabelClick {{index ""}} {
	global Gui Label
	
	LabelsSelectLabel $index

	if {$Label(callback) != ""} {
		$Label(callback)
	}

	# close popup
	wm withdraw $Gui(wLabels)
}

#-------------------------------------------------------------------------------
# .PROC LabelsSelectLabel
# .END
#-------------------------------------------------------------------------------
proc LabelsSelectLabel {{index ""}} {
	global Label

	if {$index == ""} {
		set index [$Label(fLabelList) curselection]
	}
	if {$index == ""} {return}
	$Label(fLabelList) selection set $index $index
	
	set labels [MRMLGetValue $Label(node) labels]
	if {$index >= 0 && $index < [llength $labels]} {
		set Label(label) [lindex $labels $index]
	}

	# Update GUI
	set Label(name)    [MRMLGetValue $Label(node) name]
	set Label(diffuse) [MRMLGetValue $Label(node) diffuseColor]
	LabelsColorWidgets
}

#-------------------------------------------------------------------------------
# .PROC LabelsColorWidgets
# .END
#-------------------------------------------------------------------------------
proc LabelsColorWidgets {} {
	global Label 

	foreach w $Label(colorWidgetList) {
		$w config -bg [MakeColorNormalized $Label(diffuse)]
	}
}

#-------------------------------------------------------------------------------
# .PROC LabelsGetColorFromLabel
#
# Find the color for a label
# .END
#-------------------------------------------------------------------------------
proc LabelsGetColorFromLabel {label} {
	global Label Color

	set num [MRMLGetNumNodes $Color(dag)]
	for {set n 0} {$n < $num} {incr n} {
		set node   [MRMLGetNode $Color(dag) $n]
		set labels [MRMLGetValue $node labels]

		set index 0
		foreach l $labels {
			if {$l == $label} {
				set name [MRMLGetValue $node name]
				return "$n $name $index"
			}
			incr index
		}
	}
	return ""
}

#-------------------------------------------------------------------------------
# .PROC LabelsFindLabel
#
# Find the color for the current label value, and select the color.
# .END
#-------------------------------------------------------------------------------
proc LabelsFindLabel {} {
	global Label

	set result [LabelsGetColorFromLabel $Label(label)]

	# If no color has this label, color the GUI black
	if {[llength $result] != 3} {
		set Label(name)    None
		set Label(diffuse) "0 0 0"
		LabelsColorWidgets
		return
	}
	set n   [lindex $result 0]
	set idx [lindex $result 2]

	# Select color and label
	set y [expr $n / $Label(gridDim)]
	set x [expr $n - $y * $Label(gridDim)]
	set y [expr $y * $Label(gridCellDim)]
	set x [expr $x * $Label(gridCellDim)]
	
	LabelsSelectColor $x $y
	LabelsSelectLabel $idx
}

#-------------------------------------------------------------------------------
# .PROC LabelsSetOutputLabel
#
# Update the label value in the interactive filters.
# .END
#-------------------------------------------------------------------------------
proc LabelsSetOutputLabel {{p ""}} {

	LabelsFindLabel
}

#-------------------------------------------------------------------------------
# .PROC LabelsCreateColor
# .END
#-------------------------------------------------------------------------------
proc LabelsCreateColor {} {
	global Gui Volume Label Dag Color

	set newLabel $Label(labelNew)
    if {$newLabel >= -32768 && $newLabel <= 32767} {
    } else {
        tk_messageBox -icon error -title $Gui(title) \
            -message "Label '$newLabel' must be a short integer."
        return 0
    }

	if {[MainColorsAddColor $Label(nameNew) "$Label(diffuseNew)"] == 0} {
		return
	}
	
	set nodeIndex [expr [MRMLGetNumNodes $Color(dag)] - 1]
	set node      [MRMLGetNode $Color(dag) $nodeIndex]
	if {[MainColorsAddLabel $nodeIndex $Label(labelNew)] == -1} {
		return
	}

	# Update MRML since we're adding a color
	MainUpdateMRML
	
	set Label(node)    $node
	set Label(name)    [MRMLGetValue $Label(node) name]
	set Label(diffuse) [MRMLGetValue $Label(node) diffuseColor]
}

