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
# FILE:        Realtime.tcl
# DATE:        02/16/2000 09:13
# LAST EDITOR: gering
# PROCEDURES:  
#   RealtimeInit
#   RealtimeBuildVTK
#   RealtimeUpdateMRML
#   RealtimeBuildGUI
#   RealtimeEnter
#   RealtimeSetEffect
#   RealtimeSetSwitch
#   RealtimeImageComponentCallback
#   RealtimeImageCompleteCallback
#   RealtimeMakeBaseline
#   RealtimeSetRealtime
#   RealtimeSetBaseline
#   RealtimeSetResult
#   RealtimeGetRealtimeID
#   RealtimeGetBaselineID
#   RealtimeGetResultID
#   RealtimeWrite
#   RealtimeRead
#==========================================================================auto=

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
	set Module($m,procEnter) RealtimeEnter

	# Define Dependencies
	set Module($m,depend) "Locator"

	# Initialize globals
	set Realtime(idRealtime)     $Volume(idNone)
	set Realtime(idBaseline)     NEW
	set Realtime(idResult)       NEW
	set Realtime(prefixBaseline) ""
	set Realtime(prefixResult)   ""
	set Realtime(switch)         Off
	set Realtime(effectList)     "Copy"
	set Realtime(effect)         Copy
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
	foreach v $Volume(idList) {
		$m add command -label [Volume($v,node) GetName] -command \
			"RealtimeSetRealtime $v; RenderAll"
	}

	# Baseline Volume menu
	#---------------------------------------------------------------------------
	set m $Realtime(mBaseline)
	$m delete 0 end
	set idBaseline ""
	foreach v $Volume(idList) {
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
	foreach v $Volume(idList) {
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

	eval {label $f.l -text "Prefix:"} $Gui(WLA)
	set c {entry $f.e \
		-textvariable Realtime(prefixBaseline) $Gui(WEA)}; eval [subst $c]
	pack $f.l -padx 3 -side left
	pack $f.e -padx 3 -side left -expand 1 -fill x

	#-------------------------------------------
	# Processing->Baseline->Btns
	#-------------------------------------------
	set f $fProcessing.fBaseline.fBtns

	set c {button $f.bWrite -text "Save" -width 5 \
		-command "RealtimeWrite Baseline; RenderAll" $Gui(WBA)}; eval [subst $c]
	set c {button $f.bRead -text "Read" -width 5 \
		-command "RealtimeRead Baseline; RenderAll" $Gui(WBA)}; eval [subst $c]
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

	eval {label $f.l -text "Prefix:"} $Gui(WLA)
	set c {entry $f.e \
		-textvariable Realtime(prefixResult) $Gui(WEA)}; eval [subst $c]
	pack $f.l -padx 3 -side left
	pack $f.e -padx 3 -side left -expand 1 -fill x

	#-------------------------------------------
	# Processing->Result->Btns
	#-------------------------------------------
	set f $fProcessing.fResult.fBtns

	set c {button $f.bWrite -text "Save" -width 5 \
		-command "RealtimeWrite Result; RenderAll" $Gui(WBA)}; eval [subst $c]
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
	foreach e $Realtime(effectList) {
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

#-------------------------------------------------------------------------------
# .PROC RealtimeEnter
# .END
#-------------------------------------------------------------------------------
proc RealtimeEnter {} {
	global Realtime Volume Slice

	# If the Realtime volume is None, then select what's being displayed,
	# otherwise the first volume in the mrml tree.
	
	if {[RealtimeGetRealtimeID] == $Volume(idNone)} {
		set v [[[Slicer GetBackVolume $Slice(activeID)] GetMrmlNode] GetID]
		if {$v == $Volume(idNone)} {
			set v [lindex $Volume(idList) 0]
		}
		if {$v != $Volume(idNone)} {
			RealtimeSetRealtime $v
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC RealtimeSetEffect
# .END
#-------------------------------------------------------------------------------
proc RealtimeSetEffect {e} {
	global Realtime

	set Realtime(effect) $e

	# Change menu text
	$Realtime(mbEffect) config -text $Realtime(effect)

}

#-------------------------------------------------------------------------------
# .PROC RealtimeSetSwitch
# .END
#-------------------------------------------------------------------------------
proc RealtimeSetSwitch {} {
	global Realtime

	# For now, just process the images once.
	# Later, have the Locator module call RealtimeImageComponentCallback.
	
	if {$Realtime(switch) == "On"} {
		RealtimeImageComponentCallback
	}
	set Realtime(switch) Off
}

#-------------------------------------------------------------------------------
# .PROC RealtimeImageComponentCallback
# .END
#-------------------------------------------------------------------------------
proc RealtimeImageComponentCallback {} {
	global Realtime

	# Images may have multiple components, so this callback is called
	# as each image component is received from the scanner.  Once all
	# components have been received, this callback calls
	# RealtimeImageCompleteCallback to process the complete image.
	#
	# For example, phase difference requires a real and imaginary
	# image.  As each component-image arrives, it can be added to the
	# complete-image as another component using vtkImageAppendComponent.



	RealtimeImageCompleteCallback
}

#-------------------------------------------------------------------------------
# .PROC RealtimeImageCompleteCallback
# .END
#-------------------------------------------------------------------------------
proc RealtimeImageCompleteCallback {} {
	global Realtime Volume

	# Perform the realtime image processing

	set s [RealtimeGetRealtimeID]
	set b [RealtimeGetBaselineID]
	set r [RealtimeGetResultID]

	# Check extents
	set sExt [[Volume($s,vol) GetOutput] GetExtent]
	set bExt [[Volume($b,vol) GetOutput] GetExtent]
	if {$sExt != $bExt} {
		tk_messageBox -icon error -message \
			"Extents are not equal!\n\nRealtime = $sExt\nBaseline = $bExt"
		return
	}

	# Perform the computation here
	switch $Realtime(effect) {

	# Copy
	"Copy" {
		vtkImageCopy copy
		copy SetInput [Volume($s,vol) GetOutput]
		copy Update
		copy SetInput ""
		Volume($r,vol) SetImageData [copy GetOutput]
		copy SetOutput ""
		copy Delete
	}

	# Subtract
	"Subtract" {
		# THIS DOES NOT WORK
		vtkImageMathematics math
		math SetInput 1 [Volume($s,vol) GetOutput]
		math SetInput 2 [Volume($b,vol) GetOutput]
		math SetOperationToSubtract
		math Update
		math SetInput 1 ""
		math SetInput 2 ""
		Volume($r,vol) SetImageData [math GetOutput]
		math SetOutput ""
		math Delete
	}
	}

	# Mark Result as unsaved
	set Volume($r,dirty) 1

	# r copies s's MrmlNode
	Volume($r,node) Copy Volume($s,node)

	# Update pipeline and GUI
	MainVolumesUpdate $r

	RenderAll
}

#-------------------------------------------------------------------------------
# .PROC RealtimeMakeBaseline
# .END
#-------------------------------------------------------------------------------
proc RealtimeMakeBaseline {} {
	global Volume

	# Copy the Realtime image to the Baseline image

	set s [RealtimeGetRealtimeID]
	set b [RealtimeGetBaselineID]

	# Copy image pixels
	vtkImageCopy copy
	copy SetInput [Volume($s,vol) GetOutput]
	copy Update
	copy SetInput ""
	Volume($b,vol) SetImageData [copy GetOutput]
	copy SetOutput ""
	copy Delete

	# Mark baseline as unsaved
	set Volume($b,dirty) 1

	# b copies s's MrmlNode
	Volume($b,node) Copy Volume($s,node)

	# Update pipeline and GUI
	MainVolumesUpdate $b

	RenderAll
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
	
	# Change button text, and show file prefix
	if {$v == "NEW"} {
		$Realtime(mbBaseline) config -text $v
		set Realtime(prefixBaseline) ""
	} else {
		$Realtime(mbBaseline) config -text [Volume($v,node) GetName]
		set Realtime(prefixBaseline) [MainFileGetRelativePrefix \
			[Volume($v,node) GetFilePrefix]]
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
	if {$v == $Realtime(idBaseline) && $v != "NEW"} {
		tk_messageBox -message "The Baseline and Result volumes must differ."
		return
	}
	set Realtime(idResult) $v
	
	# Change button text, and show file prefix
	if {$v == "NEW"} {
		$Realtime(mbResult) config -text $v
		set Realtime(prefixResult) ""
	} else {
		$Realtime(mbResult) config -text [Volume($v,node) GetName]
		set Realtime(prefixResult) [MainFileGetRelativePrefix \
			[Volume($v,node) GetFilePrefix]]
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
	global Realtime Volume Lut
		
	# If there is no Baseline volume, then create one
	if {$Realtime(idBaseline) != "NEW"} {
		return $Realtime(idBaseline)
	}
	
	# Create the node
	set n [MainMrmlAddNode Volume]
	set v [$n GetID]
	$n SetDescription "Baseline Volume"
	$n SetName        "Baseline"

	# Create the volume
	MainVolumesCreate $v

	RealtimeSetBaseline $v

	MainUpdateMRML

	# Copy Realtime
	RealtimeMakeBaseline

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
	global Realtime Volume Lut
		
	# If there is no Result volume, then create one
	if {$Realtime(idResult) != "NEW"} {
		return $Realtime(idResult)
	}
	
	# Create the node
	set n [MainMrmlAddNode Volume]
	set v [$n GetID]
	$n SetDescription "Result Volume"
	$n SetName        "Result"

	# Create the volume
	MainVolumesCreate $v

	RealtimeSetResult $v

	MainUpdateMRML

	return $v
}


#-------------------------------------------------------------------------------
# .PROC RealtimeWrite
# .END
#-------------------------------------------------------------------------------
proc RealtimeWrite {data} {
	global Volume Realtime

	# If the volume doesn't exist yet, then don't write it, duh!
	if {$Realtime(id$data) == "NEW"} {
		tk_messageBox -message "Nothing to write."
		return
	}

	switch $data {
		Result   {set v [RealtimeGetResultID]}
		Baseline {set v [RealtimeGetBaselineID]}
	}

	# Show user a File dialog box
	set Realtime(prefix$data) [MainFileSaveVolume $v $Realtime(prefix$data)]
	if {$Realtime(prefix$data) == ""} {return}

	# Write
	MainVolumesWrite $v $Realtime(prefix$data)

	# Prefix changed, so update the Volumes->Props tab
	MainVolumesSetActive $v
}

#-------------------------------------------------------------------------------
# .PROC RealtimeRead
# .END
#-------------------------------------------------------------------------------
proc RealtimeRead {data} {
	global Volume Realtime Mrml

	# If the volume doesn't exist yet, then don't read it, duh!
	if {$Realtime(id$data) == "NEW"} {
		tk_messageBox -message "Nothing to read."
		return
	}

	switch $data {
		Result   {set v $Realtime(idResult)}
		Baseline {set v $Realtime(idBaseline)}
	}

	# Show user a File dialog box
	set Realtime(prefix$data) [MainFileOpenVolume $v $Realtime(prefix$data)]
	if {$Realtime(prefix$data) == ""} {return}
	
	# Read
	Volume($v,node) SetFilePrefix $Realtime(prefix$data)
	Volume($v,node) SetFullPrefix \
		[file join $Mrml(dir) [Volume($v,node) GetFilePrefix]]
	if {[MainVolumesRead $v] < 0} {
		return
	}

	# Update pipeline and GUI
	MainVolumesUpdate $v

	# Prefix changed, so update the Models->Props tab
	MainVolumesSetActive $v
}

