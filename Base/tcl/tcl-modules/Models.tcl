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
# FILE:        Models.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   ModelsInit
#   ModelsUpdateMRML
#   ModelsBuildGUI
#   ModelsEnter
#   ModelsSetVolume
#   ModelsCreate
#   ModelsLabelCallback
#   ModelsSmoothWrapper
#   ModelsSmooth
#   ModelsMarch
#   ModelsMeter
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC ModelsInit
# .END
#-------------------------------------------------------------------------------
proc ModelsInit {} {
	global Model Module

	# Define Tabs
	set m Models
	set Module($m,row1List) "Help Options Clip Create"
	set Module($m,row1Name) "Help Options Clip Create "
	set Module($m,row1,tab) Options
	set Module($m,row2List) "Edit Meter"
	set Module($m,row2Name) "Edit Meter"
	set Module($m,row2,tab) Edit

	# Define Procedures
	set Module($m,procGUI) ModelsBuildGUI
	set Module($m,procMRML) ModelsUpdateMRML
	set Module($m,procEnter) ModelsEnter

	# Create
	set Model(idVolume) 0
	set Model(create,name) skin
	set Model(create,smooth) 5
	set Model(create,decimate) 1
	set Model(marching) 0

	# Edit
	set Model(edit,smooth) 5
	set Model(fileName) skin

	# Meter
	set Model(meter,first) 1
}

#-------------------------------------------------------------------------------
# .PROC ModelsUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc ModelsUpdateMRML {} {
	global Gui Model Slice Module Color Volume Label

	# Create the GUI for any new models
	foreach m $Model(idList) {
		MainModelsCreateGUI $Module(Models,fOptions).fGrid $m
	}

	# Delete the GUI for any old models
	foreach m $Model(idListDelete) {
		MainModelsDeleteGUI $Module(Models,fOptions).fGrid $m
	}

	# Refresh the GUI for all models (in case color changed)
	foreach m $Model(idList) {
		set c $Model($m,colorID)
		$Module(Models,fOptions).fGrid.s$m config \
			-troughcolor [MakeColorNormalized $Color($c,diffuseColor)]
	}

	# Volume menu
	#---------------------------------------------------------------------------
	set m $Model(mVolume)
	$m delete 0 end
	foreach v $Volume(idListForMenu) {
		$m add command -label [Volume($v,node) GetName] -command \
			"ModelsSetVolume $v"
	}

}

#-------------------------------------------------------------------------------
# .PROC ModelsBuildGUI
# .END
#-------------------------------------------------------------------------------
proc ModelsBuildGUI {} {
	global Gui Model Slice Module Label

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Options
	#   Title
	#   All
	#   Grid
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
	MainHelpApplyTags Models $help
	MainHelpBuildGUI Models

	#-------------------------------------------
	# Options frame
	#-------------------------------------------
	set fOptions $Module(Models,fOptions)
	set f $fOptions

	frame $f.fTitle -bg $Gui(activeWorkspace)
	frame $f.fAll -bg $Gui(activeWorkspace)
	frame $f.fGrid -bg $Gui(activeWorkspace)
	pack $f.fTitle $f.fAll $f.fGrid -side top -pady $Gui(pad)

	#-------------------------------------------
	# fOptions->Title frame
	#-------------------------------------------
	set f $fOptions.fTitle

	set c {label $f.lTitle -justify left -text \
		"Click the right mouse button on\nthe name of a model for options." $Gui(WLA)}
		eval [subst $c]
	pack $f.lTitle

	#-------------------------------------------
	# fOptions->All frame
	#-------------------------------------------
	set f $fOptions.fAll

	set c {button $f.bAll -text "Show All" -width 10 \
		-command "MainModelsSetVisibility All; Render3D" $Gui(WBA)}; eval [subst $c]
	set c {button $f.bNone -text "Show None" -width 10 \
		-command "MainModelsSetVisibility None; Render3D" $Gui(WBA)};
	eval [subst $c]
	pack $f.bAll $f.bNone -side left -padx $Gui(pad) -pady 0

	#-------------------------------------------
	# fOptions->Grid frame
	#-------------------------------------------
	# Done in MainModelsRefreshGUI


	#-------------------------------------------
	# Clip frame
	#-------------------------------------------
	set fClip $Module(Models,fClip)
	set f $fClip

	frame $f.fHelp -bg $Gui(activeWorkspace)
	frame $f.fGrid -bg $Gui(activeWorkspace)
	pack $f.fHelp $f.fGrid -side top -pady $Gui(pad)

	#-------------------------------------------
	# fClip->Grid frame
	#-------------------------------------------
	set f $fClip.fHelp

	set c {label $f.l  -justify left -text "The slices clip the models that\n\
		have clipping turn on.\n\n\
		To turn clipping on for a model,\n\
		check the box to the right of the\n\
		model's name on the Options page."\
		$Gui(WLA)}; eval [subst $c]
	pack $f.l

	#-------------------------------------------
	# fClip->Grid frame
	#-------------------------------------------
	set f $fClip.fGrid
	
	foreach s $Slice(idList) name "Red Yellow Green" {

		set c {label $f.l$s -text "$name Slice: " $Gui(WLA)}; eval [subst $c]
		
		frame $f.f$s -bg $Gui(activeWorkspace)
		foreach text "Off + -" value "0 1 2" width "4 2 2" {
			set c {radiobutton $f.f$s.r$value -width $width \
				-text "$text" -value "$value" -variable Slice($s,clipState) \
				-indicatoron 0 \
				-command "MainSlicesSetClipState $s; MainModelsRefreshClip; Render3D" \
				$Gui(WCA) -bg $Gui(slice$s)}
				eval [subst $c]
			pack $f.f$s.r$value -side left -padx 0 -pady 0
		}
		grid $f.l$s $f.f$s -pady $Gui(pad)
	}


	#-------------------------------------------
	# Create frame
	#-------------------------------------------
	set fCreate $Module(Models,fCreate)
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
	set c {label $f.lVolume -text "Volume:" $Gui(WLA)}; eval [subst $c]

	set c {menubutton $f.mbVolume -text "None" -relief raised -bd 2 -width 18 \
		-menu $f.mbVolume.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbVolume.m $Gui(WMA)}; eval [subst $c]
	pack $f.lVolume -padx $Gui(pad) -side left -anchor e
	pack $f.mbVolume -padx $Gui(pad) -side left -anchor w

	# Save widgets for changing
	set Model(mbVolume) $f.mbVolume
	set Model(mVolume)  $f.mbVolume.m

	#-------------------------------------------
	# Create->Label frame
	#-------------------------------------------
	set f $fCreate.fLabel

	set c {button $f.bLabel -text "Label:" \
		-command "ShowLabels ModelsLabelCallback" $Gui(WBA)}; eval [subst $c]
	set c {entry $f.eLabel -width 6 \
		-textvariable Label(label) $Gui(WEA)}; eval [subst $c]
	bind $f.eLabel <Return>   "LabelsFindLabel; ModelsLabelCallback"
	bind $f.eLabel <FocusOut> "LabelsFindLabel; ModelsLabelCallback"
	set c {entry $f.eName -width 13 \
		-textvariable Label(name) $Gui(WEA) \
		-bg $Gui(activeWorkspace) -state disabled}; eval [subst $c]
	grid $f.bLabel $f.eLabel $f.eName -padx $Gui(pad) -pady $Gui(pad) -sticky e

	lappend Label(colorWidgetList) $f.eName

	#-------------------------------------------
	# Create->Grid frame
	#-------------------------------------------
	set f $fCreate.fGrid

	foreach Param "Name Smooth Decimate" width "13 7 7" {
		set c {label $f.l$Param -text "$Param:" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e$Param -width $width \
			-textvariable Model(create,[Uncap $Param]) $Gui(WEA)}; eval [subst $c]
		grid $f.l$Param $f.e$Param  -padx $Gui(pad) -pady $Gui(pad) -sticky e
		grid $f.e$Param -sticky w
	}

	#-------------------------------------------
	# Create->Apply frame
	#-------------------------------------------
	set f $fCreate.fApply

	set c {button $f.bCreate -text "Create" -width 7 \
		-command "ModelsCreate; Render3D" $Gui(WBA)}; eval [subst $c]
	pack $f.bCreate -side top -pady $Gui(pad)

	#-------------------------------------------
	# Create->Results frame
	#-------------------------------------------
	set f $fCreate.fResults

	set c {label $f.l -justify left -text "" $Gui(WLA)}; eval [subst $c]
	pack $f.l -side top -pady 1
	set Model(create,msg) $f.l

	#-------------------------------------------
	# Edit frame
	#-------------------------------------------
	set fEdit $Module(Models,fEdit)
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

	set c {label $f.lActive -text "Active Model: " $Gui(WLA)}; eval [subst $c]
	set c {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbActive.m $Gui(WMA)}; eval [subst $c]
	pack $f.lActive $f.mbActive -side left -padx $Gui(pad) -pady 0 

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Model(mbActiveList) $f.mbActive
	lappend Model(mActiveList)  $f.mbActive.m

	#-------------------------------------------
	# Edit->Grid frame
	#-------------------------------------------
	set f $fEdit.fGrid

	set c {label $f.lTitle -text "Apply an Effect" $Gui(WTA)}
		eval [subst $c]
	grid $f.lTitle -columnspan 3 -pady $Gui(pad)

	set Param Smooth
	set c {label $f.l$Param -text "$Param:" $Gui(WLA)}; eval [subst $c]
	set c {entry $f.e$Param -width 7 \
		-textvariable Model(edit,[Uncap $Param]) $Gui(WEA)}; eval [subst $c]
	set c {button $f.bSmooth -text "Smooth" -width 7 \
		-command "ModelsSmoothWrapper; Render3D" $Gui(WBA)}; eval [subst $c]
	grid $f.l$Param $f.e$Param $f.bSmooth \
		-padx $Gui(pad) -pady $Gui(pad) -sticky e
	grid $f.e$Param -sticky w

	#-------------------------------------------
	# Edit->Write frame
	#-------------------------------------------
	set f $fEdit.fWrite

	set c {label $f.lTitle -text "Save model as a VTK file" $Gui(WTA)}
		eval [subst $c]
	frame $f.fPrefix -bg $Gui(activeWorkspace)
	frame $f.fBtns   -bg $Gui(activeWorkspace)
	pack $f.lTitle   -side top -pady $Gui(pad)
	pack $f.fPrefix  -side top -pady $Gui(pad) -fill x
	pack $f.fBtns    -side top -pady $Gui(pad)

	#-------------------------------------------
	# Edit->Write->Prefix frame
	#-------------------------------------------
	set f $fEdit.fWrite.fPrefix

	set c {label $f.lFile -text "Prefix:" $Gui(WLA)}; eval [subst $c]
	set c {entry $f.eFile -textvariable Model(fileName) -width 50 $Gui(WEA)}
		eval [subst $c]
	pack $f.lFile -side left -padx $Gui(pad)
	pack $f.eFile -side left -padx $Gui(pad) -expand 1 -fill x

	#-------------------------------------------
	# Edit->Write->Btns frame
	#-------------------------------------------
	set f $fEdit.fWrite.fBtns

	set c {button $f.bWrite -text "Write" -width 6 \
		-command "MainModelsWrite; Render3D" $Gui(WBA)}; eval [subst $c]
	pack $f.bWrite


	#-------------------------------------------
	# Meter frame
	#-------------------------------------------
	set fMeter $Module(Models,fMeter)
	set f $fMeter

	foreach frm "Apply Results" {
		frame $f.f$frm -bg $Gui(activeWorkspace)
		pack  $f.f$frm -side top -pady $Gui(pad)
	}

	#-------------------------------------------
	# Meter->Apply frame
	#-------------------------------------------
	set f $fMeter.fApply

	set text "Measure Performance"
	set c {button $f.bMeasure -text "$text" \
		-width [expr [string length $text] + 1] \
		-command "ModelsMeter" $Gui(WBA)}; eval [subst $c]
	pack $f.bMeasure

	#-------------------------------------------
	# Meter->Results frame
	#-------------------------------------------
	set f $fMeter.fResults

	frame $f.fTop -bg $Gui(activeWorkspace)
	frame $f.fBot -bg $Gui(activeWorkspace)
	pack $f.fTop $f.fBot -side top -pady $Gui(pad)

	set f $fMeter.fResults.fTop
	set c {label $f.l -justify left -text "" $Gui(WLA)}; eval [subst $c]
	pack $f.l
	set Model(meter,msgTop) $f.l

	set f $fMeter.fResults.fBot
	set c {label $f.lL -justify left -text "" $Gui(WLA)}; eval [subst $c]
	set c {label $f.lR -justify right -text "" $Gui(WLA)}; eval [subst $c]
	pack $f.lL $f.lR -side left -padx $Gui(pad)
	set Model(meter,msgLeft) $f.lL
	set Model(meter,msgRight) $f.lR

}

#-------------------------------------------------------------------------------
# .PROC ModelsEnter
# .END
#-------------------------------------------------------------------------------
proc ModelsEnter {} {
	global Volume
	
	ModelsSetVolume $Volume(activeID)
}

#-------------------------------------------------------------------------------
# .PROC ModelsSetVolume
# .END
#-------------------------------------------------------------------------------
proc ModelsSetVolume {v} {
	global Model Volume Label

	set Model(idVolume) $v
	
	# Change button text
    $Model(mbVolume) config -text [Volume($v,node) GetName]

	# Initialize the label to the highest value in the volume
	set Label(label) [Volume($v,vol) GetRangeHigh]
	LabelsFindLabel
	ModelsLabelCallback
}

#-------------------------------------------------------------------------------
# .PROC ModelsCreate
# .END
#-------------------------------------------------------------------------------
proc ModelsCreate {} {
	global Model Label

	set m [MainModelsCreateUnreadable]
	set v $Model(idVolume)
	Model($m,node) SetName $Model(create,name)
	Model($m,node) SetColor $Label(name)

	# Registration
	Model($m,node) SetRasToWld [Volume($m,node) GetRasToWld]

	if {[ModelsMarch $m $v $Model(create,decimate) $Model(create,smooth)] != 0} {
		MainModelsDelete $m
		return
	}
	$Model(create,msg) config -text "\
Marching cubes: $Model(t,mcubes) sec.\n\
Decimate: $Model(t,decimator) sec.\n\
Smooth: $Model(t,smoother) sec.\n\
$Model(n,mcubes) polygons reduced to $Model(n,decimator)."

	MainUpdateMRML
	MainModelsSetActive $m
}

#-------------------------------------------------------------------------------
# .PROC ModelsLabelCallback
# .END
#-------------------------------------------------------------------------------
proc ModelsLabelCallback {} {
	global Label Model Gui

	set Model(create,name) $Label(name)
	set Model(fileName) $Label(name)
}

#-------------------------------------------------------------------------------
# .PROC ModelsSmoothWrapper
# .END
#-------------------------------------------------------------------------------
proc ModelsSmoothWrapper {{m ""}} {
	global Model

	if {$m == ""} {
		set m $Model(activeID)
	}
	if {$m < 0} {return}

	ModelsSmooth $m $Model(edit,smooth)
}

#-------------------------------------------------------------------------------
# .PROC ModelsSmooth
# .END
#-------------------------------------------------------------------------------
proc ModelsSmooth {m iterations} {
	global Model Gui

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
	set Model(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
	set Model(n,$p) [[$p GetOutput] GetNumberOfPolys]
	set Model($m,nPolys) $Model(n,$p)

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
# .PROC ModelsMarch
# .END
#-------------------------------------------------------------------------------
proc ModelsMarch {m v decimateIterations smoothIterations} {
	global Model Gui
	
	if {$Model(marching) == 1} {
		puts "already marching"
		return -1
	}

	set Model(marching) 1
	set name [Model($m,node) GetName]

	# Marching cubes cannot run on data of dimension less than 3
	set dim [[Volume($v,vol) GetOutput] GetExtent]
	if {[lindex $dim 0] == [lindex $dim 1] ||
	    [lindex $dim 2] == [lindex $dim 3] ||
	    [lindex $dim 4] == [lindex $dim 5]} {
		puts "extent=$dim"
		tk_messageBox -message "The volume '[Volume($v,node) GetName]' is not 3D"
		set Model(marching) 0
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
	set Model(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
	set Model(n,$p) [[$p GetOutput] GetNumberOfPolys]

	# If there are no polygons, then the smoother gets mad, so stop.
	if {$Model(n,$p) == 0} {
		tk_messageBox -message "No polygons can be created."
		to SetInput ""
		mcubes SetInput ""
		flip SetInput ""
		to Delete
		mcubes Delete
		flip Delete
		rot Delete
		set Model(marching) 0
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
	set Model(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
	set Model(n,$p) [[$p GetOutput] GetNumberOfPolys]
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
	set Model(t,$p) [expr [lindex [time {$p Update}] 0]/1000000.0]
	set Model(n,$p) [[$p GetOutput] GetNumberOfPolys]
	set Model($m,nPolys) $Model(n,$p)

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

	set Model(marching) 0
	return 0
}

#-------------------------------------------------------------------------------
# .PROC ModelsMeter
# .END
#-------------------------------------------------------------------------------
proc ModelsMeter {} {
	global Model

	# Count the polygons in each model
	set total 0
	set msgLeft ""
	set msgRight ""
	foreach m $Model(idList) {

		if {[info exists Model($m,nPolys)] == 0} {

			vtkTriangleFilter triangle
			triangle SetInput $Model($m,polyData)
			[triangle GetOutput] ReleaseDataFlagOn
			triangle Update
			set Model($m,nPolys) [[triangle GetOutput] GetNumberOfPolys]

			vtkStripper stripper
			stripper SetInput [triangle GetOutput]
			[stripper GetOutput] ReleaseDataFlagOff

			# polyData will survive as long as it's the input to the mapper
			set Model($m,polyData) [stripper GetOutput]
			$Model($m,polyData) Update
			Model($m,mapper) SetInput $Model($m,polyData)

			stripper SetOutput ""
			foreach p "triangle stripper" {
				$p SetInput ""
				$p Delete
			}
		}
#		puts "m=$m: ref=[$Model($m,polyData) GetReferenceCount]"

		set n $Model($m,nPolys)
#		set Model($m,oldVis) [Model($m,node) GetVisibility]
#		if {$Model($m,oldVis) == 1} {
#			set total [expr $total + $n]
#		}
		if {[Model($m,node) GetVisibility] == 1} {
			set total [expr $total + $n]
		}
		set msgLeft "$msgLeft\n[Model($m,node) GetName]"
		set msgRight "$msgRight\n$n"
	}

	# Without
#	MainModelsSetVisibility None
#	set tWithout [lindex [time {Render3D}] 0]

	# With
#	foreach m $Model(idList) {
#		MainModelsSetVisibility $m $Model($m,oldVis)
#	}
	set tWith [lindex [time {Render3D}] 0]

	# Compute rate
#	set dif [expr $tWith - $tWithout]
	set dif $tWith
	if {$dif > 0} {
		set rate [expr $total / ($dif/1000000.0)]
	} else {
		set rate 0
	}

	set msgTop "\
Total visible polygons: $total\n\
Render time: [format "%.3f" [expr $tWith/1000000.0]]\n\
Polygons/sec rendered: [format "%.0f" $rate]"

	$Model(meter,msgTop) config -text $msgTop
	$Model(meter,msgLeft) config -text $msgLeft
	$Model(meter,msgRight) config -text $msgRight

	if {$Model(meter,first) == 1} {
		set Model(meter,first) 0
		ModelsMeter
	}
}
