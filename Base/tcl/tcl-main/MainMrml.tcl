#=auto==========================================================================
# Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
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
# FILE:        MainMrml.tcl
# PROCEDURES:  
#   MainMrmlInit
#   MainMrmlInitIdLists
#   MainMrmlUpdateMRML
#   MainMrmlDumpTree
#   MainMrmlPrint
#   MainMrmlClearList
#   MainMrmlAddNode nodeType
#   MainMrmlUndoAddNode
#   MainMrmlDeleteNodeDuringUpdate
#   MainMrmlDeleteNode
#   MainMrmlDeleteAll
#   MainMrmlSetFile
#   MainMrmlRead mrmlFile
#   MainMrmlBuildTreesVersion2.0
#   MainMrmlReadVersion1.0
#   MainMrmlBuildTreesVersion1.0
#   MainMrmlAddColors
#   MainMrmlCheckColors
#   MainMrmlRelativity
#   MainMrmlWrite
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MainMrmlInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlInit {} {
	global Mrml

	MainMrmlInitIdLists 

	# Read MRML defaults file for version 1.0
	set fileName [ExpandPath "Defaults.mrml"]
	if {[CheckFileExists $fileName] == "0"} {
		set msg "Unable to read file MRML defaults file '$fileName'"
		puts $msg
		tk_messageBox -message $msg
		exit	
	}
	MRMLReadDefaults $fileName

        # Set version info
        lappend Module(versions) [ParseCVSInfo MainMrml \
		{$Revision: 1.32 $} {$Date: 2000/03/01 02:21:01 $}]

	set Mrml(filePrefix) data
	set Mrml(colorsUnsaved) 0
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlInitIdLists
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlInitIdLists {} {
	global Mrml
	global Model Volume Color Transform EndTransform Matrix
	global TransferFunction WindowLevel TFPoint ColorLUT Options

	foreach node "Color Model Volume Transform EndTransform Matrix \
		TransferFunction WindowLevel TFPoint ColorLUT Options" {
		set ${node}(nextID) 0
		set ${node}(idList) ""
		set ${node}(idListDelete) ""
	}
	set Volume(idList) 0
	set Volume(nextID) 1
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlUpdateMRML {} {
	global Mrml

	# Compute geometric relationships
	if {[info command Mrml(dataTree)] != ""} {
		Mrml(dataTree) ComputeTransforms
	}
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlDumpTree
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------
# .PROC MainMrmlPrint
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlPrint {tags} {

	set level 0
	foreach pair $tags {
		set tag  [lindex $pair 0]
		set attr [lreplace $pair 0 0]

		# Process EndTransform
		if {$tag == "EndTransform"} {
			set level [expr $level - 1]
		}
		set indent ""
		for {set i 0} {$i < $level} {incr i} {
			set indent "$indent  "
		}

		puts "${indent}$tag"

		# Process Transform
		if {$tag == "Transform"} {
			incr level
		}
		set indent ""
		for {set i 0} {$i < $level} {incr i} {
			set indent "$indent  "
		}

		foreach a $attr {
			set key   [lindex $a 0]
			set value [lreplace $a 0 0]
			puts "${indent}  $key=$value"
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlClearList
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlClearList {} {
	global Model Volume Color Transform EndTransform Matrix
	global TransferFunction WindowLevel TFPoint ColorLUT Options

	foreach node "Color Model Volume Transform EndTransform Matrix \
		TransferFunction WindowLevel TFPoint ColorLUT Options" {
		set ${node}(idListDelete) ""
	}
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlAddNode
#
#  Adds a node to the data tree.
#  Returns the MrmlMode
# 
# .ARGS
#  str nodeType the type of node, \"Volume\", \"Color\", etc.
# .END
#-------------------------------------------------------------------------------
proc MainMrmlAddNode {nodeType} {
	global Mrml Model Volume Color Transform EndTransform Matrix Options
	global TransferFunction WindowLevel TFPoint ColorLUT 

	upvar $nodeType Array

	set tree "dataTree"
	if {$nodeType == "Color"} {
		set tree "colorTree"
	}

	# Add ID to idList
	set i $Array(nextID)
	incr Array(nextID)
	lappend Array(idList) $i

	# Put the None volume at the end
	if {$nodeType == "Volume"} {
		set j [lsearch $Volume(idList) $Volume(idNone)]
		set Volume(idList) "[lreplace $Volume(idList) $j $j] $Volume(idNone)"
	}

	# Create vtkMrmlNode
	set n ${nodeType}($i,node)
	vtkMrml${nodeType}Node $n
	$n SetID $i

	# Add node to tree
	Mrml($tree) AddItem $n

	# Return node
	return ${nodeType}($i,node)
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlUndoAddNode
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlUndoAddNode {nodeType n} {
	global Mrml Model Volume Color Transform EndTransform Matrix Options
	global TransferFunction WindowLevel TFPoint ColorLUT 

	upvar $nodeType Array

	set tree "dataTree"
	if {$nodeType == "Color"} {
		set tree "colorTree"
	}

	# Remove node's ID from idList
	set id [$n GetID]
	set i [lsearch $Array(idList) $id]
	if {$i == -1} {return}
	set Array(idList) [lreplace $Array(idList) $i $i]
	set Volume(nextID) [expr $Volume(nextID) - 1]

	# Remove node from tree, and delete it
	Mrml($tree) RemoveItem $n
	$n Delete
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlDeleteNodeDuringUpdate
# Call this routine to delete a node during MainUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc MainMrmlDeleteNodeDuringUpdate {nodeType id} {
	global Mrml Model Volume Color Transform EndTransform Matrix
	global TransferFunction WindowLevel TFPoint ColorLUT Options

	upvar $nodeType Array

	set tree "dataTree"
	if {$nodeType == "Color"} {
		set tree "colorTree"
	}

	lappend Array(idListDelete) $id

	# Remove node's ID from idList
	set i [lsearch $Array(idList) $id]
	if {$i == -1} {return}
	set Array(idList) [lreplace $Array(idList) $i $i]

	# Remove node from tree, and delete it
	Mrml($tree) RemoveItem ${nodeType}($id,node)
	${nodeType}($id,node) Delete
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlDeleteNode
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlDeleteNode {nodeType id} {
	global Mrml Model Volume Color Transform EndTransform Matrix Options
	global TransferFunction WindowLevel TFPoint ColorLUT 

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

#-------------------------------------------------------------------------------
# .PROC MainMrmlDeleteAll
# 
#  Delete all volumes, models and transforms.
#
# .END
#-------------------------------------------------------------------------------
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

	MainMrmlInitIdLists
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlSetFile
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlSetFile {filename} {
	global Mrml File

	# Store the directory of the MRML file as the Mrml(dir)
	set dir [file dirname $filename]
	if {$dir == "" || $dir == "."} {
		set dir [pwd]
	}
	set Mrml(dir) $dir

	# Store the new relative prefix
	set Mrml(filePrefix) [MainFileGetRelativePrefix $filename]

	# Synchronize with saving/opening MRML files from the menu bar
	set File(filePrefix) $Mrml(filePrefix)
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlRead
#
#
#  Delete the loaded Mrml data from memory.  Replace with a new Mrml file.
#  Append \".xml\" or \".mrml\" an necessary to the file name as necessary.
# .ARGS
# str mrmlFile name of a MRML file to load
# .END
#-------------------------------------------------------------------------------
proc MainMrmlRead {mrmlFile} {
	global Path Mrml Volume

	# Open the file 'mrmlFile' to determine which MRML version it is,
	# and then call the appropriate routine to handle it.

	# Determine name of MRML input file.
	# Append ".mrml" or ".xml" if necessary.
	set fileName $mrmlFile
	if {[file extension $fileName] != ".mrml" && [file extension $fileName] != ".xml"} {
		if {[file exists $fileName.xml] == 1} {
			set fileName $fileName.xml
		} elseif {[file exists $fileName.mrml] == 1} {
			set fileName "$fileName.mrml"
		}
	}

	# Build a MRML Tree for data, and another for colors and LUTs
	if {[info command Mrml(dataTree)] == ""} {
		vtkMrmlTree Mrml(dataTree)
	}
	if {[info command Mrml(colorTree)] == ""} {
		vtkMrmlTree Mrml(colorTree)
	}

	# Check the file exists
	if {$fileName != "" && [CheckFileExists $fileName 0] == "0"} {
		set errmsg "Unable to read input MRML file '$fileName'"
		puts $errmsg
		tk_messageBox -message $errmsg

		# At least read colors
		set tags [MainMrmlAddColors ""]
		MainMrmlBuildTreesVersion2.0 $tags
		return	
	}

	# no file?
	if {$fileName == ""} {
		# At least read colors
		set tags [MainMrmlAddColors ""]
		MainMrmlBuildTreesVersion2.0 $tags
		return	
	}

	MainMrmlDeleteAll

	# Store file and directory name
	MainMrmlSetFile $fileName

	# Colors don't need saving now
	set Mrml(colorsUnsaved) 0

	# Open the file to determine its type
	set version 2
	if {$fileName == ""} {
		set version 1
	} else {
		set fid [open $fileName r]
		gets $fid line
 		close $fid
		if {[lindex $line 0] == "MRML"} {
			set version 1
		}
	}
	if {$version == 1} {
		puts "Reading MRML V1.0: $fileName"
		MainMrmlReadVersion1.0 $fileName
		MainMrmlBuildTreesVersion1.0

		# Always add Colors.xml
		set tags [MainMrmlAddColors ""]
		MainMrmlBuildTreesVersion2.0 $tags
	} else {
		puts "Reading MRML V2.0: $fileName"
		set tags [MainMrmlReadVersion2.0 $fileName]

		# Only add colors if none exist
		set tags [MainMrmlAddColors $tags]

		MainMrmlBuildTreesVersion2.0 $tags
	}

	# Put the None volume at the end
	set i [lsearch $Volume(idList) $Volume(idNone)]
	set Volume(idList) "[lreplace $Volume(idList) $i $i] $Volume(idNone)"
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlBuildTreesVersion2.0
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlBuildTreesVersion2.0 {tags} {
	global Mrml
	global Model Volume Color Transform EndTransform Matrix
	global TransferFunction WindowLevel TFPoint ColorLUT Options
	global Preset

	foreach pair $tags {
		set tag  [lindex $pair 0]
		set attr [lreplace $pair 0 0]

		switch $tag {
		
		"Transform" {
			set n [MainMrmlAddNode Transform]
		}
		
		"EndTransform" {
			set n [MainMrmlAddNode EndTransform]
		}
		
		"Matrix" {
			set n [MainMrmlAddNode Matrix]
			foreach a $attr {
				set key [lindex $a 0]
				set val [lreplace $a 0 0]
				switch $key {
				"desc"   {$n SetDescription $val}
				"name"   {$n SetName        $val}
				"matrix" {$n SetMatrix      $val}
				}
			}
		}

		"Color" {
			set n [MainMrmlAddNode Color]
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
		}
		
		"Model" {
			set n [MainMrmlAddNode Model]
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
			$n SetFullFileName [file join $Mrml(dir) [$n GetFileName]]
		}
		
		"Volume" {
			set n [MainMrmlAddNode Volume]
			foreach a $attr {
				set key [lindex $a 0]
				set val [lreplace $a 0 0]
				switch $key {
				"desc"            {$n SetDescription    $val}
				"name"            {$n SetName           $val}
				"filePattern"     {$n SetFilePattern    $val}
				"filePrefix"      {$n SetFilePrefix     $val}
				"imageRange"      {eval $n SetImageRange $val}
				"spacing"         {eval $n SetSpacing   $val}
				"dimensions"      {eval $n SetDimensions $val}
				"scalarType"      {$n SetScalarTypeTo$val}
				"numScalars"      {$n SetNumScalars     $val}
				"rasToIjkMatrix"  {$n SetRasToIjkMatrix $val}
				"rasToVtkMatrix"  {$n SetRasToVtkMatrix $val}
				"positionMatrix"  {$n SetPositionMatrix $val}
				"scanOrder"       {$n SetScanOrder $val}
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
				"applyThreshold" {
					if {$val == "yes"} {
						$n SetApplyThreshold 1
					} else {
						$n SetApplyThreshold 0
					}
				}
				"interpolate" {
					if {$val == "yes"} {
						$n SetInterpolate 1
					} else {
						$n SetInterpolate 0
					}
				}
				"labelMap" {
					if {$val == "yes"} {
						$n SetLabelMap 1
					} else {
						$n SetLabelMap 0
					}
				}
				"littleEndian" {
					if {$val == "yes"} {
						$n SetLittleEndian 1
					} else {
						$n SetLittleEndian 0
					}
				}
				}
			}

			# Compute full path name relative to the MRML file
			$n SetFullPrefix [file join $Mrml(dir) [$n GetFilePrefix]]
			
			$n UseRasToVtkMatrixOn
		}

		"Options" {
			foreach a $attr {
				set key [lindex $a 0]
				set val [lreplace $a 0 0]
				switch $key {
					"options"      {set options $val}
					"program"      {set program $val}
					"contents"     {set contents $val}
				}
			}
			# I don't want any of gimpy's stinkin' modules in my tree!
			if {$contents != "modules"} {
				set n [MainMrmlAddNode Options]
				$n SetOptions $options
				$n SetProgram $program
				$n SetContents $contents
			}

			# Check that this is a slicer options node.
			if {[$n GetProgram] != "slicer"} {
			    return
			}

			# If these are presets, then do preset stuff on stuffing, not attr
			if {[$n GetContents] == "presets"} {
				# Since presets aren't written to the MRML file when different
				# from their default values, I must first reset them to their defaults.
				MainOptionsUseDefaultPresets
				MainOptionsParsePresets $attr
			}
		}

	    }
	}
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlReadVersion1.0
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlReadVersion1.0 {fileName} {
	global Lut Dag Volume Model Config Color Gui Mrml env Transform

	# Read file
	if {$fileName == ""} {
		set Dag(read) [MRMLCreateDag]
	} else {
		set Dag(read) [MRMLRead $fileName]
		if {$Dag(read) == "-1"} {
			tk_messageBox -message "Error reading MRML file: '$fileName'\n\
				See message written in console." 
			return
		}
	}

	# Expand URLs
	set Dag(expanded) [MRMLExpandUrls $Dag(read) $Mrml(dir)]
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlBuildTreesVersion1.0
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlBuildTreesVersion1.0 {} {
	global Dag Color Model Volume Transform EndTransform Matrix Path
	
	set level 0
	set transformCount($level) 0
	set dag $Dag(expanded)
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

			set n [MainMrmlAddNode Transform]
		}
		
		"End" {
			# For each transform inside this separator, add an EndTransform node.
			# Also add an EndTransform node for this separator itself.

			for {set c 0} {$c < $transformCount($level)} {incr c} {
				set n [MainMrmlAddNode EndTransform]
			}
			set level [expr $level - 1]

			set n [MainMrmlAddNode EndTransform]
		}
		
		"Transform" {
			# Increment the count of transforms inside the current separator.
			# Add a Transform node and a Matrix node.
			
			incr transformCount($level)

			set n [MainMrmlAddNode Transform]

			set n [MainMrmlAddNode Matrix]
			$n SetDescription  [MRMLGetValue $node desc]
			$n SetName         [MRMLGetValue $node name]
			$n SetMatrix       [MRMLGetValue $node matrix]
			eval $n Scale      [MRMLGetValue $node scale]
			$n RotateX         [MRMLGetValue $node rotateX]
			$n RotateY         [MRMLGetValue $node rotateY]
			$n RotateZ         [MRMLGetValue $node rotateZ]
			eval $n Translate  [MRMLGetValue $node translate]
		}

		"Color" {
			set n [MainMrmlAddNode Color]
			$n SetDescription  ""
			$n SetName         [MRMLGetValue $node name]
			$n SetAmbient      [MRMLGetValue $node ambient]
			$n SetDiffuse      [MRMLGetValue $node diffuse]
			$n SetSpecular     [MRMLGetValue $node specular]
			$n SetPower        [MRMLGetValue $node power]
			$n SetLabels       [MRMLGetValue $node labels]
			eval $n SetDiffuseColor [MRMLGetValue $node diffuseColor]
		}
		
		"Model" {
			set n [MainMrmlAddNode Model]
			$n SetDescription      [MRMLGetValue $node desc]
			$n SetName             [MRMLGetValue $node name]
			$n SetFileName         [MRMLGetValue $node fileName]
			$n SetFullFileName     [MRMLGetValue $node fileName]
			$n SetColor            [MRMLGetValue $node colorName]
			$n SetOpacity          [MRMLGetValue $node opacity]
			$n SetVisibility       [MRMLGetValue $node visibility]
			$n SetClipping         [MRMLGetValue $node clipping]
			$n SetBackfaceCulling  [MRMLGetValue $node backfaceCulling]
			$n SetScalarVisibility [MRMLGetValue $node scalarVisibility]
			eval $n SetScalarRange [MRMLGetValue $node scalarRange]
		}
		
		"Volume" {
			set n [MainMrmlAddNode Volume]
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
			puts "auto=[$n GetAutoWindowLevel]"
			$n SetLabelMap         [MRMLGetValue $node labelMap]
			$n SetScanOrder        [MRMLGetValue $node scanOrder]

			# Don't interpolate label maps
			if {[MRMLGetValue $node labelMap] == 1} {
				$n SetInterpolate [MRMLGetValue $node interpolate]
			}

			# Options NOT CURRENTLY SUPPORTED
#			set program [lindex [MRMLGetValue $node options] 0]
#			set options [lrange [MRMLGetValue $node options] 1 end]
#			if {$program == "slicer"} {
#				# Parse options in format: key1 value1 key2 value2 ...
#				# Verify that options exist in the list of defaults.
#				foreach {key value} $options {
#					set $key $value
#				}
#			}
#			$n SetLUTName        [MRMLGetValue $node lutID]
#			$n SetUpperThreshold [MRMLGetValue $node showAbove]
#			$n SetLowerThreshold [MRMLGetValue $node showBelow]
#			$n SetInterpolate    [MRMLGetValue $node interpolate]
#			if {[MRMLGetValue $node autoThreshold] == 1} {
#				$n AutoThresholdOn
#			} else {
#				$n AutoThresholdOff
#			}
		}
		}
	}

	# Add EndTransforms for each transform at the base level
	# (outside all separators)
	for {set c 0} {$c < $transformCount($level)} {incr c} {
		set n [MainMrmlAddNode EndTransform]
	}
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlAddColors
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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
	if {$colors == 1} {return $tags}
	
	set fileName [ExpandPath Colors.xml]
	set tagsColors [MainMrmlReadVersion2.0 $fileName]
	if {$tagsColors == 0} {
		set msg "Unable to read file default MRML color file '$fileName'"
		puts $msg
		tk_messageBox -message $msg
		return $tags
	}

	return "$tags $tagsColors"
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlCheckColors
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlCheckColors {} {
	global Mrml
	
	set fileName [ExpandPath Colors.xml]
	set tags [MainMrmlReadVersion2.0 $fileName]

	if {$tags != 0} {
		vtkMrmlColorNode default
		set n default

		Mrml(colorTree) InitTraversal
		set node [Mrml(colorTree) GetNextItem]

		foreach pair $tags {
			set tag  [lindex $pair 0]
			set attr [lreplace $pair 0 0]

			# Are we out of nodes?
			if {$node == ""} {
				set Mrml(colorsUnsaved) 1
			} else {
				if {$tag == "Color"} {
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
					if {[$node GetDescription] != [$n GetDescription]} {
						set Mrml(colorsUnsaved) 1
					}
					if {[$node GetName] != [$n GetName]} {
						set Mrml(colorsUnsaved) 1
					}
					if {[$node GetAmbient] != [$n GetAmbient]} {
						set Mrml(colorsUnsaved) 1
					}
					if {[$node GetDiffuse] != [$n GetDiffuse]} {
						set Mrml(colorsUnsaved) 1
					}
					if {[$node GetSpecular] != [$n GetSpecular]} {
						set Mrml(colorsUnsaved) 1
					}
					if {[$node GetPower] != [$n GetPower]} {
						set Mrml(colorsUnsaved) 1
					}
					if {[$node GetLabels] != [$n GetLabels]} {
						set Mrml(colorsUnsaved) 1
					}
					if {[$node GetDiffuseColor] != [$n GetDiffuseColor]} {
						set Mrml(colorsUnsaved) 1
					}
					set node [Mrml(colorTree) GetNextItem]
				}
			}
		}
		default Delete

		# Out of tags
		if {$node != ""} {
			set Mrml(colorsUnsaved) 1
		}
	}

}

#-------------------------------------------------------------------------------
# .PROC MainMrmlRelativity
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlRelativity {oldRoot} {
	global Mrml

	Mrml(dataTree) InitTraversal
	set node [Mrml(dataTree) GetNextItem]
	while {$node != ""} {
		set class [$node GetClassName]

		if {$class == "vtkMrmlVolumeNode"} {

			$node SetFilePrefix [MainFileGetRelativePrefix \
				[file join $oldRoot [$node GetFilePrefix]]]
			$node SetFullPrefix [file join $Mrml(dir) \
				[file join $oldRoot [$node GetFilePrefix]]]

		} elseif {$class == "vtkMrmlModelNode"} {

			set ext [file extension [$node GetFileName]]
			$node SetFileName [MainFileGetRelativePrefix \
				[file join $oldRoot [$node GetFileName]]]$ext
			$node SetFullFileName [file join $Mrml(dir) \
				[file join $oldRoot [$node GetFileName]]]$ext

		}
		set node [Mrml(dataTree) GetNextItem]
	}
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlWrite
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainMrmlWrite {filename} {
	global Mrml

	# Store the new root and filePrefix
	set oldRoot $Mrml(dir)
	MainMrmlSetFile $filename

	# Rename all file with relative path names
	MainMrmlRelativity $oldRoot

	# See if colors are different than the defaults
	MainMrmlCheckColors

	# If colors have changed since last save, then save colors too
	if {$Mrml(colorsUnsaved) == 1} {
		puts SaveColors

		# Combine trees
		vtkMrmlTree tree

		# Data tree
		Mrml(dataTree) InitTraversal
		set node [Mrml(dataTree) GetNextItem]
		while {$node != ""} {
			tree AddItem $node
			set node [Mrml(dataTree) GetNextItem]
		}

		# Color tree
		Mrml(colorTree) InitTraversal
		set node [Mrml(colorTree) GetNextItem]
		while {$node != ""} {
			tree AddItem $node
			set node [Mrml(colorTree) GetNextItem]
		}

		tree Write $filename
		tree RemoveAllItems
		tree Delete
	} else {
		Mrml(dataTree) Write $filename
	}
	
	# Colors don't need saving now
	set Mrml(colorsUnsaved) 0

}

