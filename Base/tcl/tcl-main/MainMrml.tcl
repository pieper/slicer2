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

#-------------------------------------------------------------------------------
# .PROC MainMrmlInit
# .END
#-------------------------------------------------------------------------------
proc MainMrmlInit {} {
	global Model Volume Color Transform EndTransform Matrix
	global TransferFunction WindowLevel TFPoint ColorLUT Options

	foreach node "Color Model Volume Transform EndTransform Matrix \
		TransferFunction WindowLevel TFPoint ColorLUT Options" {
		set ${node}(nextID) 0
		set ${node}(idList) ""
		set ${node}(idListDelete) ""
	}
	set Volume(nextID) 1

	# Read MRML defaults file for version 1.0
	set fileName [ExpandPath "Defaults.mrml"]
	if {[CheckFileExists $fileName] == "0"} {
		set msg "Unable to read file MRML defaults file '$fileName'"
		puts $msg
		tk_messageBox -message $msg
		return	
	}
	MRMLReadDefaults $fileName

	set Path(mrmlFile) ""
}


proc MainMrmlUpdateMRML {} {
	global Mrml

	# Compute geometric relationships
	if {[info command Mrml(dataTree)] != ""} {
		Mrml(dataTree) ComputeTransforms
	}
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

proc MainMrmlClearList {} {
	global Model Volume Color Transform EndTransform Matrix
	global TransferFunction WindowLevel TFPoint ColorLUT Options

	foreach node "Color Model Volume Transform EndTransform Matrix \
		TransferFunction WindowLevel TFPoint ColorLUT Options" {
		set ${node}(idListDelete) ""
	}
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlDeleteNode
# .END
#-------------------------------------------------------------------------------
proc MainMrmlDeleteNode {nodeType id} {
	global Mrml Model Volume Color Transform EndTransform Matrix
	global TransferFunction WindowLevel TFPoint ColorLUT Options

	upvar $nodeType Array

	set tree "dataTree"
	if {$nodeType == "Color"} {
		set tree "colorTree"
	}

	MainMrmlClearList
	set Array(idListDelete) $id

	# Remove node's ID from idList
	set i [lsearch $Array(idList) $id]
	if {$i == -1} {return}
	set Array(idList) [lreplace $Array(idList) $i $i]

	# Remove node from tree, and delete it
	Mrml($tree) RemoveItem ${nodeType}($id,node)
	${nodeType}($id,node) Delete

	MainUpdateMRML

	MainMrmlClearList
}

proc MainMrmlDeleteAll {} {
	global Mrml Model Volume Color Transform EndTransform Matrix
	global TransferFunction WindowLevel TFPoint ColorLUT Options

	# Volumes are a special case because the "None" always exists
	foreach id $Volume(idList) {
		if {$id != $Volume(idNone)} {

			# Add to the deleteList
			lappend Volume(idListDelete) $id

			# Remove from the idList
			set i [lsearch $Volume(idList) $id]
			set Volume(idList) [lreplace $Volume(idList) $i $i]

			# Remove node from tree, and delete it
			Mrml(dataTree) RemoveItem Volume($id,node)
			Volume($id,node) Delete
		}
	}

	# dataTree
	foreach node "Model Transform EndTransform Matrix \
		TransferFunction WindowLevel TFPoint ColorLUT Options" {
		upvar 0 $node Array

		foreach id $Array(idList) {

			# Add to the deleteList
			lappend Array(idListDelete) $id

			# Remove from the idList
			set i [lsearch $Array(idList) $id]
			set Array(idList) [lreplace $Array(idList) $i $i]

			# Remove node from tree, and delete it
			Mrml(dataTree) RemoveItem ${node}($id,node)
			${node}($id,node) Delete
		}
	}

	# colorTree
	foreach node "Color" {
		upvar 0 $node Array

		foreach id $Array(idList) {

			# Add to the deleteList
			lappend Array(idListDelete) $id

			# Remove from the idList
			set i [lsearch $Array(idList) $id]
			set Array(idList) [lreplace $Array(idList) $i $i]

			# Remove node from tree, and delete it
			Mrml(colorTree) RemoveItem ${node}($id,node)
			${node}($id,node) Delete
		}
	}

	MainUpdateMRML

	MainMrmlClearList
}

proc MainMrmlRead {mrmlFile} {
	global Path Mrml

	# Open the file 'mrmlFile' to determine which MRML version it is,
	# and then call the appropriate routine to handle it.

	# Determine name of MRML input file.
	# Append ".mrml" or ".xml" if necessary.
	set fileName $mrmlFile
	if {[file extension $fileName] != ".mrml" && [file extension $fileName] != ".xml"} {
		if {[file exists $fileName.xml] == 1} {
			set fileName $fileName.xml
		} else {
			set fileName "$fileName.mrml"
		}
	}

	# Check the file exists
	if {[CheckFileExists $fileName] == "0"} {
		set errmsg "Unable to read input MRML file '$fileName'"
		puts $errmsg
		tk_messageBox -message $errmsg
		return	
	}

	# Store file and directory name
	set dir [file dirname $fileName]
	if {$dir == "" || $dir == "."} {
		set dir [pwd]
	}
	set Path(root) $dir
	set Path(mrmlFile) $fileName

	# Build a MRML Tree for data, and another for colors and LUTs
	if {[info command Mrml(dataTree)] == ""} {
		vtkMrmlTree Mrml(dataTree)
	}
	if {[info command Mrml(colorTree)] == ""} {
		vtkMrmlTree Mrml(colorTree)
	}

	MainMrmlDeleteAll

	# Open the file to determine it's type
	set fid [open $fileName r]
	gets $fid line
 	close $fid
	if {[lindex $line 0] == "MRML"} {
		puts "MRML V1.0"
		MainMrmlReadVersion1.0 $fileName
		MainMrmlBuildTreesVersion1.0
    } else {
		puts "MRML V2.0"
		set tags [MainMrmlReadVersion2.0 $fileName]
		set tags [MainMrmlAddColors $tags]
		MainMrmlBuildTreesVersion2.0 $tags
	}
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlReadVersion1.0
# .END
#-------------------------------------------------------------------------------
proc MainMrmlReadVersion1.0 {fileName} {
	global Lut Dag Volume Model Config Color Gui Path env Transform

	# Read file
	set Dag(read) [MRMLRead $fileName]
	if {$Dag(read) == "-1"} {
		tk_messageBox -message "Error reading MRML file: '$fileName'\n\
			See message written in console." 
		return
	}

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


proc MainMrmlReadVersion2.0 {fileName} {

	# Returns list of tags on success else 0

	# Read file
	if {[catch {set fid [open $fileName r]} errmsg] == 1} {
		puts $errmsg
		tk_messagebox -message $errmsg
		return 0
	}
	set mrml [read $fid]
	close $fid

	# Check that it's the right file type
	if {[regexp {<!DOCTYPE MRML SYSTEM "mrml20.dtd">} $mrml match] == 0} {
		set errmsg "The file is MRML version 2.0"
		tk_messageBox -message $errmsg
		return 0
	}

	# Strip off everything but the body
	if {[regexp {<MRML>(.*)</MRML>} $mrml match mrml] == 0} {
		# There's no content in the file
		return ""
	}

	# Strip leading white space
	regsub "^\[\n\t \]*" $mrml "" mrml

	set tags1 ""
	while {$mrml != ""} {

		# Find next tag
		if {[regexp {^<([^ >]*)([^>]*)>} $mrml match tag attr] == 0} {
				set errmsg "Invalid MRML file. Can't parse tags:\n$mrml"
			puts "$errmsg"
			tk_messageBox -message "$errmsg"
			return 0
		}

		# Strip off this tag, so we can continue.
		if {[lsearch "Transform /Transform" $tag] != -1} {
			set str "<$tag>"
		} else {
			set str "</$tag>"
		}
		set i [string first $str $mrml]
		set mrml [string range $mrml [expr $i + [string length $str]] end]

		# Give the EndTransform tag a name
		if {$tag == "/Transform"} {
			set tag EndTransform
		}

		# Append to List of tags1
		lappend tags1 "$tag $attr"

		# Strip leading white space
		regsub "^\[\n\t \]*" $mrml "" mrml
	}

	# Parse the attribute list for each tag
	set tags2 ""
	foreach pair $tags1 {
		set tag [lindex $pair 0]
		set attr [lreplace $pair 0 0]

		# Strip leading white space
		regsub "^\[\n\t \]*" $attr "" attr

		set attrList ""
		set ignore 0
		while {$attr != ""} {
		
			# Find the next key=value pair (and also strip it off... all in one step!)
			if {[regexp "^(\[^=\]*)\[\n\t \]*=\[\n\t \]*\['\"\](\[^'\"\]*)\['\"\](.*)$" \
				$attr match key value attr] == 0} {
				set errmsg "Invalid MRML file. Can't parse attributes:\n$attr"
				puts "$errmsg"
				tk_messageBox -message "$errmsg"
				return 0
			}
			if {$key == "ignore" && $value == "yes"} {
				set ignore 1
			}
			lappend attrList "$key $value"

			# Strip leading white space
			regsub "^\[\n\t \]*" $attr "" attr
		}

		# Add this tag if we're not ignoring it
		if {$ignore == 0} {
			lappend tags2 "$tag $attrList"
		}
	}
	return $tags2
}

proc MainMrmlAddColors {tags} {

	# If there are no Color nodes, then read, and append default colors.
	# Return a new list of tags, possibly including default colors.

	set colors 0
	foreach pair $tags {
		set tag  [lindex $pair 0]
		if {$tag == "Color"} {
			set colors 1
		}
	}
	if {$colors == 1} {return}
	
	set fileName [ExpandPath Colors.xml]
	set tagsColors [MainMrmlReadVersion2.0 $fileName]
	if {$tagsColors == 0} {
		set msg "Unable to read file default MRML color file '$fileName'"
		puts $msg
		tk_messageBox -message $msg
		return	
	}

	return "$tags $tagsColors"
}

proc MainMrmlBuildTreesVersion1.0 {} {
	global Dag Color Model Volume Transform EndTransform Matrix Path
	
	set level 0
	set transformCount($level) 0
	set dag $Dag(withColors)
	set num [MRMLGetNumNodes $dag]

	for {set j 0} {$j < $num} {incr j} {
		set node [MRMLGetNode $dag $j]
		set type [MRMLGetNodeType $node]
		switch $type {
		
		"Separator" {
			# Increment the separator level.
			# Initialize the counter of transforms inside this separator.
			# Add a Transform node.
			
			incr level
			set transformCount($level) 0

			set i $Transform(nextID)
			incr Transform(nextID)
			vtkMrmlTransformNode Transform($i,node)
			Mrml(dataTree) AddItem Transform($i,node)
		}
		
		"End" {
			# For each transform inside this separator, add an EndTransform node.
			# Also add an EndTransform node for this separator itself.

			for {set c 0} {$c < $transformCount($level)} {incr c} {
				set i $EndTransform(nextID)
				incr EndTransform(nextID)
				vtkMrmlEndTransformNode EndTransform($i,node)
				Mrml(dataTree) AddItem EndTransform($i,node)
			}
			set level [expr $level - 1]

			set i $EndTransform(nextID)
			incr EndTransform(nextID)
			vtkMrmlEndTransformNode EndTransform($i,node)
			Mrml(dataTree) AddItem EndTransform($i,node)
		}
		
		"Transform" {
			# Increment the count of transforms inside the current separator.
			# Add a Transform node and a Matrix node.
			
			incr transformCount($level)

			set i $Transform(nextID)
			incr Transform(nextID)
			vtkMrmlTransformNode Transform($i,node)
			Mrml(dataTree) AddItem Transform($i,node)

			set i $Matrix(nextID)
			incr Matrix(nextID)
			lappend Matrix(idList) $i
			vtkMrmlMatrixNode Matrix($i,node)
			set n Matrix($i,node)
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
			$n SetFilePattern      [MRMLGetValue $node filePattern]
			$n SetFilePrefix       [MRMLGetValue $node filePrefix]
			$n SetFullPrefix       [MRMLGetValue $node filePrefix]
			$n SetWindow           [MRMLGetValue $node window]
			$n SetLevel            [MRMLGetValue $node level]
			$n SetAutoWindowLevel  [MRMLGetValue $node autoWindowLevel]
			$n SetLabelMap         [MRMLGetValue $node labelMap]
			$n SetScanOrder        [MRMLGetValue $node scanOrder]

			# Options NOT CURRENTLY SUPPORTED
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

	# Add EndTransforms for each transform at the base level
	# (outside all separators)
	for {set c 0} {$c < $transformCount($level)} {incr c} {
		set i $EndTransform(nextID)
		incr EndTransform(nextID)
		vtkMrmlEndTransformNode EndTransform($i,node)
		Mrml(dataTree) AddItem EndTransform($i,node)
	}
}

proc MainMrmlBuildTreesVersion2.0 {tags} {
	global Mrml Path
	global Model Volume Color Transform EndTransform Matrix
	global TransferFunction WindowLevel TFPoint ColorLUT Options
	
	foreach pair $tags {
		set tag  [lindex $pair 0]
		set attr [lreplace $pair 0 0]

		switch $tag {
		
		"Transform" {
			set i $Transform(nextID)
			incr Transform(nextID)
			vtkMrmlTransformNode Transform($i,node)
			Mrml(dataTree) AddItem Transform($i,node)
		}
		
		"EndTransform" {
			set i $EndTransform(nextID)
			incr EndTransform(nextID)
			vtkMrmlEndTransformNode EndTransform($i,node)
			Mrml(dataTree) AddItem EndTransform($i,node)
		}
		
		"Matrix" {
			set i $Matrix(nextID)
			incr Matrix(nextID)
			lappend Matrix(idList) $i
			vtkMrmlMatrixNode Matrix($i,node)
			set n Matrix($i,node)
			$n SetID           $i
			foreach a $attr {
				set key [lindex $a 0]
				set val [lreplace $a 0 0]
				switch $key {
				"desc"   {$n SetDescription $val}
				"name"   {$n SetName        $val}
				"matrix" {$n SetMatrix      $val}
				}
			}
			Mrml(dataTree) AddItem $n
		}

		"Color" {
			set i $Color(nextID)
			incr Color(nextID)
			lappend Color(idList) $i
			vtkMrmlColorNode Color($i,node)
			set n Color($i,node)
			$n SetID           $i
			foreach a $attr {
				set key [lindex $a 0]
				set val [lreplace $a 0 0]
				switch $key {
				"desc"         {$n SetDescription  $val}
				"name"         {$n SetName         $val}
				"ambient"      {$n SetAmbient      $val}
				"diffuse"      {$n SetDiffuse      $val}
				"specular"     {$n SetSpecular     $val}
				"power"        {$n SetPower        $val}
				"labels"       {$n SetLabels       $val}
				"diffuseColor" {eval $n SetDiffuseColor $val}
				}
			}
			Mrml(colorTree) AddItem $n
		}
		
		"Model" {
			set i $Model(nextID)
			incr Model(nextID)
			lappend Model(idList) $i
			vtkMrmlModelNode Model($i,node)
			set n Model($i,node)
			$n SetID               $i
			foreach a $attr {
				set key [lindex $a 0]
				set val [lreplace $a 0 0]
				switch $key {
				"desc"             {$n SetDescription  $val}
				"name"             {$n SetName         $val}
				"fileName"         {$n SetFileName     $val}
				"color"            {$n SetColor        $val}
				"opacity"          {$n SetOpacity      $val}
				"scalarRange"      {eval $n SetScalarRange $val}
				"visibility" {
					if {$val == "yes"} {
						$n SetVisibility 1
					} else {
						$n SetVisibility 0
					}
				}
				"clipping" {
					if {$val == "yes"} {
						$n SetClipping 1
					} else {
						$n SetClipping 0
					}
				}
				"backfaceCulling" {
					if {$val == "yes"} {
						$n SetBackfaceCulling 1
					} else {
						$n SetBackfaceCulling 0
					}
				}
				"scalarVisibility" {
					if {$val == "yes"} {
						$n SetScalarVisibility 1
					} else {
						$n SetScalarVisibility 0
					}
				}
				}
			}

			# Compute full path name relative to the MRML file
			$n SetFullFileName [file join $Path(root) [$n GetFileName]]

			Mrml(dataTree) AddItem $n
		}
		
		"Volume" {
			set i $Volume(nextID)
			incr Volume(nextID)
			lappend Volume(idList) $i
			vtkMrmlVolumeNode Volume($i,node)
			set n Volume($i,node)
			$n SetID     $i 
			foreach a $attr {
				set key [lindex $a 0]
				set val [lreplace $a 0 0]
				switch $key {
				"desc"            {$n SetDescription    $val}
				"name"            {$n SetName           $val}
				"filePattern"     {$n SetFilePattern    $val}
				"filePrefix"      {$n SetFilePrefix     $val}
				"imageRange"      {eval $n SetImageRange     $val}
				"littleEndian"    {$n SetLittleEndian   $val}
				"spacing"         {eval $n SetSpacing        $val}
				"dimensions"      {eval $n SetDimensions     $val}
				"labelMap"        {$n SetLabelMap       $val}
				"scalarType"      {$n SetScalarTypeTo$val}
				"numScalars"      {$n SetNumScalars     $val}
				"rasToIjkMatrix"  {$n SetRasToIjkMatrix $val}
				"rasToVtkMatrix"  {$n SetRasToVtkMatrix $val}
				"colorLUT"        {$n SetLUTName        $val}
				"window"          {$n SetWindow         $val}
				"level"           {$n SetLevel          $val}
				"lowerThreshold"  {$n SetLowerThreshold $val}
				"upperThreshold"  {$n SetUpperThreshold $val}
				"autoWindowLevel" {
					if {$val == "yes"} {
						$n SetAutoWindowLevel 1
					} else {
						$n SetAutoWindowLevel 0
					}
				}
				"autoThreshold" {
					if {$val == "yes"} {
						$n SetAutoThreshold 1
					} else {
						$n SetAutoThreshold 0
					}
				}
				"interpolate" {
					if {$val == "yes"} {
						$n SetInterpolate 1
					} else {
						$n SetInterpolate 0
					}
				}
				}
			}

			# Compute full path name relative to the MRML file
			$n SetFullPrefix [file join $Path(root) [$n GetFilePrefix]]
			
			$n UseRasToVtkMatrixOn

			Mrml(dataTree) AddItem $n
		}
		}
	}
}

proc MainMrmlWrite {filename} {
	global Mrml

	Mrml(dataTree) Write $filename
}

