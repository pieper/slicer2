
#-------------------------------------------------------------------------------
# .PROC TransformsInit
# .END
#-------------------------------------------------------------------------------
proc TransformsInit {} {
	global Transform Module Volume

	# Define Tabs
	set m Transforms
	set Module($m,row1List) "Help Props Manual Auto Fiducial"
	set Module($m,row1Name) "{Help} {Props} {Manual} {Auto} {Fiducial}"
	set Module($m,row1,tab) Manual

	# Define Procedures
	set Module($m,procGUI)   TransformsBuildGUI
	set Module($m,procMRML)  TransformsUpdateMRML

	# Props
	set Transform(propertyType) Basic
	set Transform(name) ""
	set Transform(desc) ""

	set Transform(autoOutput) mi-output.mrml
	set Transform(autoInput)  mi-input.pmrml
	set Transform(autoSpeed)  Fast
	set Transform(render)     All
	set Transform(pid) ""
	set Transform(volume) $Volume(idNone)
	set Transform(refVolume) $Volume(idNone)
	set Transform(allowAutoUndo) 0
	set Transform(tAuto) ""
}

proc TransformsUpdateMRML {} {
	global Transform Volume
	
	# Menu of Volumes
	# ------------------------------------
	set m $Transform(mbVolume).m
	$m delete 0 end
	# All volumes except none
	foreach v $Volume(idList) {
		if {$v != $Volume(idNone) && [Volume($v,node) GetLabelMap] == "0"} {
			$m add command -label "[Volume($v,node) GetName]" \
				-command "TransformsSetVolume $v"
		}
	}

	# Menu of Reference Volumes
	# ------------------------------------
	set m $Transform(mbRefVolume).m
	$m delete 0 end
	foreach v $Volume(idList) {
		if {$v != $Volume(idNone) && [Volume($v,node) GetLabelMap] == "0"} {
			$m add command -label "[Volume($v,node) GetName]" \
				-command "TransformsSetRefVolume $v"
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC TransformsBuildGUI
# .END
#-------------------------------------------------------------------------------
proc TransformsBuildGUI {} {
	global Gui Transform Module Volume

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Props
	#   Top
	#     Active
	#     Type
	#   Bot
	#     Basic
	#     Advanced
	# Manual
	# Auto
	# Fiducial
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Ron is nice.
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Transforms $help
	MainHelpBuildGUI  Transforms


	#-------------------------------------------
	# Props frame
	#-------------------------------------------
	set fProps $Module(Transforms,fProps)
	set f $fProps

	frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fBot -bg $Gui(activeWorkspace) -height 300
	pack $f.fTop $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Props->Bot frame
	#-------------------------------------------
	set f $fProps.fBot

	foreach type "Basic Advanced" {
		frame $f.f${type} -bg $Gui(activeWorkspace)
		place $f.f${type} -in $f -relheight 1.0 -relwidth 1.0
		set Transform(f${type}) $f.f${type}
	}
	raise $Transform(fBasic)

	#-------------------------------------------
	# Props->Top frame
	#-------------------------------------------
	set f $fProps.fTop

	frame $f.fActive -bg $Gui(backdrop)
	frame $f.fType   -bg $Gui(backdrop)
	pack $f.fActive $f.fType -side top -fill x -pady $Gui(pad) -padx $Gui(pad)

	#-------------------------------------------
	# Props->Top->Active frame
	#-------------------------------------------
	set f $fProps.fTop.fActive

	eval {label $f.lActive -text "Active Transform: "} $Gui(BLA)
	eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m} $Gui(WMBA)
	eval {menu $f.mbActive.m} $Gui(WMA)
	pack $f.lActive $f.mbActive -side left

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Transform(mbActiveList) $f.mbActive
	lappend Transform(mActiveList)  $f.mbActive.m

	#-------------------------------------------
	# Props->Top->Type frame
	#-------------------------------------------
	set f $fProps.fTop.fType

	eval {label $f.l -text "Properties:"} $Gui(BLA)
	frame $f.f -bg $Gui(backdrop)
	foreach p "Basic Advanced" {
		eval {radiobutton $f.f.r$p \
			-text "$p" -command "TransformsSetPropertyType" \
			-variable Transform(propertyType) -value $p -width 8 \
			-indicatoron 0} $Gui(WCA)
		pack $f.f.r$p -side left -padx 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

	#-------------------------------------------
	# Props->Bot->Basic frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic

	frame $f.fName    -bg $Gui(activeWorkspace)
	frame $f.fApply   -bg $Gui(activeWorkspace)
	pack $f.fName $f.fApply \
		-side top -fill x -pady $Gui(pad)

	#-------------------------------------------
	# Props->Bot->Advanced frame
	#-------------------------------------------
	set f $fProps.fBot.fAdvanced

	frame $f.fDesc    -bg $Gui(activeWorkspace)
	frame $f.fApply   -bg $Gui(activeWorkspace)
	pack $f.fDesc $f.fApply \
		-side top -fill x -pady $Gui(pad)

	#-------------------------------------------
	# Props->Bot->Basic->Name frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic.fName

	eval {label $f.l -text "Name:" } $Gui(WLA)
	eval {entry $f.e -textvariable Transform(name)} $Gui(WEA)
	pack $f.l -side left -padx $Gui(pad)
	pack $f.e -side left -padx $Gui(pad) -expand 1 -fill x

	#-------------------------------------------
	# Props->Bot->Basic->Apply frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic.fApply

	eval {button $f.bApply -text "Apply" \
		-command "TransformsPropsApply; RenderAll"} $Gui(WBA) {-width 8}
	eval {button $f.bCancel -text "Cancel" \
		-command "TransformsPropsCancel"} $Gui(WBA) {-width 8}
	grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)

	#-------------------------------------------
	# Props->Bot->Advanced->Desc frame
	#-------------------------------------------
	set f $fProps.fBot.fAdvanced.fDesc

	eval {label $f.l -text "Optional Description:"} $Gui(WLA)
	eval {entry $f.e -textvariable Transform(desc)} $Gui(WEA)
	pack $f.l -side top -padx $Gui(pad) -fill x -anchor w
	pack $f.e -side top -padx $Gui(pad) -expand 1 -fill x

	#-------------------------------------------
	# Props->Bot->Advanced->Apply frame
	#-------------------------------------------
	set f $fProps.fBot.fAdvanced.fApply

	eval {button $f.bApply -text "Apply" \
		-command "TransformsPropsApply; RenderAll"} $Gui(WBA) {-width 8}
	eval {button $f.bCancel -text "Cancel" \
		-command "TransformsPropsCancel"} $Gui(WBA) {-width 8}
	grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)



	#-------------------------------------------
	# Manual frame
	#-------------------------------------------
	set fManual $Module(Transforms,fManual)
	set f $fManual

	# Frames
	frame $f.fActive    -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fRender    -bg $Gui(activeWorkspace)
	frame $f.fTranslate -bg $Gui(activeWorkspace) -relief groove -bd 2
	frame $f.fRotate    -bg $Gui(activeWorkspace) -relief groove -bd 2
	pack $f.fActive $f.fRender  $f.fTranslate $f.fRotate\
		-side top -pady $Gui(pad) -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Manual->Active frame
	#-------------------------------------------
	set f $fManual.fActive

	set c {label $f.lActive -text "Active Transform: " $Gui(BLA)}; eval [subst $c]
	set c {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbActive.m $Gui(WMA)}; eval [subst $c]
	pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Transform(mbActiveList) $f.mbActive
	lappend Transform(mActiveList)  $f.mbActive.m

	#-------------------------------------------
	# Manual->Render frame
	#-------------------------------------------
	set f $fManual.fRender

	set modes "Active Slices All"
	set names "{1 Slice} {3 Slices} {3D}"

	set c {label $f.l -text "Render:" $Gui(WLA)}; eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	foreach mode $modes name $names {
		set c {radiobutton $f.f.r$mode -width [expr [string length $name]+1]\
			-text "$name" -variable Transform(render) -value $mode \
			-indicatoron 0 $Gui(WCA)}
			eval [subst $c]
		pack $f.f.r$mode -side left -padx 0 -pady 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

	#-------------------------------------------
	# Manual->Translate frame
	#-------------------------------------------
	set f $fManual.fTranslate

	set c {label $f.lTitle -text "Translation (mm)" $Gui(WLA)}
		eval [subst $c]
	grid $f.lTitle -columnspan 3 -pady $Gui(pad) -padx 1 

	foreach slider "LR PA IS" {

		set c {label $f.l${slider} -text "${slider} : " $Gui(WLA)}
			eval [subst $c]

		set c {entry $f.e${slider} -textvariable Transform(regTran${slider}) \
			-width 4 $Gui(WEA)}; eval [subst $c]
			bind $f.e${slider} <Return> \
				"TransformsManualTranslate regTran${slider}"
			bind $f.e${slider} <FocusOut> \
				"TransformsManualTranslate regTran${slider}"

		set c {scale $f.s${slider} -from -180 -to 180 -length 120 \
			-command "TransformsManualTranslate regTran${slider}" \
			-variable Transform(regTran${slider}) -resolution 1 $Gui(WSA)} 
			eval [subst $c]

		grid $f.l${slider} $f.e${slider} $f.s${slider} -pady 2
	}

	#-------------------------------------------
	# Manual->Rotate frame
	#-------------------------------------------
	set f $fManual.fRotate

	set c {label $f.lTitle -text "Rotation (mm)" $Gui(WLA)}
		eval [subst $c]
	grid $f.lTitle -columnspan 3 -pady $Gui(pad) -padx 1 

	foreach slider "LR PA IS" {

		set c {label $f.l${slider} -text "${slider} : " $Gui(WLA)}
			eval [subst $c]

		set c {entry $f.e${slider} -textvariable Transform(regRot${slider}) \
			-width 4 $Gui(WEA)}; eval [subst $c]
			bind $f.e${slider} <Return> \
				"TransformsManualRotate regRot${slider}"
			bind $f.e${slider} <FocusOut> \
				"TransformsManualRotate regRot${slider}"

		set c {scale $f.s${slider} -from -180 -to 180 -length 120 \
			-command "TransformsManualRotate regRot${slider}" \
			-variable Transform(regRot${slider}) -resolution 1 $Gui(WSA)} 
			eval [subst $c]

		grid $f.l${slider} $f.e${slider} $f.s${slider} -pady 2
	}


	#-------------------------------------------
	# Auto frame
	#-------------------------------------------
	set fAuto $Module(Transforms,fAuto)
	set f $fAuto

	# Frames
	frame $f.fActive  -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fVolumes -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fDesc    -bg $Gui(activeWorkspace)
	frame $f.fSpeed   -bg $Gui(activeWorkspace)
	frame $f.fRun     -bg $Gui(activeWorkspace)

	pack $f.fActive  $f.fVolumes $f.fDesc $f.fSpeed $f.fRun \
		-side top -pady $Gui(pad) -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Auto->Active frame
	#-------------------------------------------
	set f $fAuto.fActive

	set c {label $f.lActive -text "Active Transform: " $Gui(BLA)}; eval [subst $c]
	set c {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbActive.m $Gui(WMA)}; eval [subst $c]
	pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Transform(mbActiveList) $f.mbActive
	lappend Transform(mActiveList)  $f.mbActive.m

	#-------------------------------------------
	# Auto->Volumes frame
	#-------------------------------------------
	set f $fAuto.fVolumes

	set c {label $f.lVolume -text "Volume to Register:" $Gui(WLA)}; 
		eval [subst $c]
	set c {menubutton $f.mbVolume -text "None" \
		-relief raised -bd 2 -width 13 -menu $f.mbVolume.m $Gui(WMBA)}
		eval [subst $c]
	set c {menu $f.mbVolume.m $Gui(WMA)}; eval [subst $c]

	set c {label $f.lRefVolume -text "Reference Volume:" $Gui(WLA)}; 
		eval [subst $c]
	set c {menubutton $f.mbRefVolume -text "None" \
		-relief raised -bd 2 -width 13 -menu $f.mbRefVolume.m $Gui(WMBA)}
		eval [subst $c]
	set c {menu $f.mbRefVolume.m $Gui(WMA)}; eval [subst $c]
	grid $f.lVolume $f.mbVolume -sticky e -padx $Gui(pad) -pady $Gui(pad)
	grid $f.mbVolume -sticky w
	grid $f.lRefVolume $f.mbRefVolume -sticky e -padx $Gui(pad) -pady $Gui(pad)
	grid $f.mbRefVolume -sticky w

	# Append widgets to list that gets refreshed during UpdateMRML
	set Transform(mbVolume) $f.mbVolume
	set Transform(mbRefVolume) $f.mbRefVolume
	
	#-------------------------------------------
	# Auto->Desc frame
	#-------------------------------------------
	set f $fAuto.fDesc

	set c {label $f.l -justify left -text "\
		Press 'Run' to start the program\n\
		that performs automatic registration\n\
		by Mutual Information.\n\
		Your manual registration is used\n\
		as an initial pose.\
		" $Gui(WLA)}; eval [subst $c]
	pack $f.l
	
    #-------------------------------------------
    # Auto->Speed Frame
    #-------------------------------------------
    set f $fAuto.fSpeed

    frame $f.fTitle -bg $Gui(activeWorkspace)
    frame $f.fBtns -bg $Gui(activeWorkspace)
    pack $f.fTitle $f.fBtns -side left -padx 5

    set c {label $f.fTitle.lSpeed -text "Run Speed: " $Gui(WLA)}
        eval [subst $c]
    pack $f.fTitle.lSpeed

    foreach text "Fast Slow" value "Fast Slow" \
        width "5 5" {
        set c {radiobutton $f.fBtns.rSpeed$value -width $width \
            -text "$text" -value "$value" -variable Transform(autoSpeed) \
            -indicatoron 0 $Gui(WCA)}
            eval [subst $c]
        pack $f.fBtns.rSpeed$value -side left -padx 0 -pady 0
    }

	#-------------------------------------------
	# Auto->Run frame
	#-------------------------------------------
	set f $fAuto.fRun

	foreach str "Run Cancel Undo" {
		set c {button $f.b$str -text "$str" -width [expr [string length $str]+1] \
			-command "TransformsAuto$str" $Gui(WBA)}; eval [subst $c]
		set Transform(b$str) $f.b$str
	}
	pack $f.bRun $f.bUndo -side left -padx $Gui(pad) -pady $Gui(pad)
	set Transform(bUndo) $f.bUndo
	$f.bUndo configure -state disabled

	#-------------------------------------------
	# Fiducial frame
	#-------------------------------------------
	set fFiducial $Module(Transforms,fFiducial)
	set f $fFiducial

	# Frames
	frame $f.fActive -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fPeter  -bg $Gui(activeWorkspace) -relief groove -bd 3

	pack $f.fActive $f.fPeter \
		-side top -pady $Gui(pad) -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Fiducial->Active frame
	#-------------------------------------------
	set f $fFiducial.fActive

	set c {label $f.lActive -text "Active Transform: " $Gui(BLA)}; eval [subst $c]
	set c {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbActive.m $Gui(WMA)}; eval [subst $c]
	pack $f.lActive $f.mbActive -side left -pady $Gui(pad) -padx $Gui(pad)

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Transform(mbActiveList) $f.mbActive
	lappend Transform(mActiveList)  $f.mbActive.m

	#-------------------------------------------
	# Fiducial->Peter frame
	#-------------------------------------------
	set f $fFiducial.fPeter
}

proc TransformsSetPropertyType {} {
	global Transform
	
	raise $Transform(f$Transform(propertyType))
}
 
proc TransformsPropsApply {} {
	global Transform Label Module Mrml

	set m $Transform(activeID)
	if {$m == ""} {return}

	if {$m == "NEW"} {
		set i $Transform(nextID)
		incr Transform(nextID)
		lappend Transform(idList) $i
		vtkMrmlTransformNode Transform($i,node)
		set n Transform($i,node)
		$n SetID               $i

		# These get set down below, but we need them before MainUpdateMRML
		$n SetName $Transform(name)

		Mrml(dataTree) AddItem $n
		MainUpdateMRML
		set Transform(freeze) 0
		MainTransformsSetActive $i
		set m $i
	}

	Transform($m,node) SetName $Transform(name)

	# If tabs are frozen, then 
	if {$Module(freezer) != ""} {
		set cmd "Tab $Module(freezer)"
		set Module(freezer) ""
		eval $cmd
	}
	
	MainUpdateMRML
}

proc TransformsPropsCancel {} {
	global Transform Module

	# Reset props
	set m $Transform(activeID)
	if {$m == "NEW"} {
		set m [lindex $Transform(idList) 0]
	}
	set Transform(freeze) 0
	MainTransformsSetActive $m

	# Unfreeze
	if {$Module(freezer) != ""} {
		set cmd "Tab $Module(freezer)"
		set Module(freezer) ""
		eval $cmd
	}
}

proc TransformsManualTranslate {param {value ""}} {
	global Transform

	# Value is blank if used entry field instead of slider
	if {$value == ""} {
		set value $Transform($param)
	} else {
		set Transform($param) $value
	}

	# If there is no active transform, then do nothing
	set t $Transform(activeID)
	if {$t == ""} {return}

	# Transfer values from GUI to active transform
	set tran [Transform($t,node) GetTransform]
	vtkMatrix4x4 mat
	$tran GetMatrix mat
	
	switch $param {
		"regTranLR" {
			set oldVal [mat GetElement 0 3]
			mat SetElement 0 3 $value
		}
		"regTranPA" {
			set oldVal [mat GetElement 1 3]
			mat SetElement 1 3 $value
		}
		"regTranIS" {
			set oldVal [mat GetElement 2 3]
			mat SetElement 2 3 $value
		}
	}

	# Update all MRML only if the values changed
	if {$oldVal != $value} {
		$tran SetMatrix mat

		MainUpdateMRML
		Render$Transform(render)
	}
	mat Delete
}


proc TransformsManualRotate {param {value ""}} {
	global Transform

	# "value" is blank if used entry field instead of slider
	if {$value == ""} {
		set value $Transform($param)
	} else {
		set Transform($param) $value
	}

	# If there is no active transform, then do nothing
	set t $Transform(activeID)
	if {$t == ""} {return}

	# If this is a different axis of rotation than last time,
	# then store the current transform in "Transform(rotMatrix)"
	# so that the user's rotation can be concatenated with this matrix.
	# This way, this routine can be called with angles of 35, 40, 45,
	# and the second rotation will produce a visible change of 
	# 5 degrees instead of 40.
	#
	set axis [string range $param 6 7]
	if {$axis != $Transform(rotAxis)} {
		[Transform($t,node) GetTransform] GetMatrix Transform(rotMatrix)
		set Transform(rotAxis) $axis
		# O-out the rotation in the other 2 axi
		switch $axis {
			"LR" {
				set Transform(regRotPA) 0
				set Transform(regRotIS) 0
			}
			"PA" {
				set Transform(regRotLR) 0
				set Transform(regRotIS) 0
			}
			"IS" {
				set Transform(regRotPA) 0
				set Transform(regRotLR) 0
			}
		}
	}

	# Now, concatenate the rotation with the stored transform
	vtkTransform tran
	tran SetMatrix Transform(rotMatrix)
	switch $param {
		"regRotLR" {
			tran RotateX $value
		}
		"regRotPA" {
			tran RotateY $value
		}
		"regRotIS" {
			tran RotateZ $value
		}
	}

	# Only UpdateMRML if the transform changed
	# (There must be a simpler way to do this)
	
	vtkMatrix4x4 mat
	tran GetMatrix mat
	vtkMatrix4x4 mat2
	[Transform($t,node) GetTransform] GetMatrix mat2
	set differ 0
	for {set i 0} {$i < 4} {incr i} {
		for {set j 0} {$j < 4} {incr j} {
			if {[mat GetElement $i $j] != [mat2 GetElement $i $j]} {
				set differ 1
			}
		}
	}
	if {$differ == 1} {
		Transform($t,node) SetTransform tran
		MainUpdateMRML
		Render$Transform(render)
	}
	tran Delete
	mat Delete
	mat2 Delete
}

proc TransformsSetRefVolume {{v ""}} {
	global Transform Volume

	if {$v == ""} {
		set v $Transform(refVolume)
	} else {
		set Transform(refVolume) $v
	}

	$Transform(mbRefVolume) config -text "[Volume($v,node) GetName]"
}

proc TransformsSetVolume {{v ""}} {
	global Transform Volume

	if {$v == ""} {
		set v $Transform(volume)
	} else {
		set Transform(volume) $v
	}

	$Transform(mbVolume) config -text "[Volume($v,node) GetName]"
}

#-------------------------------------------------------------------------------
#        AUTO REGISTRATION
#-------------------------------------------------------------------------------

proc TransformsWritePseudoMrmlVolume {fid v} {
	global Volume
	
	scan [Volume($v,node) GetDimensions] "%d %d" x y
	set filePattern [Volume($v,node) GetFilePattern]
	set filePrefix [Volume($v,node) GetFilePrefix]
	set imageRange [Volume($v,node) GetImageRange]
	set filename [format $filePattern $filePrefix [lindex $imageRange 0]]
	set headerSize [expr [file size $filename] - ($x * $y * 2)]

	puts $fid "filePrefix $filePrefix"
	puts $fid "filePattern $filePattern"
	puts $fid "headerSize $headerSize"
	puts $fid "imageRange $imageRange"
	puts $fid "dimensions [Volume($v,node) GetDimensions]"
	puts $fid "spacing [Volume($v,node) GetSpacing]"
	puts $fid "rasToIjkMatrix [Volume($v,node) GetRasToIjkMatrix]"
	puts $fid "littleEndian [Volume($v,node) GetLittleEndian]"
	puts $fid "window [Volume($v,node) GetWindow]"
	puts $fid "level [Volume($v,node) GetLevel]"

}

proc TransformsAutoRun {} {
	global Path env Gui Transform Volume

	# Store which transform we're editing
	set t $Transform(activeID)
	set Transform(tAuto) $t
	if {$t == ""} {
		tk_messageBox -message "Please select a transform"
	}

	# v = ID of volume to register
	# r = ID of reference volume
	set v $Transform(volume)
	set r $Transform(refVolume)

	# Check that the volumes exist
	scan [Volume($v,node) GetImageRange] "%d %d" lo hi
	if {[CheckVolumeExists [Volume($v,node) GetFilePrefix] \
		[Volume($v,node) GetFilePattern] $lo $hi] != ""} {
		set str "The [Volume($v,node) GetName] volume cannot be found."
		puts $str
		tk_messageBox -message $str
		return
	}
	scan [Volume($r,node) GetImageRange] "%d %d" lo hi
	if {[CheckVolumeExists [Volume($r,node) GetFilePrefix] \
		[Volume($r,node) GetFilePattern] $lo $hi] != ""} {
		set str "The [Volume($r,node) GetName] volume cannot be found."
		puts $str
		tk_messageBox -message $str
		return
	}

	# Write a "pseudo-MRML" file for the MI program consisting of:
	#   - Reference volume
	#   - Initial pose (from manual registration)
	#   - Volume to register

	set filename $Transform(autoInput)
	set fid [open $filename w]

	TransformsWritePseudoMrmlVolume $fid $r
	puts $fid "matrix [Transform($t,node) GetMatrix]"
	TransformsWritePseudoMrmlVolume $fid $v

	close $fid

	# No-can-do on PCs
	if {$Gui(pc) == 1} {
		tk_messageBox -message "This only runs on UNIX."
		return
	}

	# Command to run MI
	set argBin     "[file join $Path(prog) mi-bin]"
	set argProgram "[file join $argBin mi]" 
	set argInput   "[file join $env(PWD) $filename]"
	set argTmp     "$Path(tmpDir)"
	if {$Transform(autoSpeed) == "Fast"} {
		set argSpeed fast
	} else {
		set argSpeed slow 
	}
	set cmd "$argProgram $argBin $argInput $argSpeed $argTmp"
	puts "MI command:\n$cmd"

	# Delete the output file
	catch {file delete $Transform(autoOutput)}

	# Exec
	catch {set pid [eval exec $cmd &]} errmsg

	# pid does not exist on error
	if {[info exists pid] == 0} {
		puts "Attempt to run MI produced error:\n'$errmsg'"
		return
	}
	puts "Running MI with pid=$pid"
	set Transform(pid) $pid

	TransformsPoll
}

#-------------------------------------------------------------------------------
# TransformsAutoCancel
#-------------------------------------------------------------------------------
proc TransformsAutoCancel {} {
    global Transform Gui

    set pid $Transform(pid)
    if {$pid == ""} {
		return
	}

	set cmd "kill -9 $pid"
	catch {exec $cmd} errmsg
	puts "$cmd --> $errmsg"
}

#-------------------------------------------------------------------------------
# TransformsPoll
#-------------------------------------------------------------------------------
proc TransformsPoll {} {
    global Transform Gui
 
    set pid $Transform(pid)
 
    if {$pid != ""} {
        catch {exec kill -0 $pid} errmsg
        if {$errmsg != ""} {
           
            # Stop polling
            set Transform(pid) ""
           
			# Determine status (if file exists, then success)
			set filename $Transform(autoOutput)
			set status 1 
			if {[CheckFileExists $filename 0] == "0"} {
				set status 0
			}

            # Apply and Notify user with popup
			if {$status == 1} {
				# Overwrite initial pose with final pose
				TransformsAutoApply
				set msg "Automatic registration completed successfully." 
	
				# Delete the output file
				catch {file delete $Transform(autoOutput)}
				catch {file delete $Transform(autoInput)}

			} else {
				set msg "Automatic registration failed." 
			}
			set x [winfo rootx $Transform(bRun)]
			set y [winfo rooty $Transform(bRun)]
            MsgPopup RegistrationReady $x $y $msg "Registration Complete"
            return
        }
    }
 
    # Poll every 10 seconds
    after 10000 TransformsPoll
}
 
#-------------------------------------------------------------------------------
# TransformsAutoApply
#-------------------------------------------------------------------------------
proc TransformsAutoApply {} {
    global Volume Transform Gui

	set v $Transform(volume)
	if {$v == $Volume(idNone)} {return}

	# Read transform from mrml file created by MI
	set filename $Transform(autoOutput)
	set dag [MRMLRead $filename]
	if {$dag == "-1"} {
		tk_messageBox -icon error -message "Failed to read '$filename'"
		return
	}
	set dag [MRMLExpandUrls $dag $Gui(root)]
	set dag [MRMLComputeRelationships $dag]

	set n [MRMLCountTypeOfNode $dag Transform]
	if {$n == 0} {
		tk_messageBox -icon error -message \
			"No transform nodes in MRML file '$filename'."
		return
	}

	set i    [MRMLGetIndexOfNodeInDag $dag 0 Transform]
	set node [MRMLGetNode $dag $i] 
	set str  [MRMLGetValue $node matrix]

	tk_messageBox -message "str=$str"

	# Convert str to matrix
	vtkMatrix4x4 mat
	for {set i 0} {$i < 4} {incr i} {
		for {set j 0} {$j < 4} {incr j} {
			mat SetElement $i $j [lindex $str [expr $i*4+$j]]
		}
	}

	# Set the current transform to this matrix
	set t $Transform(tAuto)
	set tran [Transform($t,node) GetTransform]
	$tran Push
	$tran SetMatrix mat
	mat Delete

	# Allow undo
	$Transform(bUndo) configure -state normal

	# Update MRML
	MainUpdateMRML
	RenderAll
}

proc TransformsAutoUndo {} {
	global Transform
	
	set t $Transform(tAuto)
	set tran [Transform($t,node) GetTransform]
	$tran Pop

	# Disallow undo
	$Transform(bUndo) configure -state disabled

	# Update MRML
	MainUpdateMRML
	RenderAll
}
