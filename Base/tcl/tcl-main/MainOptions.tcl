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
# FILE:        MainOptions.tcl
# DATE:        02/22/2000 11:11
# PROCEDURES:  
#   MainOptionsInit
#   MainOptionsUpdateMRML
#   MainOptionsBuildVTK
#   MainOptionsCreate
#   MainOptionsDelete
#   MainOptionsSetActive
#   MainOptionsParsePresets
#   MainOptionsUseDefaultPresets
#   MainOptionsParseDefaults
#   MainOptionsUnparsePresets
#   MainOptionsPreset
#   MainOptionsPresetCallback
#   MainOptionsRecallPresets
#   MainOptionsStorePresets
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MainOptionsInit
# .END
#-------------------------------------------------------------------------------
proc MainOptionsInit {} {
	global Module Options Preset

	# Define Procedures
	lappend Module(procVTK)  MainOptionsBuildVTK

	# Append widgets to list that gets refreshed during UpdateMRML
	set Options(mbActiveList) ""
	set Options(mActiveList)  ""

	set Options(activeID) ""
	set Options(freeze) ""

        # Set version info
        lappend Module(versions) [ParseCVSInfo MainOptions \
		{$Revision: 1.10 $} {$Date: 2000/02/22 16:30:11 $}]

	# Props
	set Options(program) "slicer"
	set Options(contents) ""
#	set Options(options) ""

	set Options(moduleList) "ordered='$Module(idList)'\nsuppressed='$Module(supList)'"

        set Preset(userOptions) 0
        set Preset(idList) "0 1 2 3"
	foreach p $Preset(idList) {
		set Preset($p,state) Release
	}
	
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc MainOptionsUpdateMRML {} {
	global Options

	# Build any new nodes
	#--------------------------------------------------------
	foreach t $Options(idList) {
		if {[MainOptionsCreate $t] == 1} {
			# Success
		}
	}    

	# Delete any old nodes
	#--------------------------------------------------------
	foreach t $Options(idListDelete) {
		if {[MainOptionsDelete $t] == 1} {
			# Success
		}
	}
	# Did we delete the active node?
	if {[lsearch $Options(idList) $Options(activeID)] == -1} {
		MainOptionsSetActive [lindex $Options(idList) 0]
	}

	# Form the menu
	#--------------------------------------------------------
	foreach m $Options(mActiveList) {
		$m delete 0 end
		foreach t $Options(idList) {
			$m add command -label [Options($t,node) GetContents] \
				-command "MainOptionsSetActive $t"
		}
	}

	# In case we changed the name of the active transform
	MainOptionsSetActive $Options(activeID)
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsBuildVTK
# .END
#-------------------------------------------------------------------------------
proc MainOptionsBuildVTK {} {
	global Options

}

#-------------------------------------------------------------------------------
# .PROC MainOptionsCreate
#
# Returns:
#  1 - success
#  0 - already built this Option
# -1 - failed to read files
# .END
#-------------------------------------------------------------------------------
proc MainOptionsCreate {t} {
	global Options

	# If we've already built this volume, then do nothing
	if {[info exists Options($t,created)] == 1} {
		return 0
	}
	set Options($t,created) 1

	return 1
}


#-------------------------------------------------------------------------------
# .PROC MainOptionsDelete
#
# Returns:
#  1 - success
#  0 - already deleted this Option
# .END
#-------------------------------------------------------------------------------
proc MainOptionsDelete {t} {
	global Options

	# If we've already deleted this transform, then return 0
	if {[info exists Options($t,created)] == 0} {
		return 0
	}

	# Delete VTK objects (and remove commands from TCL namespace)

	# Delete all TCL variables of the form: Options($t,<whatever>)
	foreach name [array names Options] {
		if {[string first "$t," $name] == 0} {
			unset Options($name)
		}
	}

	return 1
}


#-------------------------------------------------------------------------------
# .PROC MainOptionsSetActive
# .END
#-------------------------------------------------------------------------------
proc MainOptionsSetActive {t} {
	global Options

	if {$Options(freeze) == 1} {return}
	
	# Set activeID to t
	set Options(activeID) $t

	if {$t == ""} {
		# Change button text
		foreach mb $Options(mbActiveList) {
			$mb config -text None
		}
		return
	} elseif {$t == "NEW"} {
		# Change button text
		foreach mb $Options(mbActiveList) {
			$mb config -text "NEW"
		}
		# Use defaults to update GUI
		vtkMrmlOptionsNode default
		set Options(program)  "slicer"
		set Options(contents) "presets"
#		set Options(options)  ""
		default Delete
	} else {
		# Change button text
		foreach mb $Options(mbActiveList) {
			$mb config -text [Options($t,node) GetContents]
		}
		# Update GUI
		set Options(program)  [Options($t,node) GetProgram]
		set Options(contents) [Options($t,node) GetContents]
#		set Options(options)  [Options($t,node) GetOptions]
	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsParsePresets
# .END
#-------------------------------------------------------------------------------
proc MainOptionsParsePresets {attr} {
	global Preset
	
	foreach a $attr {
		set key [lindex $a 0]
		set val [lreplace $a 0 0]

		set Preset($key) $val
	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsUseDefaultPresets
# .END
#-------------------------------------------------------------------------------
proc MainOptionsUseDefaultPresets {} {
	global Module Preset

	foreach m $Module(idList) {
		if {[info exists Module($m,presets)] == 1} {
			foreach p "$Preset(idList)" {
				foreach key $Preset($m,keys) {
					set Preset($m,$p,$key) $Preset($m,default,$key)
				}
			}
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsParseDefaults
#  Parses the default presets string provided by each module in its Init.
#  Initializes all presets (including "default" preset) to these defaults
# .END
#-------------------------------------------------------------------------------
proc MainOptionsParseDefaults {m} {
	global Module Preset
	
	set Preset($m,keys) ""
	set attr $Module($m,presets)

	# Strip leading white space
	regsub "^\[\n\t \]*" $attr "" attr

	while {$attr != ""} {
		
		# Find the next key=value pair (and also strip it off... all in one step!)
		if {[regexp "^(\[^=\]*)\[\n\t \]*=\[\n\t \]*\['\"\](\[^'\"\]*)\['\"\](.*)$" \
			$attr match key value attr] == 0} {
			set errmsg "Can't parse attributes:\n$attr"
			puts "$errmsg"
			tk_messageBox -message "$errmsg"
			return
		}
			
		foreach p "$Preset(idList) default" {
			set Preset($m,$p,$key) $value
		}
		lappend Preset($m,keys) $key

		# Strip leading white space
		regsub "^\[\n\t \]*" $attr "" attr
	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsUnparsePresets
# Puts presets into the XML format.
# Makes an Options node and adds it to data tree.
# .END
#-------------------------------------------------------------------------------
proc MainOptionsUnparsePresets {{presetNum ""}} {
	global Preset Mrml Options Module Model
	
	# Store current settings as preset #0 (userOptions)
	set Preset($Preset(userOptions),state) Press
	MainOptionsPresetCallback $Preset(userOptions)
	set Preset($Preset(userOptions),state) Release

	# Build an option string of attributes that differ from defaults
	
	# Foreach module, foreach preset button, foreach key, 
	# does the value differ from the default?
	#
	set options ""
	foreach m $Module(idList) {
		if {[info exists Preset($m,keys)] == 1} {
			set wrote 0
			foreach key $Preset($m,keys) {
				foreach p $Preset(idList) {
					set name "$m,$p,$key"
					if {$Preset($name) != $Preset($m,default,$key)} {
						set wrote 1
						set options "$options $name='$Preset($name)'"
					}
				}
			}
			if {$wrote == 1} {
				set options "$options\n"
			}
		}
	}

	# Models are a special case since the keys are not known ahead of time.
	# Use the current settings as the "defaults" to see if it is necessary
	# to save the presets.
	#
	set wrote 0
	foreach m $Model(idList) {
		foreach key "visibility opacity clipping backfaceCulling" {
			foreach p $Preset(idList) {
				set name "Models,$p,$m,$key"
				# Careful: if the user never clicked the button, then the preset
				# doesn't exist.
				if {[info exists Preset($name)] == 1} {
					# I could do a comparison with a default, but naahhh.
					set wrote 1
					set options "$options $name='$Preset($name)'"
				}
			}
		}	
	}
	if {$wrote == 1} {
		set options "$options\n"
	}


	# If we are saving user options in Options.xml, don't add node to tree.
	if {$presetNum == $Preset(userOptions)} {
	    return $options
	}

	# If a preset options node exists, edit it, else create one.
	set tree Mrml(dataTree)
	$tree InitTraversal
	set node [$tree GetNextItem]
        set found 0
	while {$node != ""} {
		set class [$node GetClassName]
		if {$class == "vtkMrmlOptionsNode"} {
			if {[$node GetContents] == "presets" && $found == 0} {
				set found 1
				$node SetOptions $options
			}
		}
		set node [$tree GetNextItem]
	}

	if {$found == 0} {
		# Create a node
	        set n [MainMrmlAddNode Options]
		$n SetOptions $options
		$n SetProgram slicer
		$n SetContents presets
	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsPreset
# .END
#-------------------------------------------------------------------------------
proc MainOptionsPreset {p state} {
	global View Gui Preset

	if {$state == "Press"} {
		set Preset($p,state) $state
		after 250 "MainOptionsPresetCallback $p; RenderAll"
	} else {
		set Preset($p,state) $state
		$View(fPreset).c$p config -activebackground $Gui(activeButton) 
	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsPresetCallback
# .END
#-------------------------------------------------------------------------------
proc MainOptionsPresetCallback {p} {
    global View Target Gui Preset Slice Locator Anno Model Options Module
    
    if {$Preset($p,state) == "Press"} {
	
	# Change button to red
	if {$p != $Preset(userOptions)} {
	    $View(fPreset).c$p config -activebackground red
	}
	
	# Set preset value to the current
	MainOptionsStorePresets $p
	
    } else {
	
	# Set current to the preset value
	MainOptionsRecallPresets $p
    }
}


#-------------------------------------------------------------------------------
# .PROC MainOptionsRecallPresets
# .END
#-------------------------------------------------------------------------------
proc MainOptionsRecallPresets {p} {
    global Module

    # Set current to the preset value
    foreach m $Module(procRecallPresets) {
	$m $p
    }
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsStorePresets
# .END
#-------------------------------------------------------------------------------
proc MainOptionsStorePresets {p} {
    global Module
    
    # Set preset value to the current
    foreach m $Module(procStorePresets) {
	$m $p
    }
}