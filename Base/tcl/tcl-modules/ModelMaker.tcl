
#-------------------------------------------------------------------------------
# .PROC ModelMakerInit
# .END
#-------------------------------------------------------------------------------
proc ModelMakerInit {} {
	global ModelMaker Module

	# Define Tabs
	set m ModelMaker
	set Module($m,row1List) "Help Create Edit"
	set Module($m,row1Name) "{Help} {Create} {Edit} "
	set Module($m,row1,tab) Create

	# Define Procedures
	set Module($m,procGUI) ModelMakerBuildGUI
	set Module($m,procMRML) ModelMakerUpdateMRML
	set Module($m,procEnter) ModelMakerEnter

	# Create
	set ModelMaker(idVolume) 0
	set ModelMaker(name) skin
	set ModelMaker(smooth) 5
	set ModelMaker(decimate) 1
	set ModelMaker(marching) 0

	# Edit
	set ModelMaker(edit,smooth) 5
	set ModelMaker(prefix) ""

}

#-------------------------------------------------------------------------------
# .PROC ModelMakerUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc ModelMakerUpdateMRML {} {
	global ModelMaker Volume

	# Volume menu
	#---------------------------------------------------------------------------
	set m $ModelMaker(mVolume)
	$m delete 0 end
	foreach v $Volume(idListForMenu) {
		$m add command -label [Volume($v,node) GetName] -command \
			"ModelMakerSetVolume $v"
	}

}

#-------------------------------------------------------------------------------
# .PROC ModelMakerBuildGUI
# .END
#-------------------------------------------------------------------------------
proc ModelMakerBuildGUI {} {
	global Gui ModelMaker Model Module Label

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Display
	#   Title
	#   All
	#   Grid
	# Properties
	# Clip
	#   Help
	#   Grid
	# Create
	# Edit
	#   
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Models are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags ModelMaker $help
	MainHelpBuildGUI ModelMaker

	#-------------------------------------------
	# Create frame
	#-------------------------------------------
	set fCreate $Module(ModelMaker,fCreate)
	set f $fCreate

	foreach frm " Volume Label Grid Apply Results" {
		frame $f.f$frm -bg $Gui(activeWorkspace)
		pack  $f.f$frm -side top -pady $Gui(pad)
	}

	#-------------------------------------------
	# Create->Volume frame
	#-------------------------------------------
	set f $fCreate.fVolume

	# Volume menu
	eval {label $f.lVolume -text "Volume:"} $Gui(WLA)

	eval {menubutton $f.mbVolume -text "None" -relief raised -bd 2 -width 18 \
		-menu $f.mbVolume.m} $Gui(WMBA)
	eval {menu $f.mbVolume.m} $Gui(WMA)
	pack $f.lVolume -padx $Gui(pad) -side left -anchor e
	pack $f.mbVolume -padx $Gui(pad) -side left -anchor w

	# Save widgets for changing
	set ModelMaker(mbVolume) $f.mbVolume
	set ModelMaker(mVolume)  $f.mbVolume.m

	#-------------------------------------------
	# Create->Label frame
	#-------------------------------------------
	set f $fCreate.fLabel

	eval {button $f.bLabel -text "Label:" \
		-command "ShowLabels ModelMakerLabelCallback"} $Gui(WBA)
	eval {entry $f.eLabel -width 6 \
		-textvariable Label(label)} $Gui(WEA)
	bind $f.eLabel <Return>   "LabelsFindLabel; ModelMakerLabelCallback"
	bind $f.eLabel <FocusOut> "LabelsFindLabel; ModelMakerLabelCallback"
	eval {entry $f.eName -width 13 \
		-textvariable Label(name)} $Gui(WEA) \
		{-bg $Gui(activeWorkspace) -state disabled}
	grid $f.bLabel $f.eLabel $f.eName -padx $Gui(pad) -pady $Gui(pad) -sticky e

	lappend Label(colorWidgetList) $f.eName

	#-------------------------------------------
	# Create->Grid frame
	#-------------------------------------------
	set f $fCreate.fGrid

	foreach Param "Name Smooth Decimate" width "13 7 7" {
		eval {label $f.l$Param -text "$Param:"} $Gui(WLA)
		eval {entry $f.e$Param -width $width \
			-textvariable ModelMaker([Uncap $Param])} $Gui(WEA)
		grid $f.l$Param $f.e$Param  -padx $Gui(pad) -pady $Gui(pad) -sticky e
		grid $f.e$Param -sticky w
	}

	#-------------------------------------------
	# Create->Apply frame
	#-------------------------------------------
	set f $fCreate.fApply

	eval {button $f.bCreate -text "Create" -width 7 \
		-command "ModelMakerCreate; Render3D"} $Gui(WBA)
	pack $f.bCreate -side top -pady $Gui(pad)

	#-------------------------------------------
	# Create->Results frame
	#-------------------------------------------
	set f $fCreate.fResults

	eval {label $f.l -justify left -text ""} $Gui(WLA)
	pack $f.l -side top -pady 1
	set ModelMaker(msg) $f.l

	#-------------------------------------------
	# Edit frame
	#-------------------------------------------
	set fEdit $Module(ModelMaker,fEdit)
	set f $fEdit

	frame $f.fActive -bg $Gui(activeWorkspace)
	frame $f.fGrid   -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fWrite  -bg $Gui(activeWorkspace) -relief groove -bd 3
	pack  $f.fActive $f.fGrid $f.fWrite \
		-side top -padx $Gui(pad) -pady 10 -fill x

	#-------------------------------------------
	# Edit->Active frame
	#-------------------------------------------
	set f $fEdit.fActive

	eval {label $f.lActive -text "Active Model: "} $Gui(WLA)
	eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m} $Gui(WMBA)
	eval {menu $f.mbActive.m} $Gui(WMA)
	pack $f.lActive $f.mbActive -side left -padx $Gui(pad) -pady 0 

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Model(mbActiveList) $f.mbActive
	lappend Model(mActiveList)  $f.mbActive.m

	#-------------------------------------------
	# Edit->Grid frame
	#-------------------------------------------
	set f $fEdit.fGrid

	eval {label $f.lTitle -text "Apply an Effect"} $Gui(WTA)
	grid $f.lTitle -columnspan 3 -pady $Gui(pad)

	set Param Smooth
	eval {label $f.l$Param -text "$Param:"} $Gui(WLA)
	eval {entry $f.e$Param -width 7 \
		-textvariable ModelMaker(edit,[Uncap $Param])} $Gui(WEA)
	eval {button $f.bSmooth -text "Smooth" -width 7 \
		-command "ModelMakerSmoothWrapper; Render3D"} $Gui(WBA)
	grid $f.l$Param $f.e$Param $f.bSmooth \
		-padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.e$Param -sticky w

	#-------------------------------------------
	# Edit->Write frame
	#-------------------------------------------
	set f $fEdit.fWrite

	eval {label $f.l -text "Save model as a VTK file"} $Gui(WTA)
	eval {button $f.b -text "Write" -width 6 \
		-command "MainModelsWrite; Render3D"} $Gui(WBA)
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f  -side top -pady $Gui(pad) -fill x
	pack $f.b  -side top -pady $Gui(pad)

	eval {label $f.f.l -text "Prefix:"} $Gui(WLA)
	eval {entry $f.f.e -textvariable ModelMaker(prefix) -width 50} $Gui(WEA)
	pack $f.f.l $f.f.e -side left -padx $Gui(pad) -expand 1 -fill x

}

#-------------------------------------------------------------------------------
# .PROC ModelMakerEnter
# .END
#-------------------------------------------------------------------------------
proc ModelMakerEnter {} {
	global Volume
	
	ModelMakerSetVolume $Volume(activeID)
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerSetVolume
# .END
#-------------------------------------------------------------------------------
proc ModelMakerSetVolume {v} {
	global ModelMaker Volume Label

	set ModelMaker(idVolume) $v
	
	# Change button text
    $ModelMaker(mbVolume) config -text [Volume($v,node) GetName]

	# Initialize the label to the highest value in the volume
	set Label(label) [Volume($v,vol) GetRangeHigh]
	LabelsFindLabel
	ModelMakerLabelCallback
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerCreate
# .END
#-------------------------------------------------------------------------------
proc ModelMakerCreate {} {
	global Model ModelMaker Label

	set m [MainModelsCreateUnreadable]
	set v $ModelMaker(idVolume)
	Model($m,node) SetName $ModelMaker(name)
	Model($m,node) SetColor $Label(name)

	# Registration
	Model($m,node) SetRasToWld [Volume($m,node) GetRasToWld]

	if {[ModelMakerMarch $m $v $ModelMaker(decimate) $ModelMaker(smooth)] != 0} {
		MainModelsDelete $m
		return
	}
	$ModelMaker(msg) config -text "\
Marching cubes: $ModelMaker(t,mcubes) sec.\n\
Decimate: $ModelMaker(t,decimator) sec.\n\
Smooth: $ModelMaker(t,smoother) sec.\n\
$ModelMaker(n,mcubes) polygons reduced to $ModelMaker(n,decimator)."

	MainUpdateMRML
	MainModelsSetActive $m
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerLabelCallback
# .END
#-------------------------------------------------------------------------------
proc ModelMakerLabelCallback {} {
	global Label ModelMaker

	set ModelMaker(name) $Label(name)
	set ModelMaker(prefix) $Label(name)
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerSmoothWrapper
# .END
#-------------------------------------------------------------------------------
proc ModelMakerSmoothWrapper {{m ""}} {
	global Model ModelMaker

	if {$m == ""} {
		set m $Model(activeID)
	}
	if {$m == ""} {return}

	ModelMakerSmooth $m $ModelMaker(edit,smooth)
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerSmooth
# .END
#-------------------------------------------------------------------------------
proc ModelMakerSmooth {m iterations} {
	global Model Gui ModelMaker

	set name [Model($m,node) GetName]

	set p smoother
	vtkSmoothPolyDataFilter $p
    $p SetInput $Model($m,polyData)
    $p SetNumberOfIterations $iterations
	# This next line massively rounds corners
	$p SetRelaxationFactor 0.33
    $p SetFeatureAngle 60
    $p FeatureEdgeSmoothingOff
    $p BoundarySmoothingOff
    $p SetConvergence 0
    [$p GetOutput] ReleaseDataFlagOn
	set Gui(progressText) "Smoothing $name"
	$p SetStartMethod     MainStartProgress
	$p SetProgressMethod "MainShowProgress $p"
	$p SetEndMethod       MainEndProgress
	set ModelMaker(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
	set ModelMaker(n,$p) [[$p GetOutput] GetNumberOfPolys]
	set ModelMaker($m,nPolys) $ModelMaker(n,$p)

	set p normals
	vtkPolyDataNormals $p
    $p SetInput [smoother GetOutput]
    $p SetFeatureAngle 60
    [$p GetOutput] ReleaseDataFlagOn

	set p stripper
	vtkStripper $p
    $p SetInput [normals GetOutput]
	[$p GetOutput] ReleaseDataFlagOff

	# polyData will survive as long as it's the input to the mapper
	set Model($m,polyData) [$p GetOutput]
	$Model($m,polyData) Update
	Model($m,mapper) SetInput $Model($m,polyData)

	stripper SetOutput ""
	foreach p "smoother normals stripper" {
		$p SetInput ""
		$p Delete
	}
}

#-------------------------------------------------------------------------------
# .PROC ModelMakerMarch
# .END
#-------------------------------------------------------------------------------
proc ModelMakerMarch {m v decimateIterations smoothIterations} {
	global Model ModelMaker Gui
	
	if {$ModelMaker(marching) == 1} {
		puts "already marching"
		return -1
	}

	set ModelMaker(marching) 1
	set name [Model($m,node) GetName]

	# Marching cubes cannot run on data of dimension less than 3
	set dim [[Volume($v,vol) GetOutput] GetExtent]
	if {[lindex $dim 0] == [lindex $dim 1] ||
	    [lindex $dim 2] == [lindex $dim 3] ||
	    [lindex $dim 4] == [lindex $dim 5]} {
		puts "extent=$dim"
		tk_messageBox -message "The volume '[Volume($v,node) GetName]' is not 3D"
		set ModelMaker(marching) 0
		return -1
	}

	set spacing [[Volume($v,vol) GetOutput] GetSpacing]
	set origin  [[Volume($v,vol) GetOutput] GetOrigin]
	# The spacing is accounted for in the rasToVtk transform, 
	# so we have to remove it here, or mcubes will use it.
	[Volume($v,vol) GetOutput] SetSpacing 1 1 1
	[Volume($v,vol) GetOutput] SetOrigin 0 0 0

	# Transform crap
	
	# Read orientation matrix and permute the images if necessary.
	vtkTransform rot
	set matrixList [Volume($v,node) GetRasToVtkMatrix]
	for {set row 0} { $row < 4 } {incr row} {
		for {set col 0} {$col < 4} {incr col} {
			[rot GetMatrix] SetElement $row $col \
				[lindex $matrixList [expr $row*4+$col]]
		}
	}
	[rot GetMatrix] Invert

	# Tramsform
	vtkImageFlip flip
	set mm [rot GetMatrix] 
	if {[$mm Determinant $mm] < 0} {
		for {set col 0} {$col < 4} {incr col} {
			set el [[rot GetMatrix] GetElement $col 0]
			[rot GetMatrix] SetElement $col 0 [expr -1*$el]
		}
		puts permute
		# vtkTransform permute
		# [permute GetMatrix] SetElement 0 0 -1
		# reader SetTransform permute
		flip SetInput [Volume($v,vol) GetOutput]
		set src flip
	} else {
		set src Volume($v,vol)
	}

	vtkImageToStructuredPoints to
	to SetInput [$src GetOutput]

	set p mcubes
	vtkMarchingCubes $p
	$p SetInput [to GetOutput]
	$p SetValue 0 1
	$p ComputeScalarsOff
	$p ComputeGradientsOff
	$p ComputeNormalsOff
	[$p GetOutput] ReleaseDataFlagOn
	set Gui(progressText) "Marching $name"
	$p SetStartMethod     MainStartProgress
	$p SetProgressMethod "MainShowProgress $p"
	$p SetEndMethod       MainEndProgress
	set ModelMaker(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
	set ModelMaker(n,$p) [[$p GetOutput] GetNumberOfPolys]

	# If there are no polygons, then the smoother gets mad, so stop.
	if {$ModelMaker(n,$p) == 0} {
		tk_messageBox -message "No polygons can be created."
		to SetInput ""
		mcubes SetInput ""
		flip SetInput ""
		to Delete
		mcubes Delete
		flip Delete
		rot Delete
		set ModelMaker(marching) 0
		return -1
	}

	set p decimator
	vtkDecimate $p
	$p SetInput [mcubes GetOutput]
    $p SetInitialFeatureAngle 60
    $p SetMaximumIterations $decimateIterations
    $p SetMaximumSubIterations 0
    $p PreserveEdgesOn
    $p SetMaximumError 1
    $p SetTargetReduction 1
	$p GlobalWarningDisplayOff
    $p SetInitialError .0002
    $p SetErrorIncrement .0002
	[$p GetOutput] ReleaseDataFlagOn
	set Gui(progressText) "Decimating $name"
	$p SetStartMethod     MainStartProgress
	$p SetProgressMethod "MainShowProgress $p"
	$p SetEndMethod       MainEndProgress
	set ModelMaker(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
	set ModelMaker(n,$p) [[$p GetOutput] GetNumberOfPolys]
	$p GlobalWarningDisplayOn
	
	set p smoother
	vtkSmoothPolyDataFilter $p
    $p SetInput [decimator GetOutput]
    $p SetNumberOfIterations $smoothIterations
	# This next line massively rounds corners
	$p SetRelaxationFactor 0.33
    $p SetFeatureAngle 60
    $p FeatureEdgeSmoothingOff
    $p BoundarySmoothingOff
    $p SetConvergence 0
    [$p GetOutput] ReleaseDataFlagOn
	set Gui(progressText) "Smoothing $name"
	$p SetStartMethod     MainStartProgress
	$p SetProgressMethod "MainShowProgress $p"
	$p SetEndMethod       MainEndProgress
	set ModelMaker(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
	set ModelMaker(n,$p) [[$p GetOutput] GetNumberOfPolys]
	set ModelMaker($m,nPolys) $ModelMaker(n,$p)

	set p transformer
	vtkTransformPolyDataFilter $p
	$p SetInput [smoother GetOutput]
	$p SetTransform rot
	[$p GetOutput] ReleaseDataFlagOn

	set p normals
	vtkPolyDataNormals $p
    $p SetInput [transformer GetOutput]
    $p SetFeatureAngle 60
    [$p GetOutput] ReleaseDataFlagOn

	set p stripper
	vtkStripper $p
    $p SetInput [normals GetOutput]
	[$p GetOutput] ReleaseDataFlagOff

	# polyData will survive as long as it's the input to the mapper
	set Model($m,polyData) [$p GetOutput]
	$Model($m,polyData) Update
	Model($m,mapper) SetInput $Model($m,polyData)

	stripper SetOutput ""
	foreach p "to flip mcubes decimator transformer smoother normals stripper" {
		$p SetInput ""
		$p Delete
	}
	rot Delete

	# Restore spacing
	eval [Volume($v,vol) GetOutput] SetSpacing $spacing
	eval [Volume($v,vol) GetOutput] SetOrigin $origin

	set ModelMaker(marching) 0
	return 0
}

