
#-------------------------------------------------------------------------------
# .PROC MainMatricesInit
# .END
#-------------------------------------------------------------------------------
proc MainMatricesInit {} {
	global Module Matrix

	lappend Module(procVTK)  MainMatricesBuildVTK

	# Append widgets to list that gets refreshed during UpdateMRML
	set Matrix(mbActiveList) ""
	set Matrix(mActiveList)  ""

	set Matrix(activeID) ""
	set Matrix(regTranLR)  0
	set Matrix(regTranPA)  0
	set Matrix(regTranIS)  0
	set Matrix(regRotLR)   0
	set Matrix(regRotPA)   0
	set Matrix(regRotIS)   0
	set Matrix(rotAxis) ""
	set Matrix(freeze) ""
}

#-------------------------------------------------------------------------------
# .PROC MainMatricesUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc MainMatricesUpdateMRML {} {
	global Matrix

	# Build any new volumes
	#--------------------------------------------------------
	foreach t $Matrix(idList) {
		if {[MainMatricesCreate $t] == 1} {
			# Success
		}
	}    

	# Delete any old volumes
	#--------------------------------------------------------
	foreach t $Matrix(idListDelete) {
		if {[MainMatricesDelete $t] == 1} {
			# Success
		}
	}
	# Did we delete the active volume?
	if {[lsearch $Matrix(idList) $Matrix(activeID)] == -1} {
		MainMatricesSetActive [lindex $Matrix(idList) 0]
	}

	# Form the menu
	#--------------------------------------------------------
	foreach m $Matrix(mActiveList) {
		$m delete 0 end
		foreach t $Matrix(idList) {
			$m add command -label [Matrix($t,node) GetName] \
				-command "MainMatricesSetActive $t"
		}
	}

	# In case we changed the name of the active transform
	MainMatricesSetActive $Matrix(activeID)
}

proc MainMatricesBuildVTK {} {
	global Matrix

	vtkMatrix4x4 Matrix(rotMatrix)
}

#-------------------------------------------------------------------------------
# .PROC MainMatricesCreate
#
# Returns:
#  1 - success
#  0 - already built this volume
# -1 - failed to read files
# .END
#-------------------------------------------------------------------------------
proc MainMatricesCreate {t} {
	global View Matrix Gui Dag Lut

	# If we've already built this volume, then do nothing
	if {[info command Matrix($t,transform)] != ""} {
		return 0
	}

	# We don't really use this, I just need to mark that it's created
	vtkTransform Matrix($t,transform)

	return 1
}


#-------------------------------------------------------------------------------
# .PROC MainMatricesDelete
#
# Returns:
#  1 - success
#  0 - already deleted this volume
# .END
#-------------------------------------------------------------------------------
proc MainMatricesDelete {t} {
	global Matrix

	# If we've already deleted this transform, then return 0
	if {[info command Matrix($t,transform)] == ""} {
		return 0
	}

	# Delete VTK objects (and remove commands from TCL namespace)
	Matrix($t,transform)  Delete

	# Delete all TCL variables of the form: Matrix($t,<whatever>)
	foreach name [array names Matrix] {
		if {[string first "$t," $name] == 0} {
			unset Matrix($name)
		}
	}

	return 1
}


#-------------------------------------------------------------------------------
# .PROC MainMatricesSetActive
# .END
#-------------------------------------------------------------------------------
proc MainMatricesSetActive {t} {
	global Matrix

	if {$Matrix(freeze) == 1} {return}
	
	# Set activeID to t
	set Matrix(activeID) $t

	set Matrix(rotAxis) ""
	Matrix(rotMatrix) Identity

	if {$t == ""} {
		return
	} elseif {$t == "NEW"} {
		# Change button text
		foreach mb $Matrix(mbActiveList) {
			$mb config -text "NEW"
		}
		# Use defaults to update GUI
		vtkMrmlMatrixNode default
		set Matrix(name) [default GetName]
		default Delete
		set Matrix(regTranLR) 0
		set Matrix(regTranPA) 0
		set Matrix(regTranIS) 0
	} else {
		# Change button text
		foreach mb $Matrix(mbActiveList) {
			$mb config -text [Matrix($t,node) GetName]
		}
		# Update GUI
		set Matrix(name) [Matrix($t,node) GetName]
		set mat [[Matrix($t,node) GetTransform] GetMatrix]
		set Matrix(regTranLR) [$mat GetElement 0 3]
		set Matrix(regTranPA) [$mat GetElement 1 3]
		set Matrix(regTranIS) [$mat GetElement 2 3]
	}

	# Update GUI
	foreach item "regRotLR regRotPA regRotIS" {
		set Matrix($item) 0
	}
}

