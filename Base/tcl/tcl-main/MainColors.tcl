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
# FILE:        MainColors.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   MainColorsInit
#   MainColorsUpdateMRML
#   MainColorsDelete
#   MainColorsAddLabel
#   MainColorsAddColor
#   MainColorsDeleteLabel
#   MainColorsMakeColors
#   MainColorsBuildDag
#   MainColorsGetColorFromLabel
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC MainColorsInit
# .END
#-------------------------------------------------------------------------------
proc MainColorsInit {} {
	global Color Gui

	set Color(dag) ""
}

#-------------------------------------------------------------------------------
# .PROC MainColorsUpdateMRML
#
# Create any portions of the GUI that depend on the data.  For example, a
# menu widget that lists all the volumes would be created here.  That way,
# it could be refreshed by calling this routine whenever new volumes are
# loaded into the slicer.
# .END
#-------------------------------------------------------------------------------
proc MainColorsUpdateMRML {} {
	global Dag Color

	# Build a dag of just colors for the editor to use
	MainColorsBuildDag $Dag(withColors)

	MainColorsMakeColors
}

#-------------------------------------------------------------------------------
# .PROC MainColorsDelete
#
# Clear Color(dag) and delete all tcl variables related to colors
# .END
#-------------------------------------------------------------------------------
proc MainColorsDelete {} {
	global Color

	set idList $Color(idList)
	foreach c $Color(idList) {
		if {$c >= 0} {
			# Delete TCL variables
			foreach key [MRMLGetDefault Color keyList] {
				unset Color($c,$key)
			}
			unset Color($c,id)

			# Delete ID
			set i [lsearch $idList $c]
			set idList [lreplace $idList $i $i]
		}
	}

	set Color(dag) [MRMLClearDag $Color(dag)]

	set Color(idList) $idList
	set Color(nextID) 0
}

#-------------------------------------------------------------------------------
# .PROC MainColorsAddLabel
#
# Creates a new label "newLabel" to the node at index "nodeIndex"
# returns index of newLabel into node's list of labels on success, else -1
# .END
#-------------------------------------------------------------------------------
proc MainColorsAddLabel {nodeIndex newLabel} {
	global Color Gui

	# Convert to integer
	if {$newLabel >= -32768 && $newLabel <= 32767} {
	} else {
		tk_messageBox -icon error -title $Gui(title) \
			-message "Label '$newLabel' must be a short integer."
		return -1
	}

	# Don't allow duplicate labels
	set node [MRMLGetNode $Color(dag) $nodeIndex]
	set labels [MRMLGetValue $node labels]
	if {[lsearch $labels $newLabel] != "-1"} {
		tk_messageBox -icon error -title $Gui(title) \
			-message "Label '$newLabel' already exists."
		return -1
	}

	# Append the new label and sort the list of labels
	lappend labels $newLabel
	set labels [lsort -increasing $labels]
	set index [lsearch $labels $newLabel]

	# Update the node
	set node [MRMLSetValue $node labels $labels]

	# Update the dag
	set Color(dag) [MRMLSetNode $Color(dag) $nodeIndex $node]

	return $index
}

#-------------------------------------------------------------------------------
# .PROC MainColorsAddColor
#
# Creates a new color named "newColor"
# returns 1 on success, else 0
# .END
#-------------------------------------------------------------------------------
proc MainColorsAddColor {newColor {diffuseColor ""}} {
	global Color Gui

	# Don't allow duplicate colors
	set colors ""
	set num [MRMLGetNumNodes $Color(dag)]
	for {set n 0} {$n < $num} {incr n} {
		set node [MRMLGetNode $Color(dag) $n]
		set type [MRMLGetNodeType $node]
		if {$type == "Color"} {
			set name [MRMLGetValue $node name]
			set colors "$colors $name"
		}
	}
	if {[lsearch $colors $newColor] != "-1"} {
		tk_messageBox -icon error -title $Gui(title) \
			-message "Color '$newColor' already exists."
		return 0
	}

	# Create new node initialized to defaults
	set Color(node) [MRMLCreateNode "Color"]
	
	# Change name
	set Color(node) [MRMLSetValue $Color(node) name $newColor]

	# Diffuse
	if {$diffuseColor != ""} {
		set Color(node) [MRMLSetValue $Color(node) diffuseColor $diffuseColor]
	}

	# Update the dag
	set Color(dag) [MRMLAppendNode $Color(dag) $Color(node)]

	return 1
}

#-------------------------------------------------------------------------------
# MainColorsDeleteColor
#
# Delete dag at index "nodeIndex" in Color(dag)
#-------------------------------------------------------------------------------
proc MainColorsDeleteColor {nodeIndex} {
	global Color Gui

	set Color(dag) [MRMLDeleteNode $Color(dag) $nodeIndex]
}

#-------------------------------------------------------------------------------
# .PROC MainColorsDeleteLabel
#
# Deletes "delLabel" from node at index "nodeIndex"
#-------------------------------------------------------------------------------
proc MainColorsDeleteLabel {nodeIndex delLabel} {
	global Color

	# Delete the label
	set node   [MRMLGetNode $Color(dag) $nodeIndex]
	set labels [MRMLGetValue $node labels]
	set index  [lsearch $labels $delLabel]
	set labels [lreplace $labels $index $index]

	# Update the node
	set node [MRMLSetValue $node labels $labels]

	# Update the dag
	set Color(dag) [MRMLSetNode $Color(dag) $nodeIndex $node]
}

#-------------------------------------------------------------------------------
# .PROC MainColorsMakeColors
#
# Set the colors from Color(dag) into the Label WL.
# .END
#-------------------------------------------------------------------------------
proc MainColorsMakeColors {} {
	global Lut Color

	set lut  Lut($Lut(idLabel),lut)
	set iLut Lut($Lut(idLabel),indirectLUT)

	# Set default color to white, and thresholded color to clear black.
	set num [MRMLGetNumNodes $Color(dag)]
	$lut SetNumberOfTableValues [expr $num + 2]
	$lut SetTableValue 0 0.0 0.0 0.0 0.0
	$lut SetTableValue 1 1.0 1.0 1.0 1.0

	# Set colors for each label value
	$iLut InitDirect
	for {set n 0} {$n < $num} {incr n} {
		set node [MRMLGetNode $Color(dag) $n]

		set color [MRMLGetValue $node diffuseColor]
		eval $lut SetTableValue [expr $n+2] $color 1.0

		set values [MRMLGetValue $node labels]
		foreach v $values {
			$iLut MapDirect $v [expr $n+2]
		}
	}
	$iLut Build
}
 

#-------------------------------------------------------------------------------
# .PROC MainColorsBuildDag
#
# Build Color(dag) from the color nodes in "dag"
# .END
#-------------------------------------------------------------------------------
proc MainColorsBuildDag {dag} {
	global Color
	
	set Color(dag) ""
	set num [MRMLCountTypeOfNode $dag "Color"]
	for {set n 0} {$n < $num} {incr n} {
		set i [MRMLGetIndexOfNodeInDag $dag $n "Color"]
		set node [MRMLGetNode $dag $i]
		set Color(dag) [MRMLAppendNode $Color(dag) $node]
	}
}

#-------------------------------------------------------------------------------
# .PROC MainColorsGetColorFromLabel
#
# Find the color for a label
# .END
#-------------------------------------------------------------------------------
proc MainColorsGetColorFromLabel {label} {
	global Color

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

