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
# FILE:        MainColors.tcl
# DATE:        01/20/2000 09:40
# LAST EDITOR: gering
# PROCEDURES:  
#   MainColorsInit
#   MainColorsUpdateMRML
#   MainColorsSetActive
#   MainColorsAddLabel
#   MainColorsAddColor
#   MainColorsDeleteLabel
#   MainColorsGetColorFromLabel
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC MainColorsInit
# .END
#-------------------------------------------------------------------------------
proc MainColorsInit {} {
	global Color Gui

	set Color(activeID) ""
	set Color(name) ""
	set Color(label) ""
	set Color(ambient) 0
	set Color(diffuse) 1
	set Color(specular) 0
	set Color(power) 1
	set Color(labels) ""
	set Color(diffuseColor) "1 1 1"
}

#-------------------------------------------------------------------------------
# .PROC MainColorsUpdateMRML
#
#
# .END
#-------------------------------------------------------------------------------
proc MainColorsUpdateMRML {} {
	global Lut Color Mrml

	# Build any new colors
	#--------------------------------------------------------
	# (nothing to be done)

	# Delete any old colors
	#--------------------------------------------------------
	# (nothing to be done)

	# Did we delete the active color?
	if {[lsearch $Color(idList) $Color(activeID)] == -1} {
		MainColorsSetActive [lindex $Color(idList) 0]
	}

	# Refresh GUI 
	#--------------------------------------------------------
	set lut  Lut($Lut(idLabel),lut)
	set iLut Lut($Lut(idLabel),indirectLUT)

	# Set default color to white, and thresholded color to clear black.
	set num [llength $Color(idList)]
	$lut SetNumberOfTableValues [expr $num + 2]
	$lut SetTableValue 0 0.0 0.0 0.0 0.0
	$lut SetTableValue 1 1.0 1.0 1.0 1.0

	# Set colors for each label value
	$iLut InitDirect
	set tree Mrml(colorTree) 
	set node [$tree InitColorTraversal]
	set n 0
	while {$node != ""} {
		set diffuseColor [$node GetDiffuseColor]
		eval $lut SetTableValue [expr $n+2] $diffuseColor 1.0

		set values [$node GetLabels]
		foreach v $values {
			$iLut MapDirect $v [expr $n+2]
		}
		set node [$tree GetNextColor]
		incr n
	}
	$iLut Build
}
 
#-------------------------------------------------------------------------------
# .PROC MainColorsSetActive
# .END
#-------------------------------------------------------------------------------
proc MainColorsSetActive {c} {
	global Color

	set Color(activeID) $c

	if {$c == ""} {return}

	# Update GUI
	set Color(name) [Color($c,node) GetName]
	scan [Color($c,node) GetDiffuseColor] "%g %g %g" \
		Color(red) Color(green) Color(blue)
	foreach param "Ambient Diffuse Specular Power" {
		set Color([Uncap $param]) [Color($c,node) Get$param]
	}
	
}

#-------------------------------------------------------------------------------
# .PROC MainColorsAddLabel
#
# Creates a new label "newLabel" to the color with ID c
# returns 1 on success, else 0
# .END
#-------------------------------------------------------------------------------
proc MainColorsAddLabel {c newLabel} {
	global Color Gui Mrml

	# Convert to integer
	if {$newLabel >= -32768 && $newLabel <= 32767} {
	} else {
		tk_messageBox -icon error -title $Gui(title) \
			-message "Label '$newLabel' must be a short integer."
		return 0
	}

	# Don't allow duplicate labels
	set labels [Color($c,node) GetLabels]
	if {[lsearch $labels $newLabel] != "-1"} {
		tk_messageBox -icon error -title $Gui(title) \
			-message "Label '$newLabel' already exists."
		return 0
	}

	# Append the new label and sort the list of labels
	lappend labels $newLabel
	set labels [lsort -increasing $labels]
	set index  [lsearch $labels $newLabel]

	# Update the node
	Color($c,node) SetLabels $labels

	return 1
}

#-------------------------------------------------------------------------------
# .PROC MainColorsAddColor
#
# Creates a new color named "newColor"
# returns the new color's ID on success, else ""
# .END
#-------------------------------------------------------------------------------
proc MainColorsAddColor {name diffuseColor \
	{ambient ""} {diffuse ""} {specular ""} {power ""}} {
	global Color Mrml Gui

	# Don't allow duplicate colors
	set colors ""
    set tree Mrml(colorTree) 
    set node [$tree InitColorTraversal]
    while {$node != ""} {
		set colors "$colors [$node GetName]"
        set node [$tree GetNextColor]
    }
	if {[lsearch $colors $name] != "-1"} {
		tk_messageBox -icon error -title $Gui(title) \
			-message "Color '$name' already exists."
		return ""
	}

	# Create new node
	set c $Color(nextID)
	incr Color(nextID)
	lappend Color(idList) $c
	vtkMrmlColorNode Color($c,node)
	set n Color($c,node)
	$n SetID           $c
	$n SetDescription  ""
	$n SetName         $name
	eval $n SetDiffuseColor $diffuseColor
	if {$ambient != ""} {
		$n SetAmbient      $ambient
	}
	if {$diffuse != ""} {
		$n SetDiffuse      $diffuse
	}
	if {$specular != ""} {
		$n SetSpecular     $specular
	}
	if {$power != ""} {
		$n SetPower        $power
	}

	Mrml(colorTree) AddItem $n

	return $c
}

#-------------------------------------------------------------------------------
# .PROC MainColorsDeleteLabel
#
# Deletes "delLabel" from Color node "node"
#-------------------------------------------------------------------------------
proc MainColorsDeleteLabel {c delLabel} {
	global Color

	set labels [Color($c,node) GetLabels]

	set i  [lsearch $labels $delLabel]
	set labels [lreplace $labels $i $i]
	Color($c,node) SetLabels $labels
}

#-------------------------------------------------------------------------------
# .PROC MainColorsGetColorFromLabel
#
# Returns the color ID of a label value, or "" if unsuccessful.
# .END
#-------------------------------------------------------------------------------
proc MainColorsGetColorFromLabel {label} {
	global Color Mrml

	set tree Mrml(colorTree) 
    set node [$tree InitColorTraversal]
    while {$node != ""} {
		set labels [$node GetLabels]
		foreach l $labels {
			if {$l == $label} {
				return [$node GetID]
			}
		}
		set node [$tree GetNextColor]
	}
	return ""
}

