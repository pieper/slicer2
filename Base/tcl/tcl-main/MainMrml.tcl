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
# FILE:        MainMrml.tcl
# DATE:        12/10/1999 08:40
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
	global Model Volume

	set Volume(idListDelete) ""  
	set Model(idListDelete) ""
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlDeleteModel
# .END
#-------------------------------------------------------------------------------
proc MainMrmlDeleteModel {m} {
	global Model Volume

	set Volume(idListDelete) ""
	set Model(idListDelete) $m

	set i [lsearch $Model(idList) $m]
	set Model(idList) [lreplace $Model(idList) $i $i]

	MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC MainMrmlRead
# .END
#-------------------------------------------------------------------------------
proc MainMrmlRead {mrmlFile} {
	global Lut Dag Volume Model Config Color Gui env

	# Read MRML defaults file
	set fileName [ExpandPath "Defaults.mrml"]
	if {[CheckFileExists $fileName] == "0"} {
		set msg "Unable to read file MRML defaults file '$fileName'"
		puts $msg
		tk_messageBox -message $msg
		exit	
	}
	
	MRMLReadDefaults $fileName

	# Determine name of MRML input file.
	# Append ".mrml" if necessary.
	set fileName $mrmlFile

	if {$fileName == ""} {
		set Dag(read) [MRMLCreateDag]
	} else {
		if {[GetSuffix $fileName] != "mrml"} {
			set fileName "$fileName.mrml"
		}
		# Read MRML input file
		if {[CheckFileExists $fileName] == "0"} {
			set msg "Unable to read input MRML file '$fileName'"
			puts $msg
			tk_messageBox -message $msg
			exit	
		}
		set Dag(read) [MRMLRead $fileName]
		if {$Dag(read) == "-1"} {
			tk_messageBox -message "Error reading MRML file: '$fileName'\n\
				See message written in console." 
			exit
		}
	}

	# Store directory name
	set dir [file dirname $fileName]
	if {$dir == "" || $dir == "."} {
		set Gui(root) [pwd]
	}
	set Gui(root) $dir

	# Expand URLs
	set Dag(expanded) [MRMLExpandUrls $Dag(read) $Gui(root)]

	# If there are no Color nodes, then read, expand, append default colors.
	set n [MRMLCountTypeOfNode $Dag(expanded) Color]
	if {$n == 0} {
		set fileName [ExpandPath Colors.mrml]
		if {[CheckFileExists $fileName] == "0"} {
			set msg "Unable to read file default MRML color file '$fileName'"
			puts $msg
			tk_messageBox -message $msg
			exit	
		}
		set Dag(defaultColors) [MRMLRead $fileName]
		if {$Dag(defaultColors) == "-1"} {
			tk_messageBox -message "Error reading MRML file: '$fileName'\n\
				See message written in console." 
			exit
		}
		set Dag(defaultColors) [MRMLExpandUrls $Dag(defaultColors) [file dirname $fileName]]
		set Dag(withColors) [MRMLAppendDag $Dag(expanded) $Dag(defaultColors)]
	} else {
		set Dag(withColors) $Dag(expanded)
	}

	# Make a dag of only color nodes
	MainColorsBuildDag $Dag(withColors)

	# Create a dag of built-in volumes 0 for result, working, none, realtime.
	# Insert them inside a separator so they don't affect the rest
	# of a dag.
	set Dag(builtIn) [MRMLCreateDag]
	set Dag(builtIn) [MRMLAppendNode $Dag(builtIn) "Separator"]

	# None Volume
	set node [MRMLCreateNode "Volume"]
	set node [MRMLSetValue $node name "None"]
	set node [MRMLSetValue $node imageRange "1 1"] 
	set node [MRMLSetValue $node dimensions "64 64"] 
	set node [MRMLSetValue $node spacing "1 1 1"]
	set node [MRMLSetValue $node headerSize 0]
	set node [MRMLSetValue $node filePrefix none] 
	set node [MRMLSetValue $node filePattern %s.%03d]
	set Dag(builtIn) [MRMLAppendNode $Dag(builtIn) $node]

	# Add default color (id = -1)
	set node [MRMLCreateNode "Color"]
	set node [MRMLSetValue $node "name" "None"]
	set node [MRMLAddAttribute $node "id" "-1"]
	set Dag(builtIn) [MRMLAppendNode $Dag(builtIn) $node]
	set Dag(builtIn) [MRMLAppendNode $Dag(builtIn) "End"]

	# Insert BuiltInDag before dag
	set Dag(computed) [MRMLInsertDag $Dag(withColors) $Dag(builtIn) 0]

	# Compute geometric relationships between nodes
	# Note: last number passed to the procedure is the first volume ID
	set Dag(computed) [MRMLComputeRelationships $Dag(computed) \
		[expr 1 - $Volume(numBuiltIn)]]

	set Volume(idList) ""
	MRMLParseDagIntoArray $Dag(computed) Volume "Volume" "slicer" \
		$Volume(defaultOptions)

	set Model(idList) ""
	MRMLParseDagIntoArray $Dag(computed) Model "Model"

	set Config(idList) ""
	MRMLParseDagIntoArray $Dag(computed) Config "Config"

	set Color(idList) ""
	MRMLParseDagIntoArray $Dag(computed) Color "Color"

	# Make a computed dag without the built-in nodes.
	# Delete the first 2+numBuiltIn nodes (Sep, numBuiltIn Vols, End)
	set num [expr 2 + $Volume(numBuiltIn)]
	set dag $Dag(computed)
	for {set i 0} {$i < $num} {incr i} {
		set node [MRMLGetNode $dag 0]
		set dag [MRMLDeleteNode $dag 0]
	}
	set Dag(computedWithoutBuiltIn) $dag

	# "Current" to be edited during program execution (and saved as a scene)
	set Dag(current) $dag
}

