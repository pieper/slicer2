
#-------------------------------------------------------------------------------
# .PROC RealtimeInit
# .END
#-------------------------------------------------------------------------------
proc RealtimeInit {} {
	global Realtime Gui Volume Module

	# Define Tabs
	set m Realtime
	set Module($m,row1List) "Help Processing"
	set Module($m,row1Name) "{Help} {Processing}"
	set Module($m,row1,tab) Processing

	# Define Procedures
	set Module($m,procGUI)   RealtimeBuildGUI
	set Module($m,procMRML)  RealtimeUpdateMRML
	set Module($m,procVTK)   RealtimeBuildVTK

	# Define Dependencies
	set Module($m,depend) "Locator"

	# Initialize globals
	set Realtime(idRealtime)     $Volume(idNone)
	set Realtime(idBaseline)     NEW
	set Realtime(idResult)       NEW
	set Realtime(prefixBaseline) ""
	set Realtime(prefixResult)   ""
	set Realtime(effect)         Average
	set Realtime(switch)         Off
}

#-------------------------------------------------------------------------------
# .PROC RealtimeBuildVTK
# .END
#-------------------------------------------------------------------------------
proc RealtimeBuildVTK {} {
	global Realtime

}

#-------------------------------------------------------------------------------
# .PROC RealtimeUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc RealtimeUpdateMRML {} {
	global Volume Realtime

	# See if the volume for each menu actually exists.
	# If not, use the None volume
	#
	set n $Volume(idNone)
	if {[lsearch $Volume(idList) $Realtime(idRealtime)] == -1} {
		RealtimeSetRealtime $n
	}
	if {$Realtime(idBaseline) != "NEW" && \
		[lsearch $Volume(idList) $Realtime(idBaseline)] == -1} {
		RealtimeSetBaseline NEW
	}
	if {$Realtime(idResult) != "NEW" && \
		[lsearch $Volume(idList) $Realtime(idResult)] == -1} {
		RealtimeSetResult NEW
	}

	# Realtime Volume menu
	#---------------------------------------------------------------------------
	set m $Realtime(mRealtime)
	$m delete 0 end
	foreach v $Volume(idListForMenu) {
		$m add command -label [Volume($v,node) GetName] -command \
			"RealtimeSetRealtime $v; RenderAll"
	}

	# Baseline Volume menu
	#---------------------------------------------------------------------------
	set m $Realtime(mBaseline)
	$m delete 0 end
	set idBaseline ""
	foreach v $Volume(idListForMenu) {
		if {$v != $Volume(idNone) && $v != $Realtime(idResult)} {
			$m add command -label [Volume($v,node) GetName] -command \
				"RealtimeSetBaseline $v; RenderAll"
		}
		if {[Volume($v,node) GetName] == "Baseline"} {
			set idBaseline $v
		}
	}
	# If there is Baseline, then select it, else add a NEW option
	if {$idBaseline != ""} {
		RealtimeSetBaseline $idBaseline
	} else {
		$m add command -label NEW -command "RealtimeSetBaseline NEW; RenderAll"
	}

	# Result Volume menu
	#---------------------------------------------------------------------------
	set m $Realtime(mResult)
	$m delete 0 end
	set idResult ""
	foreach v $Volume(idListForMenu) {
		if {$v != $Volume(idNone) && $v != $Realtime(idBaseline)} {
			$m add command -label [Volume($v,node) GetName] -command \
				"RealtimeSetResult $v; RenderAll"
		}
		if {[Volume($v,node) GetName] == "Result"} {
			set idResult $v
		}
	}
	# If there is working, then select it, else add a NEW option
	if {$idResult != ""} {
		RealtimeSetResult $idResult
	} else {
		$m add command -label NEW -command "RealtimeSetResult NEW; RenderAll"
	}
}

#-------------------------------------------------------------------------------
# .PROC RealtimeBuildGUI
# .END
#-------------------------------------------------------------------------------
proc RealtimeBuildGUI {} {
	global Gui Volume Realtime Module Slice Path

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Processing
	#   Realtime
	#   Baseline
	#   Result
	#   Effects
	#     Menu
	#     Switch
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Models are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Realtime $help
	MainHelpBuildGUI Realtime


	############################################################################
	#                                 Processing
	############################################################################

	#-------------------------------------------
	# Processing frame
	#-------------------------------------------
	set fProcessing $Module(Realtime,fProcessing)
	set f $fProcessing

	frame $f.fRealtime  -bg $Gui(activeWorkspace)
	frame $f.fBaseline  -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fResult    -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fEffects   -bg $Gui(activeWorkspace) -relief groove -bd 3

	pack $f.fRealtime $f.fBaseline $f.fResult $f.fEffects \
		-side top -padx $Gui(pad) -pady $Gui(pad) -fill x

	#-------------------------------------------
	# Processing->Realtime
	#-------------------------------------------
	set f $fProcessing.fRealtime

	# Realtime Volume menu
	set c {label $f.lRealtime -text "Realtime Volume:" $Gui(WTA)}; eval [subst $c]

	set c {menubutton $f.mbRealtime -text "None" -relief raised -bd 2 -width 18 \
		-menu $f.mbRealtime.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbRealtime.m $Gui(WMA)}; eval [subst $c]
	pack $f.lRealtime -padx $Gui(pad) -side left -anchor e
	pack $f.mbRealtime -padx $Gui(pad) -side left -anchor w

	# Save widgets for changing
	set Realtime(mbRealtime) $f.mbRealtime
	set Realtime(mRealtime)  $f.mbRealtime.m


	#-------------------------------------------
	# Processing->Baseline
	#-------------------------------------------
	set f $fProcessing.fBaseline

	frame $f.fMenu -bg $Gui(activeWorkspace)
	frame $f.fPrefix -bg $Gui(activeWorkspace)
	frame $f.fBtns   -bg $Gui(activeWorkspace)
	pack $f.fMenu -side top -pady $Gui(pad)
	pack $f.fPrefix -side top -pady $Gui(pad) -fill x
	pack $f.fBtns -side top -pady $Gui(pad)

	#-------------------------------------------
	# Processing->Baseline->Menu
	#-------------------------------------------
	set f $fProcessing.fBaseline.fMenu

	# Volume menu
	set c {label $f.lBaseline -text "Baseline Volume:" $Gui(WTA)}; eval [subst $c]

	set c {menubutton $f.mbBaseline -text "NEW" -relief raised -bd 2 -width 18 \
		-menu $f.mbBaseline.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbBaseline.m $Gui(WMA)}; eval [subst $c]
	pack $f.lBaseline $f.mbBaseline -padx $Gui(pad) -side left

	# Save widgets for changing
	set Realtime(mbBaseline) $f.mbBaseline
	set Realtime(mBaseline)  $f.mbBaseline.m

	#-------------------------------------------
	# Processing->Baseline->Prefix
	#-------------------------------------------
	set f $fProcessing.fBaseline.fPrefix

	eval {button $f.b -text "Browse" -width 6 \
		-command "RealtimeSetPrefix Baseline"} $Gui(WBA)
	set c {entry $f.e \
		-textvariable Realtime(prefixBaseline) $Gui(WEA)}; eval [subst $c]
	pack $f.b $f.e -padx 3 -side left -expand 1 -fill x

	#-------------------------------------------
	# Processing->Baseline->Btns
	#-------------------------------------------
	set f $fProcessing.fBaseline.fBtns

	set c {button $f.bWrite -text "Save" -width 5 \
		-command "RealtimeWriteOutput Baseline; RenderAll" $Gui(WBA)}; eval [subst $c]
	set c {button $f.bRead -text "Read" -width 5 \
		-command "RealtimeReadOutput Baseline; RenderAll" $Gui(WBA)}; eval [subst $c]
	set c {button $f.bSet -text "Copy Realtime" -width 14 \
		-command "RealtimeMakeBaseline; RenderAll" $Gui(WBA)}; eval [subst $c]
	pack $f.bWrite $f.bRead $f.bSet -side left -padx $Gui(pad)



	#-------------------------------------------
	# Processing->Result
	#-------------------------------------------
	set f $fProcessing.fResult

	frame $f.fMenu -bg $Gui(activeWorkspace)
	frame $f.fPrefix -bg $Gui(activeWorkspace)
	frame $f.fBtns   -bg $Gui(activeWorkspace)
	pack $f.fMenu -side top -pady $Gui(pad)
	pack $f.fPrefix -side top -pady $Gui(pad) -fill x
	pack $f.fBtns -side top -pady $Gui(pad)

	#-------------------------------------------
	# Processing->Result->Menu
	#-------------------------------------------
	set f $fProcessing.fResult.fMenu

	# Volume menu
	set c {label $f.lResult -text "Result Volume:" $Gui(WTA)}; eval [subst $c]

	set c {menubutton $f.mbResult -text "NEW" -relief raised -bd 2 -width 18 \
		-menu $f.mbResult.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbResult.m $Gui(WMA)}; eval [subst $c]
	pack $f.lResult $f.mbResult -padx $Gui(pad) -side left

	# Save widgets for changing
	set Realtime(mbResult) $f.mbResult
	set Realtime(mResult)  $f.mbResult.m

	#-------------------------------------------
	# Processing->Result->Prefix
	#-------------------------------------------
	set f $fProcessing.fResult.fPrefix

	eval {button $f.b -text "Browse" -width 6 \
		-command "RealtimeSetPrefix Result"} $Gui(WBA)
	set c {entry $f.e \
		-textvariable Realtime(prefixResult) $Gui(WEA)}; eval [subst $c]
	pack $f.b $f.e -padx 3 -side left -expand 1 -fill x

	#-------------------------------------------
	# Processing->Result->Btns
	#-------------------------------------------
	set f $fProcessing.fResult.fBtns

	set c {button $f.bWrite -text "Save" -width 5 \
		-command "RealtimeWriteOutput Result; RenderAll" $Gui(WBA)}; eval [subst $c]
	pack $f.bWrite -side left -padx $Gui(pad)



	#-------------------------------------------
	# Processing->Effects
	#-------------------------------------------
	set f $fProcessing.fEffects

	frame $f.fMenu   -bg $Gui(activeWorkspace)
	frame $f.fSwitch -bg $Gui(activeWorkspace)
	pack $f.fMenu $f.fSwitch -side top -pady $Gui(pad)

	#-------------------------------------------
	# Processing->Effects->Menu
	#-------------------------------------------
	set f $fProcessing.fEffects.fMenu

	# Effects menu
	set c {label $f.l -text "Effect:" $Gui(WTA)}; eval [subst $c]

	set c {menubutton $f.mbEffect -text $Realtime(effect) -relief raised -bd 2 -width 15 \
		-menu $f.mbEffect.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbEffect.m $Gui(WMA)}; eval [subst $c]
	set Realtime(mbEffect) $f.mbEffect
	set m $Realtime(mbEffect).m
	foreach e "Average Subtract" {
		$m add command -label $e -command "RealtimeSetEffect $e"
	}
	pack $f.l $f.mbEffect -side left -padx $Gui(pad)
	
	#-------------------------------------------
	# Processing->Effects->Switch
	#-------------------------------------------
	set f $fProcessing.fEffects.fSwitch

	set c {label $f.lActive -text "Processing:" $Gui(WLA)}; eval [subst $c]
	pack $f.lActive -side left -pady $Gui(pad) -padx $Gui(pad) -fill x

	foreach s "On Off Pause" text "On Off Pause" width "3 4 6" {
		set c {radiobutton $f.r$s -width $width -indicatoron 0\
			-text $text -value $s -variable Realtime(switch) \
			-command "RealtimeSetSwitch" $Gui(WCA)}
			eval [subst $c]
		pack $f.r$s -side left -fill x -anchor e
	}
}

proc RealtimeSetEffect {e} {
	global Realtime

	set Realtime(effect) $e

	# Change menu text
	$Realtime(mbEffect) config -text $Realtime(effect)

}

proc RealtimeSetSwitch {} {
	global Realtime

}


#-------------------------------------------------------------------------------
# .PROC RealtimeSetPrefix
# .END
#-------------------------------------------------------------------------------
proc RealtimeSetPrefix {data} {
	global Realtime Mrml

	# Cannot have blank prefix
	if {$Realtime(prefix$data) == ""} {
		set Realtime(prefix$data) [Uncap $data]
	}

 	# Show popup initialized to the last file saved
	set filename [file join $Mrml(dir) $Realtime(prefix$data)]
	set dir [file dirname $filename]
	set typelist {
		{"All Files" {*}}
	}
	set filename [tk_getSaveFile -title "Save Volume" \
		-filetypes $typelist -initialdir "$dir" -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return}

	# Store it as a relative prefix for next time
	set Realtime(prefix$data) [MainFileGetRelativePrefix $filename]
}


#-------------------------------------------------------------------------------
# .PROC RealtimeSetRealtime
# .END
#-------------------------------------------------------------------------------
proc RealtimeSetRealtime {v} {
	global Realtime Volume

	if {$v == $Realtime(idBaseline)} {
		tk_messageBox -message "The Realtime and Baseline volumes must differ."
		return
	}
	if {$v == $Realtime(idResult)} {
		tk_messageBox -message "The Realtime and Result volumes must differ."
		return
	}

	set Realtime(idRealtime) $v
	
	# Change button text
    $Realtime(mbRealtime) config -text [Volume($v,node) GetName]
}

#-------------------------------------------------------------------------------
# .PROC RealtimeSetBaseline
# .END
#-------------------------------------------------------------------------------
proc RealtimeSetBaseline {v} {
	global Realtime Volume

	if {$v == [RealtimeGetRealtimeID]} {
		tk_messageBox -message "The Realtime and Baseline volumes must differ."
		return
	}
	if {$v == $Realtime(idResult) && $v != "NEW"} {
		tk_messageBox -message "The Result and Baseline volumes must differ."
		return
	}
	set Realtime(idBaseline) $v
	
	# Change button text
	if {$v == "NEW"} {
		$Realtime(mbBaseline) config -text $v
	} else {
		$Realtime(mbBaseline) config -text [Volume($v,node) GetName]
	}
}

#-------------------------------------------------------------------------------
# .PROC RealtimeSetResult
# .END
#-------------------------------------------------------------------------------
proc RealtimeSetResult {v} {
	global Realtime Volume

	if {$v == [RealtimeGetRealtimeID]} {
		tk_messageBox -message "The Realtime and Result volumes must differ."
		return
	}
	if {$v == $Realtime(idResult) && $v != "NEW"} {
		tk_messageBox -message "The Result and Result volumes must differ."
		return
	}
	set Realtime(idResult) $v
	
	# Change button text
	if {$v == "NEW"} {
		$Realtime(mbResult) config -text $v
	} else {
		$Realtime(mbResult) config -text [Volume($v,node) GetName]
	}
}

#-------------------------------------------------------------------------------
# .PROC RealtimeGetRealtimeID
# .END
#-------------------------------------------------------------------------------
proc RealtimeGetRealtimeID {} {
	global Realtime
	
	return $Realtime(idRealtime)
}

#-------------------------------------------------------------------------------
# .PROC RealtimeGetBaselineID
#
# Returns the Baseline volume's ID.
# If there is no Baseline volume (Realtime(idBaseline)==NEW), then it creates one.
# .END
#-------------------------------------------------------------------------------
proc RealtimeGetBaselineID {} {
	global Realtime Dag Volume Lut
		
	if {$Realtime(idBaseline) != "NEW"} {
		return $Realtime(idBaseline)
	}

	# Create a Baseline volume
	# -----------------------------------------------------
	set v [expr [lindex [lsort -integer -decreasing $Volume(idList)] 0] + 1]
	lappend Volume(idList) $v
	set Realtime(idBaseline) $v

	tk_messageBox -message "NEW BASELINE=$v"

	# MrmlVolumeNode
	vtkMrmlVolumeNode Volume($v,node)
	Volume($v,node) SetID          $v
	Volume($v,node) SetDescription "Baseline Volume=$v"
	Volume($v,node) SetName        "Baseline"
	Volume($v,node) SetLUTName     $Lut(idLabel)
	Volume($v,node) InterpolateOff

	# MrmlVolume
	vtkMrmlVolume Volume($v,vol)
	Volume($v,vol) SetMrmlNode           Volume($v,node)
	Volume($v,vol) SetHistogramWidth     $Volume(histWidth)
	Volume($v,vol) SetHistogramHeight    $Volume(histHeight)
	Volume($v,vol) UseLabelIndirectLUTOn
	Volume($v,vol) SetStartMethod     MainStartProgress
	Volume($v,vol) SetProgressMethod "MainShowProgress Volume($v,vol)"
	Volume($v,vol) SetEndMethod       MainEndProgress

	MainUpdateMRML

	return $v
}

#-------------------------------------------------------------------------------
# .PROC RealtimeGetResultID
#
# Returns the Result volume's ID.
# If there is no Result volume (Realtime(idResult)==NEW), then it creates one.
# .END
#-------------------------------------------------------------------------------
proc RealtimeGetResultID {} {
	global Realtime Dag Volume Lut
		
	if {$Realtime(idResult) != "NEW"} {
		return $Realtime(idResult)
	}

	# Create a Result volume
	# -----------------------------------------------------
	set v [expr [lindex [lsort -integer -decreasing $Volume(idList)] 0] + 1]
	lappend Volume(idList) $v
	set Realtime(idResult) $v

	tk_messageBox -message "NEW BASELINE=$v"

	# MrmlVolumeNode
	vtkMrmlVolumeNode Volume($v,node)
	Volume($v,node) SetID          $v
	Volume($v,node) SetDescription "Result Volume=$v"
	Volume($v,node) SetName        "Result"
	Volume($v,node) SetLUTName     $Lut(idLabel)
	Volume($v,node) InterpolateOff

	# MrmlVolume
	vtkMrmlVolume Volume($v,vol)
	Volume($v,vol) SetMrmlNode           Volume($v,node)
	Volume($v,vol) SetHistogramWidth     $Volume(histWidth)
	Volume($v,vol) SetHistogramHeight    $Volume(histHeight)
	Volume($v,vol) UseLabelIndirectLUTOn
	Volume($v,vol) SetStartMethod     MainStartProgress
	Volume($v,vol) SetProgressMethod "MainShowProgress Volume($v,vol)"
	Volume($v,vol) SetEndMethod       MainEndProgress

	MainUpdateMRML

	return $v
}


proc RealtimeMakeBaseline {} {
}

proc RealtimeReadOutput {data} {
	global Realtime

}

#-------------------------------------------------------------------------------
# .PROC RealtimeWriteOutput
#
# Write either the Baseline or Result output to disk.
# .END
#-------------------------------------------------------------------------------
proc RealtimeWriteOutput {data} {
	global Volume Gui Path Lut tcl_platform Mrml Realtime

	# If the volume doesn't exist yet, then don't write it, duh!
	if {$Realtime(id$data) == "NEW"} {
		tk_messageBox -message "Nothing to write."
		return
	}

	switch $data {
		Result   {set v [RealtimeGetResultID]}
		Baseline {set v [RealtimeGetBaselineID]}
	}

	# Change prefix and header to differ from the input volume
	#
	set filePrefix $Realtime(prefix$data)
	set fileFull [file join $Mrml(dir) $filePrefix]

	# Check that it's not blank
	if {[file isdirectory $fileFull] == 1} {
		tk_messageBox -icon error -title $Gui(title) \
			-message "Please enter a file prefix for the $data volume."
		return 0
	}
	
	# Check that it's a prefix, not a directory
	if {[file isdirectory $fileFull] == 1} {
		tk_messageBox -icon error -title $Gui(title) \
			-message "Please enter a file prefix, not a directory,\n\
			for the $data volume."
		return 0
	}

	# Check that the directory exists
	set dir [file dirname $fileFull]
	if {[file isdirectory $dir] == 0} {
		if {$dir != ""} {
			file mkdir $dir
		}
		if {[file isdirectory $dir] == 0} {
			tk_messageBox -icon info -type ok -title $Gui(title) \
			-message "Failed to make '$dir', so using current directory."
			set dir ""
		}
	}
	Volume($v,node) SetFilePrefix $filePrefix
	Volume($v,node) SetFullPrefix $fileFull

	# interpolate off
	Volume($v,node) InterpolateOff

	# Determine if littleEndian
	if {$tcl_platform(machine) == "intel" || $tcl_platform(machine) == "mips"} {
		Volume($v,node) SetLittleEndian 1
	} else {
		Volume($v,node) SetLittleEndian 1
	}

	# Write volume data
	set Gui(progressText) "Writing [Volume($v,node) GetName]"
	puts "Writing '$fileFull' ..."
	Volume($v,vol) Write
	puts " ...done."

	# Write MRML file
	set filename $fileFull.xml
	vtkMrmlTree tree
	tree AddItem Volume($v,node)
	tree Write $filename
	tree RemoveAllItems
	tree Delete
	puts "Saved MRML file: $filename"
}

