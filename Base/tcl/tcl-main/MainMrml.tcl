#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use, copy, and distribute this
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
# FILE:        MainMrml.tcl
# DATE:        12/09/1999 14:09
# LAST EDITOR: gering
# PROCEDURES:  
#   MainMrmlInit
#   MainMrmlDeleteModel
#   MainMrmlRead
#==========================================================================auto=

proc MainMrmlClearList {} {
	global Model Volume Color Transform

	set Volume(idListDelete) ""  
	set Model(idListDelete) ""
	set Color(idListDelete) ""
	set Transform(idListDelete) ""
	set Transform(end,idListDelete) ""
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlInit
# .END
#-------------------------------------------------------------------------------
proc MainMrmlInit {} {
	global Model Volume Color Transform

	set Color(nextID) 0
	set Model(nextID) 0
	set Volume(nextID) 1
	set Transform(nextID) 0
	set Transform(end,nextID) 0

	set Color(idList) ""
	set Model(idList) ""
	set Volume(idList) ""
	set Transform(idList) ""
	set Transform(end,idList) ""

	set Volume(idListDelete) ""  
	set Model(idListDelete) ""
	set Color(idListDelete) ""
	set Transform(idListDelete) ""
	set Transform(end,idListDelete) ""

	# Read MRML defaults file
	set fileName [ExpandPath "Defaults.mrml"]
	if {[CheckFileExists $fileName] == "0"} {
		set msg "Unable to read file MRML defaults file '$fileName'"
		puts $msg
		tk_messageBox -message $msg
		return	
	}
	MRMLReadDefaults $fileName
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlDeleteNode
# .END
#-------------------------------------------------------------------------------
proc MainMrmlDeleteNode {nodeType id} {
	global Model Volume Color Transform

	upvar $nodeType Array

	set end ""
	if {$nodeType == "TransformEnd"} {
		set end "end,"
	}
	set tree "dataTree"
	if {$nodeType == "Color"} {
		set tree "colorTree"
	}

	MainMrmlClearList
	set Array(${end}idListDelete) $id

	# Remove node's ID from idList
	set i [lsearch $Array(${end}idList) $id]
	if {$i == -1} {return}
	set Array(${end}idList) [lreplace $Array(${end}idList) $i $i]

	# Remove node from tree, and delete it
	Mrml($tree) RemoveItem ${nodeType}(${end}$id,node)
	${nodeType}(${end}$id,node) Delete

	MainUpdateMRML

	MainMrmlClearList
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlReadDag
# .END
#-------------------------------------------------------------------------------
proc MainMrmlReadDag {mrmlFile} {
	global Lut Dag Volume Model Config Color Gui Path env Transform

	# Determine name of MRML input file.
	# Append ".mrml" if necessary.
	set fileName $mrmlFile

	if {$fileName == ""} {
		set Dag(read) [MRMLCreateDag]
	} else {
		if {[file extension $fileName] != ".mrml"} {
			set fileName "$fileName.mrml"
		}
		# Read MRML input file
		if {[CheckFileExists $fileName] == "0"} {
			set msg "Unable to read input MRML file '$fileName'"
			puts $msg
			tk_messageBox -message $msg
			return	
		}
		set Dag(read) [MRMLRead $fileName]
		if {$Dag(read) == "-1"} {
			tk_messageBox -message "Error reading MRML file: '$fileName'\n\
				See message written in console." 
			return
		}
	}

	# Store directory name
	set dir [file dirname $fileName]
	if {$dir == "" || $dir == "."} {
		set dir [pwd]
	}
	set Path(root) $dir

	# Expand URLs
	set Dag(expanded) [MRMLExpandUrls $Dag(read) $Path(root)]

	# If there are no Color nodes, then read, expand, append default colors.
	set n [MRMLCountTypeOfNode $Dag(expanded) Color]
	if {$n == 0} {
		set fileName [ExpandPath Colors.mrml]
		if {[CheckFileExists $fileName] == "0"} {
			set msg "Unable to read file default MRML color file '$fileName'"
			puts $msg
			tk_messageBox -message $msg
			return	
		}
		set Dag(defaultColors) [MRMLRead $fileName]
		if {$Dag(defaultColors) == "-1"} {
			tk_messageBox -message "Error reading MRML file: '$fileName'\n\
				See message written in console." 
			return
		}
		set Dag(defaultColors) [MRMLExpandUrls $Dag(defaultColors) [file dirname $fileName]]
		set Dag(withColors) [MRMLAppendDag $Dag(expanded) $Dag(defaultColors)]
	} else {
		set Dag(withColors) $Dag(expanded)
	}
}

proc MainMrmlBuildTrees {} {
	global Dag Color Model Volume Transform
	
	# Build a MRML Tree for data, and another for colors and LUTs

	vtkMrmlTree Mrml(dataTree)
	vtkMrmlTree Mrml(colorTree)

	# Dag
	set level 0
	set transformCount($level) 0
	set dag $Dag(withColors)
	set num [MRMLGetNumNodes $dag]
	for {set j 0} {$j < $num} {incr j} {
		set node [MRMLGetNode $dag $j]
		set type [MRMLGetNodeType $node]
		switch $type {
		
		"Separator" {
			incr level
			set transformCount($level) 0
		}
		
		"End" {
			foreach c $transformCount($level) {
				set i $Transform(end,nextID)
				incr Transform(end,nextID)
				vtkMrmlEndTransformNode Transform(end,$i,node)

				Mrml(dataTree) AddItem Transform(end,$i,node)
			}
			decr level
		}
		
		"Transform" {
			incr transformCount($level)

			set i $Transform(nextID)
			incr Transform(nextID)
			lappend Transform(idList) $i
			vtkMrmlTransformNode Transform($i,node)
			set n Transform($i,node)
			$n SetID           $i
			$n SetDescription  [MRMLGetValue $node desc]
			$n SetName         [MRMLGetValue $node name]
			$n SetMatrix       [MRMLGetValue $node matrix]
			eval $n Scale      [MRMLGetValue $node scale]
			$n RotateX         [MRMLGetValue $node rotateX]
			$n RotateY         [MRMLGetValue $node rotateY]
			$n RotateZ         [MRMLGetValue $node rotateZ]
			eval $n Translate  [MRMLGetValue $node translate]
			Mrml(dataTree) AddItem $n
		}

		"Color" {
			set i $Color(nextID)
			incr Color(nextID)
			lappend Color(idList) $i
			vtkMrmlColorNode Color($i,node)
			set n Color($i,node)
			$n SetID           $i
			$n SetDescription  ""
			$n SetName         [MRMLGetValue $node name]
			$n SetAmbient      [MRMLGetValue $node ambient]
			$n SetDiffuse      [MRMLGetValue $node diffuse]
			$n SetSpecular     [MRMLGetValue $node specular]
			$n SetPower        [MRMLGetValue $node power]
			$n SetLabels       [MRMLGetValue $node labels]
			eval $n SetDiffuseColor [MRMLGetValue $node diffuseColor]

			Mrml(colorTree) AddItem $n
		}
		
		"Model" {
			set i $Model(nextID)
			incr Model(nextID)
			lappend Model(idList) $i
			vtkMrmlModelNode Model($i,node)
			set n Model($i,node)
			$n SetID               $i
			$n SetDescription      [MRMLGetValue $node desc]
			$n SetName             [MRMLGetValue $node name]
			$n SetFileName         [MRMLGetValue $node fileName]
			$n SetColor            [MRMLGetValue $node colorName]
			$n SetOpacity          [MRMLGetValue $node opacity]
			$n SetVisibility       [MRMLGetValue $node visibility]
			$n SetClipping         [MRMLGetValue $node clipping]
			$n SetBackfaceCulling  [MRMLGetValue $node backfaceCulling]
			$n SetScalarVisibility [MRMLGetValue $node scalarVisibility]
			eval $n SetScalarRange [MRMLGetValue $node scalarRange]

			Mrml(dataTree) AddItem $n
		}
		
		"Volume" {
			set i $Volume(nextID)
			incr Volume(nextID)
			lappend Volume(idList) $i
			vtkMrmlVolumeNode Volume($i,node)
			set n Volume($i,node)
			$n SetID               $i
			$n SetDescription      [MRMLGetValue $node desc]
			$n SetName             [MRMLGetValue $node name]
			eval $n SetImageRange  [MRMLGetValue $node imageRange]
			eval $n SetDimensions  [MRMLGetValue $node dimensions]
			eval $n SetSpacing     [MRMLGetValue $node spacing]
			$n SetScalarTypeTo[MRMLGetValue $node scalarType]
			$n SetNumScalars       [MRMLGetValue $node numScalars]
			$n SetLittleEndian     [MRMLGetValue $node littleEndian]
			$n SetTilt             [MRMLGetValue $node tilt]
			$n SetRasToIjkMatrix   [MRMLGetValue $node rasToIjkMatrix]
			$n SetRasToVtkMatrix   [MRMLGetValue $node rasToVtkMatrix]
			$n UseRasToVtkMatrixOn
			$n SetFilePrefix       [MRMLGetValue $node filePrefix]
			$n SetFilePattern      [MRMLGetValue $node filePattern]
			$n SetFullPrefix       [MRMLGetValue $node filePrefix]
			$n SetWindow           [MRMLGetValue $node window]
			$n SetLevel            [MRMLGetValue $node level]
			$n SetAutoWindowLevel  [MRMLGetValue $node autoWindowLevel]
			$n SetLabelMap         [MRMLGetValue $node labelMap]
			$n SetScanOrder        [MRMLGetValue $node scanOrder]

			# Options DAVE
			set program [lindex [MRMLGetValue $node options] 0]
			set options [lrange [MRMLGetValue $node options] 1 end]
			if {$program == "slicer"} {
				# Parse options in format: key1 value1 key2 value2 ...
				# Verify that options exist in the list of defaults.
				foreach {key value} $options {
					set $key $value
				}
			}
#			$n SetLUTName        [MRMLGetValue $node lutID]
#			$n SetUpperThreshold [MRMLGetValue $node showAbove]
#			$n SetLowerThreshold [MRMLGetValue $node showBelow]
#			$n SetInterpolate    [MRMLGetValue $node interpolate]
#			if {[MRMLGetValue $node autoThreshold] == 1} {
#				$n AutoThresholdOn
#			} else {
#				$n AutoThresholdOff
#			}

			# Don't interpolate label maps
			if {[MRMLGetValue $node labelMap] == 1} {
				$n SetInterpolate [MRMLGetValue $node interpolate]
			}

			# If we don't specify headerSize, then it just reads the
			# last bytes in the file as the pictures.  This allows
			# variable file header lengths, as in is true of DICOM.

			Mrml(dataTree) AddItem $n
		}
		}
	}
	puts "Colors: $Color(idList)"
	puts "Models: $Model(idList)"
	puts "Volumes: $Volume(idList)"
}

proc MainMrmlUpdateMRML {} {
	global Mrml

	# Compute geometric relationships
	Mrml(dataTree) ComputeTransforms
}

proc MainMrmlDumpTree {type} {
	global Mrml

	set tree Mrml(${type}Tree)
	$tree InitTraversal
	set node [$tree GetNextItem]
    while {$node != ""} {
		puts "dump='$node'"
		set node [$tree GetNextItem]
    }
}
