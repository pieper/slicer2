
#-------------------------------------------------------------------------------
# .PROC MainTransformsInit
# .END
#-------------------------------------------------------------------------------
proc MainTransformsInit {} {
	global Module Transform

	lappend Module(procVTK)  MainTransformsBuildVTK

	# Append widgets to list that gets refreshed during UpdateMRML
	set Transform(mbActiveList) ""
	set Transform(mActiveList)  ""

	set Transform(activeID) ""
	set Transform(regTranLR)  0
	set Transform(regTranPA)  0
	set Transform(regTranIS)  0
	set Transform(regRotLR)   0
	set Transform(regRotPA)   0
	set Transform(regRotIS)   0
	set Transform(rotAxis) ""
	set Transform(freeze) ""
}

#-------------------------------------------------------------------------------
# .PROC MainTransformsUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc MainTransformsUpdateMRML {} {
	global Transform

	# Build any new volumes
	#--------------------------------------------------------
	foreach t $Transform(idList) {
		if {[MainTransformsCreate $t] == 1} {
			# Success
		}
	}    

	# Delete any old volumes
	#--------------------------------------------------------
	foreach t $Transform(idListDelete) {
		if {[MainTransformsDelete $t] == 1} {
			# Success
		}
	}
	# Did we delete the active volume?
	if {[lsearch $Transform(idList) $Transform(activeID)] == -1} {
		MainTransformsSetActive [lindex $Transform(idList) 0]
	}

	# Form the menu
	#--------------------------------------------------------
	foreach m $Transform(mActiveList) {
		$m delete 0 end
		foreach t $Transform(idList) {
			$m add command -label [Transform($t,node) GetName] \
				-command "MainTransformsSetActive $t"
		}
	}

	# In case we changed the name of the active transform
	MainTransformsSetActive $Transform(activeID)
}

proc MainTransformsBuildVTK {} {
	global Transform

	vtkMatrix4x4 Transform(rotMatrix)
}

#-------------------------------------------------------------------------------
# .PROC MainTransformsCreate
#
# Returns:
#  1 - success
#  0 - already built this volume
# -1 - failed to read files
# .END
#-------------------------------------------------------------------------------
proc MainTransformsCreate {t} {
	global View Transform Gui Dag Lut

	# If we've already built this volume, then do nothing
	if {[info command Transform($t,transform)] != ""} {
		return 0
	}

	# We don't really use this, I just need to mark that it's created
	vtkTransform Transform($t,transform)

	return 1
}


#-------------------------------------------------------------------------------
# .PROC MainTransformsDelete
#
# Returns:
#  1 - success
#  0 - already deleted this volume
# .END
#-------------------------------------------------------------------------------
proc MainTransformsDelete {t} {
	global Transform

	# If we've already deleted this transform, then return 0
	if {[info command Transform($t,transform)] == ""} {
		return 0
	}

	# Delete VTK objects (and remove commands from TCL namespace)
	Transform($t,transform)  Delete

	# Delete all TCL variables of the form: Transform($t,<whatever>)
	foreach name [array names Transform] {
		if {[string first "$t," $name] == 0} {
			unset Transform($name)
		}
	}

	return 1
}


#-------------------------------------------------------------------------------
# .PROC MainTransformsSetActive
# .END
#-------------------------------------------------------------------------------
proc MainTransformsSetActive {t} {
	global Transform

	if {$Transform(freeze) == 1} {return}
	
	# Set activeID to t
	set Transform(activeID) $t

	set Transform(rotAxis) ""
	Transform(rotMatrix) Identity

	if {$t == ""} {
		return
	} elseif {$t == "NEW"} {
		# Change button text
		foreach mb $Transform(mbActiveList) {
			$mb config -text "NEW"
		}
		# Use defaults to update GUI
		vtkMrmlTransformNode default
		set Transform(name) [default GetName]
		default Delete
		set Transform(regTranLR) 0
		set Transform(regTranPA) 0
		set Transform(regTranIS) 0
	} else {
		# Change button text
		foreach mb $Transform(mbActiveList) {
			$mb config -text [Transform($t,node) GetName]
		}
		# Update GUI
		set Transform(name) [Transform($t,node) GetName]
		vtkMatrix4x4 mat
		[Transform($t,node) GetTransform] GetMatrix mat
		set Transform(regTranLR) [mat GetElement 0 3]
		set Transform(regTranPA) [mat GetElement 1 3]
		set Transform(regTranIS) [mat GetElement 2 3]
		mat Delete
	}

	# Update GUI
	foreach item "regRotLR regRotPA regRotIS" {
		set Transform($item) 0
	}
}

