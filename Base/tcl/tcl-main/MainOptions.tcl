#=auto==========================================================================
# (c) Copyright 2001 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
# FILE:        MainOptions.tcl
# PROCEDURES:  
#   MainOptionsInit
#   MainOptionsUpdateMRML
#   MainOptionsBuildVTK
#   MainOptionsCreate
#   MainOptionsDelete
#   MainOptionsSetActive
#   MainOptionsParsePresets
#   MainOptionsUseDefaultPresets
#   MainOptionsUseDefaultPresetsForOneScene
#   MainOptionsParseDefaults
#   MainOptionsRetrievePresetValues
#   MainOptionsUnparsePresets
#   MainOptionsPreset
#   MainOptionsPresetCallback
#   MainOptionsRecallPresets
#   MainOptionsStorePresets
#   MainOptionsPresetSaveCreateDialog path
#   MainOptionsPresetSaveOk
#   MainOptionsPresetDeleteDialog
#   MainOptionsPresetDelete
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MainOptionsInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainOptionsInit {} {
	global Module Options Preset Scenes

	# Define Procedures
	lappend Module(procVTK)  MainOptionsBuildVTK

	# Append widgets to list that gets refreshed during UpdateMRML
	set Options(mbActiveList) ""
	set Options(mActiveList)  ""

	set Options(activeID) ""
	set Options(freeze) ""

        # Set version info
        lappend Module(versions) [ParseCVSInfo MainOptions \
		{$Revision: 1.18 $} {$Date: 2001/12/26 15:23:39 $}]

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
	
	set Scenes(oldMRML) 0
	set Scenes(nameList) ""
	
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsUpdateMRML
# 
# .ARGS
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
# 
# .ARGS
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
# 
# .ARGS
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
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainOptionsParsePresets {attr} {
	global Preset Scenes
	
	# if this procedure is called, we can be sure that we have an old
	# mrml 2.0 options tag...
	set Scenes(oldMRML) 1
	
	foreach a $attr {
		set key [lindex $a 0]
		set val [lreplace $a 0 0]

		set Preset($key) $val
	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsUseDefaultPresets
# 
# .ARGS
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
# .PROC MainOptionsUseDefaultPresetsForOneScene
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainOptionsUseDefaultPresetsForOneScene {p} {
	global Module Preset

	foreach m $Module(idList) {
		if {[info exists Module($m,presets)] == 1} {
			foreach key $Preset($m,keys) {
				set Preset($m,$p,$key) $Preset($m,default,$key)
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
# .PROC MainOptionsRetrievePresetValues
#  Retrieves all preset values from the MRML tree.
# .END
#-------------------------------------------------------------------------------
proc MainOptionsRetrievePresetValues {} {
	global Mrml Preset Scenes Gui
	
	Mrml(dataTree) InitTraversal
	set node [Mrml(dataTree) GetNextItem]
	set currentScene ""
	set currentVolume ""
	if {$Scenes(oldMRML) == 0} {
		set Scenes(nameList) ""
		set Preset(idList) "0"
	} else {
		set Scenes(nameList) "1 2 3"
		set Preset(idList) "0 1 2 3"
	}
	
	while {$node != ""} {
		if {[string compare -length 6 $node "Scenes"] == 0} {
			# don't allow spaces in scene names
			# they only cause errors
			regsub -all " " [$node GetName] "_" currentScene
			if {$currentScene == "default"} {
				set currentScene "0"
			}
			if {$currentScene != "0"} {
				# don't add the default values to the scenes list
				lappend Scenes(nameList) $currentScene
				lappend Preset(idList) $currentScene
			}
			# set all presets to default settings
			MainOptionsUseDefaultPresetsForOneScene $currentScene
		}
		if {[string compare -length 9 $node "EndScenes"] == 0} {
			set currentScene ""
		}
		if {([string compare -length 6 $node "Matrix"] == 0) && ($currentScene != "")} {
			set Preset(Matrix,$currentScene,name) [$node GetName]
			set Preset(Matrix,$currentScene,matrix) "[$node GetMatrix]"
		}
		if {([string compare -length 10 $node "ModelState"] == 0) && ($currentScene != "")} {
			set ModelID [SharedModelLookup [$node GetModelRefID]]
			set Preset(Models,$currentScene,$ModelID,visibility) [$node GetVisible]
			set Preset(Models,$currentScene,$ModelID,opacity) [$node GetOpacity]
			set Preset(Models,$currentScene,$ModelID,clipping) [$node GetClipping]
			set Preset(Models,$currentScene,$ModelID,backfaceCulling) [$node GetBackfaceCulling]
		}
		if {([string compare -length 11 $node "VolumeState"] == 0) && ($currentScene != "")} {
			set currentVolume [$node GetVolumeRefID]
			set Preset(Slices,$currentScene,fade) [$node GetFade]
			set Preset(Slices,$currentScene,opacity) [$node GetOpacity]
		}
		if {[string compare -length 14 $node "EndVolumeState"] == 0} {
			set currentVolume ""
		}
		if {([string compare -length 11 $node "WindowLevel"] == 0) && ($currentVolume != "")} {
			set Preset(WindowLevel,$currentScene,$currentVolume,lowerThreshold) [$node GetLowerThreshold]
			set Preset(WindowLevel,$currentScene,$currentVolume,upperThreshold) [$node GetUpperThreshold]
			set Preset(WindowLevel,$currentScene,$currentVolume,window) [$node GetWindow]
			set Preset(WindowLevel,$currentScene,$currentVolume,level) [$node GetLevel]
			set Preset(WindowLevel,$currentScene,$currentVolume,autoWindowLevel) [$node GetAutoWindowLevel]
			set Preset(WindowLevel,$currentScene,$currentVolume,applyThreshold) [$node GetApplyThreshold]
			set Preset(WindowLevel,$currentScene,$currentVolume,autoThreshold) [$node GetAutoThreshold]
		}
		if {([string compare -length 12 $node "CrossSection"] == 0) && ($currentVolume != "")} {
			set pos [$node GetPosition]
			set Preset(Slices,$currentScene,$pos,orient) [$node GetDirection]
			set Preset(Slices,$currentScene,$pos,visibility) [$node GetInModel]
			set Preset(Slices,$currentScene,$pos,zoom) [$node GetZoom]
			set Preset(Slices,$currentScene,$pos,offset) [$node GetSliceSlider]
			set Preset(Slices,$currentScene,$pos,backVolID) [SharedVolumeLookup [$node GetBackVolRefID]]
			set Preset(Slices,$currentScene,$pos,foreVolID) [SharedVolumeLookup [$node GetForeVolRefID]]
			set Preset(Slices,$currentScene,$pos,labelVolID) [SharedVolumeLookup [$node GetLabelVolRefID]]
			set Preset(Slices,$currentScene,$pos,clipState) [$node GetClipState]
		}
		if {([string compare -length 7 $node "Locator"] == 0) && ($currentScene != "")} {
			set Preset(Locator,$currentScene,diffuseColor) [$node GetDiffuseColor]
			set Preset(Locator,$currentScene,driver) [$node GetDriver]
			set Preset(Locator,$currentScene,visibility) [$node GetVisibility]
			set Preset(Locator,$currentScene,transverseVisibility) [$node GetTransverseVisibility]
			set Preset(Locator,$currentScene,normalLen) [$node GetNormalLen]
			set Preset(Locator,$currentScene,transverseLen) [$node GetTransverseLen]
			set Preset(Locator,$currentScene,radius) [$node GetRadius]
		}
		if {([string compare -length 12 $node "SceneOptions"] == 0) && ($currentScene != "")} {
			set Preset(View,$currentScene,viewUp) [$node GetViewUp]
			set Preset(View,$currentScene,position) [$node GetPosition]
			set Preset(View,$currentScene,focalPoint) [$node GetFocalPoint]
			set Preset(View,$currentScene,clippingRange) [$node GetClippingRange]
			set Preset(View,$currentScene,viewMode) [$node GetViewMode]
			set Preset(View,$currentScene,viewBgColor) [$node GetViewBgColor]
			set Preset(Anno,$currentScene,box) [$node GetShowBox]
			set Preset(Anno,$currentScene,axes) [$node GetShowAxes]
			set Preset(Anno,$currentScene,outline) [$node GetShowSliceBounds]
			set Preset(Anno,$currentScene,letters) [$node GetShowLetters]
			set Preset(Anno,$currentScene,cross) [$node GetShowCross]
			set Preset(Anno,$currentScene,hashes) [$node GetShowHashes]
			set Preset(Anno,$currentScene,mouse) [$node GetShowMouse]
			set Preset(Volumes,$currentScene,DICOMStartDir) [$node GetDICOMStartDir]
			set Preset(Volumes,$currentScene,FileNameSortParam) [$node GetFileNameSortParam]
			set Preset(Volumes,$currentScene,DICOMPreviewWidth) [$node GetDICOMPreviewWidth]
			set Preset(Volumes,$currentScene,DICOMPreviewHeight) [$node GetDICOMPreviewHeight]
			set Preset(Volumes,$currentScene,DICOMPreviewHighestValue) [$node GetDICOMPreviewHighestValue]
			set Preset(Volumes,$currentScene,DICOMDataDictFile) [$node GetDICOMDataDictFile]
		}
		
		set node [Mrml(dataTree) GetNextItem]
	}
	
	foreach p $Preset(idList) {
		set Preset($p,state) Release
	}

	
	# Refresh view menu, but only if there are scenes in the MRML tree
	if {$Scenes(nameList) != ""} {
		$Gui(ViewMenuButton).m delete 0 last
		foreach scene $Scenes(nameList) {
			regsub -all "_" $scene " " scene2
			$Gui(ViewMenuButton).m add command -label "$scene2" -command "MainViewSelectView {$scene}"
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsUnparsePresets
# Makes MRML nodes out of the presets.
# Makes an Options node and adds it to data tree.
# .END
#-------------------------------------------------------------------------------
proc MainOptionsUnparsePresets {{presetNum ""}} {
	global Preset Mrml Options Module Model Volume
	global Scenes ModelState EndScenes VolumeState EndVolumeState CrossSection
	global SceneOptions Locator Matrix WindowLevel
	
	# Delete all scenes and other options nodes from the MRML tree
	# Uses ModelHierarchyDeleteNode to avoid a complete MRML update
	# after the deletion of each node
	
	Mrml(dataTree) InitTraversal
	set node [Mrml(dataTree) GetNextItem]
	set inScene 0
	
	while {$node != ""} {
		if {[string compare -length 6 $node "Scenes"] == 0} {
			ModelHierarchyDeleteNode "Scenes" [$node GetID]
			set inScene 1
		}
		if {[string compare -length 9 $node "EndScenes"] == 0} {
			ModelHierarchyDeleteNode "EndScenes" [$node GetID]
			set inScene 0
		}
		if {[string compare -length 10 $node "ModelState"] == 0} {
			ModelHierarchyDeleteNode "ModelState" [$node GetID]
		}
		if {[string compare -length 11 $node "VolumeState"] == 0} {
			ModelHierarchyDeleteNode "VolumeState" [$node GetID]
		}
		if {[string compare -length 14 $node "EndVolumeState"] == 0} {
			ModelHierarchyDeleteNode "EndVolumeState" [$node GetID]
		}
		if {[string compare -length 12 $node "CrossSection"] == 0} {
			ModelHierarchyDeleteNode "CrossSection" [$node GetID]
		}
		if {[string compare -length  12 $node "SceneOptions"] == 0} {
			ModelHierarchyDeleteNode "SceneOptions" [$node GetID]
		}
		if {[string compare -length 7 $node "Locator"] == 0} {
			ModelHierarchyDeleteNode "Locator" [$node GetID]
		}
		if {([string compare -length 6 $node "Matrix"] == 0) && ($inScene == 1)} {
			ModelHierarchyDeleteNode "Matrix" [$node GetID]
		}
		if {([string compare -length 11 $node "WindowLevel"] == 0) && ($inScene == 1)} {
			ModelHierarchyDeleteNode "WindowLevel" [$node GetID]
		}
		set node [Mrml(dataTree) GetNextItem]
	}
	
	# Store current settings as preset #0 (userOptions)
	set Preset($Preset(userOptions),state) Press
	MainOptionsPresetCallback $Preset(userOptions)
	set Preset($Preset(userOptions),state) Release

#	# Build an option string of attributes that differ from defaults
#	
#	# Foreach module, foreach preset button, foreach key, 
#	# does the value differ from the default?
#	#
#	set options ""
#	foreach m $Module(idList) {
#		if {[info exists Preset($m,keys)] == 1} {
#			set wrote 0
#			foreach key $Preset($m,keys) {
#				foreach p $Preset(idList) {
#					set name "$m,$p,$key"
#					puts $name
#					if {$Preset($name) != $Preset($m,default,$key)} {
#						set wrote 1
#						set options "$options $name='$Preset($name)'"
#					}
#				}
#			}
#			if {$wrote == 1} {
#				set options "$options\n"
#			}
#		}
#	}

	set wrote 0
	foreach p [concat {0} $Scenes(nameList)] {
		set node [MainMrmlAddNode "Scenes"]
		if {$p != "0"} {
			$node SetName $p
		} else {
			$node SetName default
		}
		
		if {[info exists Preset(Matrix,$p,name)] == 1} {
			set node [MainMrmlAddNode "Matrix"]
			$node SetName $Preset(Matrix,$p,name)
			$node SetMatrix "$Preset(Matrix,$p,matrix)"
		}
		
		# Save volume states
		
		if {[info exists Preset(Slices,keys)] == 1} {
			foreach v_id $Volume(idList) {
				set node [MainMrmlAddNode "VolumeState"]
				set v_name "V$v_id"
				$node SetVolumeRefID $v_name
				$node SetFade $Preset(Slices,$p,fade)
				$node SetOpacity $Preset(Slices,$p,opacity)
			
				if {[info exists Preset(WindowLevel,$p,$v_name,lowerThreshold)] == 1} {
					set node [MainMrmlAddNode "WindowLevel"]
					$node SetWindow $Preset(WindowLevel,$p,$v_name,window)
					$node SetLevel $Preset(WindowLevel,$p,$v_name,level)
					$node SetLowerThreshold $Preset(WindowLevel,$p,$v_name,lowerThreshold)
					$node SetUpperThreshold $Preset(WindowLevel,$p,$v_name,upperThreshold)
					$node SetAutoWindowLevel $Preset(WindowLevel,$p,$v_name,autoWindowLevel)
					$node SetApplyThreshold $Preset(WindowLevel,$p,$v_name,applyThreshold)
					$node SetAutoThreshold $Preset(WindowLevel,$p,$v_name,autoThreshold)
				}
				MainMrmlAddNode "EndVolumeState"
			}
			
			# Save slice options
			
			foreach pos {0 1 2} {
				set node [MainMrmlAddNode "CrossSection"]
				$node SetPosition $pos
				$node SetDirection $Preset(Slices,$p,$pos,orient)
				$node SetInModel $Preset(Slices,$p,$pos,visibility)
				$node SetZoom $Preset(Slices,$p,$pos,zoom)
				$node SetSliceSlider $Preset(Slices,$p,$pos,offset)
				$node SetBackVolRefID [Volume($Preset(Slices,$p,$pos,backVolID),node) GetVolumeID]
				$node SetForeVolRefID [Volume($Preset(Slices,$p,$pos,foreVolID),node) GetVolumeID]
				$node SetLabelVolRefID [Volume($Preset(Slices,$p,$pos,labelVolID),node) GetVolumeID]
				$node SetClipState $Preset(Slices,$p,$pos,clipState)
				
				set node [MainMrmlAddNode "Locator"]
				$node SetDiffuseColor $Preset(Locator,$p,diffuseColor)
				$node SetDriver $Preset(Locator,$p,$pos,driver)
				$node SetVisibility $Preset(Locator,$p,visibility)
				$node SetTransverseVisibility $Preset(Locator,$p,transverseVisibility)
				$node SetNormalLen $Preset(Locator,$p,normalLen)
				$node SetTransverseLen $Preset(Locator,$p,transverseLen)
				$node SetRadius $Preset(Locator,$p,radius)
			}
		}

		# Save scene options
		
		set node [MainMrmlAddNode "SceneOptions"]
		$node SetViewUp $Preset(View,$p,viewUp)
		$node SetPosition $Preset(View,$p,position)
		$node SetFocalPoint $Preset(View,$p,focalPoint)
		$node SetClippingRange $Preset(View,$p,clippingRange)
		$node SetViewMode $Preset(View,$p,viewMode)
		$node SetViewBgColor $Preset(View,$p,viewBgColor)
		$node SetShowBox $Preset(Anno,$p,box)
		$node SetShowAxes $Preset(Anno,$p,axes)
		$node SetShowSliceBounds $Preset(Anno,$p,outline)
		$node SetShowLetters $Preset(Anno,$p,letters)
		$node SetShowCross $Preset(Anno,$p,cross)
		$node SetShowHashes $Preset(Anno,$p,hashes)
		$node SetShowMouse $Preset(Anno,$p,mouse)
		$node SetDICOMStartDir $Preset(Volumes,$p,DICOMStartDir)
		$node SetFileNameSortParam $Preset(Volumes,$p,FileNameSortParam)
		$node SetDICOMDataDictFile $Preset(Volumes,$p,DICOMDataDictFile)
		$node SetDICOMPreviewWidth $Preset(Volumes,$p,DICOMPreviewWidth)
		$node SetDICOMPreviewHeight $Preset(Volumes,$p,DICOMPreviewHeight)
		$node SetDICOMPreviewHighestValue $Preset(Volumes,$p,DICOMPreviewHighestValue)
				

		# Models are a special case since the keys are not known ahead of time.
		# Use the current settings as the "defaults" to see if it is necessary
		# to save the presets.

		# Save model states
		
		foreach m $Model(idList) {
		
			# Careful: if the user never clicked the button, then the preset
			# doesn't exist.
			if {[info exists Preset(Models,$p,$m,visibility)] == 1} {
				set node [MainMrmlAddNode "ModelState"]
				$node SetModelRefID [Model($m,node) GetModelID]
				$node SetVisible $Preset(Models,$p,$m,visibility)
				$node SetOpacity $Preset(Models,$p,$m,opacity)
				$node SetClipping $Preset(Models,$p,$m,clipping)
				$node SetBackfaceCulling $Preset(Models,$p,$m,backfaceCulling)
			}
		}

		MainMrmlAddNode "EndScenes"
	}


#	# If we are saving user options in Options.xml, don't add node to tree.
#	if {$presetNum == $Preset(userOptions)} {
#	    return $options
#	}
#
#	# If a preset options node exists, edit it, else create one.
#	set tree Mrml(dataTree)
#	$tree InitTraversal
#	set node [$tree GetNextItem]
#       set found 0
#	while {$node != ""} {
#		set class [$node GetClassName]
#		if {$class == "vtkMrmlOptionsNode"} {
#			if {[$node GetContents] == "presets" && $found == 0} {
#				set found 1
#				$node SetOptions $options
#			}
#		}
#		set node [$tree GetNextItem]
#	}
#
#	if {$found == 0} {
#		# Create a node
#	        set n [MainMrmlAddNode Options]
#		$n SetOptions $options
#		$n SetProgram slicer
#		$n SetContents presets
#	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsPreset
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainOptionsPreset {p state} {
	global View Gui Preset

	if {$state == "Press"} {
		set Preset($p,state) $state
		after 250 "MainOptionsPresetCallback $p; RenderAll"
	} else {
		set Preset($p,state) $state
		#$View(fPreset).c$p config -activebackground $Gui(activeButton) 
	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsPresetCallback
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainOptionsPresetCallback {p} {
    global View Target Gui Preset Slice Locator Anno Model Options Module
    
    if {$Preset($p,state) == "Press"} {
	
	# Change button to red
	if {$p != $Preset(userOptions)} {
	    #$View(fPreset).c$p config -activebackground red
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
# 
# .ARGS
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
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainOptionsStorePresets {p} {
    global Module
    
    # Set preset value to the current
    foreach m $Module(procStorePresets) {
	$m $p
    }
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsPresetSaveCreateDialog
# 
# .ARGS
# widget: path of the widget that invokes this dialog
# .END
#-------------------------------------------------------------------------------
proc MainOptionsPresetSaveCreateDialog {widget} {
	global Gui

	if {[winfo exists .askforname] == 0} {
		toplevel .askforname -class Dialog -bg $Gui(activeWorkspace)
		wm title .askforname "Save view"
		# show the dialog at the original widget's position
		wm geometry .askforname +[winfo rootx $widget]+[winfo rooty $widget]
		eval {label .askforname.l1 -text "Enter the name of the view:"} $Gui(WLA)
		eval {entry .askforname.e1} $Gui(WEA)
		eval {button .askforname.bOk -text "Ok" -width 8 -command "MainOptionsPresetSaveOk"} $Gui(WBA)
		eval {button .askforname.bCancel -text "Cancel" -width 8 -command "destroy .askforname"} $Gui(WBA)
		grid .askforname.l1
		grid .askforname.e1
		grid .askforname.bOk .askforname.bCancel -padx 5 -pady 3
		
		# make the dialog modal
		update idle
		grab set .askforname
		tkwait window .askforname
		grab release .askforname
	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsPresetSaveOk
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainOptionsPresetSaveOk {} {
	global Scenes Preset Gui
	
	regsub -all " " [.askforname.e1 get] "_" currentScene
	destroy .askforname
	
	MainOptionsStorePresets $currentScene
	if {[lsearch $Scenes(nameList) $currentScene] == -1} {
		# add name to the scenes list, if not already present
		lappend Scenes(nameList) $currentScene
		lappend Preset(idList) $currentScene
	}
	
	# Refresh view menu
	$Gui(ViewMenuButton).m delete 0 last
	foreach scene $Scenes(nameList) {
		$Gui(ViewMenuButton).m add command -label "$scene" -command "MainViewSelectView {$scene}"
	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsPresetDeleteDialog
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainOptionsPresetDeleteDialog {widget} {
	global Gui
	
	set currentView [$Gui(ViewMenuButton) cget -text]
	if {$currentView=="Select"} {
		DevErrorWindow "No view selected."
	} else {
		YesNoPopup important_question\
			[winfo rootx $widget]\
			[winfo rooty $widget]\
			"Do you really want to remove $currentView?" "MainOptionsPresetDelete $currentView"
	}
}

#-------------------------------------------------------------------------------
# .PROC MainOptionsPresetDelete
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainOptionsPresetDelete {name} {
	global Scenes Preset Gui
	
	# delete the view from the scenes and preset lists
	set i [lsearch $Scenes(nameList) $name]
	set Scenes(nameList) [lreplace $Scenes(nameList) $i $i]
	set i [lsearch $Preset(idList) $name]
	set Preset(idList) [lreplace $Preset(idList) $i $i]
	$Gui(ViewMenuButton) configure -text "Select"
	
	# Refresh view menu
	$Gui(ViewMenuButton).m delete 0 last
	foreach scene $Scenes(nameList) {
		$Gui(ViewMenuButton).m add command -label "$scene" -command "MainViewSelectView {$scene}"
	}
}
