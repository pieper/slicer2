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
# FILE:        MainVolumes.tcl
# PROCEDURES:  
#   MainVolumesInit
#   MainVolumesBuildVTK
#   MainVolumesUpdateMRML
#   MainVolumesCopyData dst src clear
#   MainVolumesCreate
#   MainVolumesRead
#   MainVolumesWrite v prefix
#   MainVolumesDelete
#   MainVolumesBuildGUI
#   MainVolumesPopupGo
#   MainVolumesPopup
#   MainVolumesUpdate
#   MainVolumesRender
#   MainVolumesRenderActive
#   MainVolumesSetActive v
#   MainVolumesSetParam
#   MainVolumesUpdateSliderRange
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MainVolumesInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainVolumesInit {} {
	global Module Volume

	# Define Procedures
	lappend Module(procGUI)  MainVolumesBuildGUI
	lappend Module(procVTK)  MainVolumesBuildVTK
        
        set m MainVolumes

        # Set version info
        lappend Module(versions) [ParseCVSInfo $m \
		{$Revision: 1.45 $} {$Date: 2001/07/06 19:47:33 $}]

	set Volume(defaultOptions) "interpolate 1 autoThreshold 0  lowerThreshold -32768 upperThreshold 32767 showAbove -32768 showBelow 32767 edit None lutID 0 rangeAuto 1 rangeLow -1 rangeHigh 1001"

	set Volume(histWidth) 140
	set Volume(histHeight) 55

	# Append widgets to list that gets refreshed during UpdateMRML
	set Volume(mbActiveList) ""
	set Volume(mActiveList)  ""

	# Append widgets to list that's refreshed in MainVolumesUpdateSliderRange
	set Volume(sWindowList) ""
	set Volume(sLevelList) ""

	set Volume(idNone) 0
	set Volume(activeID)  $Volume(idNone)
	set Volume(freeze) ""
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainVolumesBuildVTK {} {
	global Volume Lut
	
	# Make the None Volume, which can never be deleted
	#---------------------------------------------------
	set v $Volume(idNone)

	vtkMrmlVolumeNode Volume($v,node)
	set n Volume($v,node)
	$n SetID $v
	$n SetName "None"
	$n SetDescription "NoneVolume=$v"
	$n SetLUTName 0

	vtkMrmlDataVolume Volume($v,vol)
	Volume($v,vol) SetMrmlNode         Volume($v,node)
	Volume($v,vol) SetHistogramWidth   $Volume(histWidth)
	Volume($v,vol) SetHistogramHeight  $Volume(histHeight)
	Volume($v,vol) SetStartMethod      MainStartProgress
	Volume($v,vol) SetProgressMethod  "MainShowProgress Volume($v,vol)"
	Volume($v,vol) SetEndMethod        MainEndProgress
	# Don't call the next line, because the Lut isn't created yet.
	# And, the None volume doesn't need it.
	# Volume($v,vol) SetLabelIndirectLUT Lut($Lut(idLabel),indirectLUT)

	# Have the slicer use this NoneVolume instead of its own creation
	Slicer SetNoneVolume Volume($v,vol)

	set Volume(0,dirty) 0
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainVolumesUpdateMRML {} {
	global Volume Lut Gui

	# Build any new volumes
	#--------------------------------------------------------
	foreach v $Volume(idList) {
		if {[MainVolumesCreate $v] > 0} {
			# Mark it as not being created on the fly 
			# since it was added from the Data module or read in from MRML
			set Volume($v,fly) 0

			if {[MainVolumesRead $v] < 0} {
			    # Let the user know about the error
			    tk_messageBox -message "Could not read volume [Volume($v,node) GetFullPrefix]."
			    # Failed, so axe it
			    MainMrmlDeleteNodeDuringUpdate Volume $v
			    
			}
		}
	}  

	# Delete any old volumes
	#--------------------------------------------------------
	foreach v $Volume(idListDelete) {
		MainVolumesDelete $v
	}

	# Did we delete the active volume?
	if {[lsearch $Volume(idList) $Volume(activeID)] == -1} {
		MainVolumesSetActive [lindex $Volume(idList) 0]
	}

	# Set the lut to use for label maps in each MrmlVolume 
	#--------------------------------------------------------
	foreach v $Volume(idList) {
		Volume($v,vol) SetLabelIndirectLUT Lut($Lut(idLabel),indirectLUT)
	}

	# Form the menus
	#--------------------------------------------------------
	# Active Volume menu
	foreach m $Volume(mActiveList) {
		$m delete 0 end
		foreach v $Volume(idList) {
			$m add command -label [Volume($v,node) GetName] \
				-command "MainVolumesSetActive $v"
		}
	}

	# Registration
	foreach v $Volume(idList) {
		if {$v != $Volume(idList)} {
			MainVolumesUpdate $v
		}
	}

	# In case we changed the name of the active transform
	MainVolumesSetActive $Volume(activeID)
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesCopyData
# 
# .ARGS
# int dst   The destination volume id.
# int src   The source volume id.
# str clear If \"On\", the output is set to all 0's. If \"Off\" the image is copied
# .END
#-------------------------------------------------------------------------------
proc MainVolumesCopyData {dst src clear} {
	global Volume Lut

	vtkImageCopy copy
	copy SetInput [Volume($src,vol) GetOutput]
	copy Clear$clear
	copy Update
	copy SetInput ""
	Volume($dst,vol) SetImageData [copy GetOutput]
	copy SetOutput ""
	copy Delete
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesCreate
#
# Returns:
#  1 - success
#  0 - already built this volume
# .END
#-------------------------------------------------------------------------------
proc MainVolumesCreate {v} {
	global View Volume Gui Dag Lut

	# If we've already built this volume, then do nothing
	if {[info command Volume($v,vol)] != ""} {
		return 0
	}

	# If no LUT name, use first LUT in the list
	if {[Volume($v,node) GetLUTName] == ""} {
		Volume($v,node) SetLUTName [lindex $Lut(idList) 0]
	}

	# Create vtkMrmlDataVolume
	vtkMrmlDataVolume Volume($v,vol)
	Volume($v,vol) SetMrmlNode          Volume($v,node)
	Volume($v,vol) SetLabelIndirectLUT  Lut($Lut(idLabel),indirectLUT)
	Volume($v,vol) SetLookupTable       Lut([Volume($v,node) GetLUTName],lut)
	Volume($v,vol) SetHistogramHeight   $Volume(histHeight)
	Volume($v,vol) SetHistogramWidth    $Volume(histWidth)
	Volume($v,vol) SetStartMethod       MainStartProgress
	Volume($v,vol) SetProgressMethod   "MainShowProgress Volume($v,vol)"
	Volume($v,vol) SetEndMethod         MainEndProgress

	# Label maps ALWAYS use the Label indirectLUT, and are not interpolated
	if {[Volume($v,node) GetLabelMap] == 1} {
		Volume($v,node) SetLUTName $Lut(idLabel)
		Volume($v,vol)  UseLabelIndirectLUTOn
		Volume($v,node) InterpolateOff
		Volume($v,vol) Update
	}

	# Mark it as unsaved and created on the fly.
	# If it actually isn't being created on the fly, I can't tell that from
	# inside this procedure, so the "fly" variable will be set to 0 in the
	# MainVolumesUpdateMRML procedure.
	set Volume($v,dirty) 1
	set Volume($v,fly) 1

	return 1
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesRead
#
# Returns:
#  1 - success
# -1 - failed to read files
# .END
#-------------------------------------------------------------------------------
proc MainVolumesRead {v} {
	global Volume Gui

	# Check that all files exist
	scan [Volume($v,node) GetImageRange] "%d %d" lo hi

    # Removed by Attila Tanacs 10/16/2000
    # Fixed by Attila Tanacs 6/14/2001

    set num [Volume($v,node) GetNumberOfDICOMFiles]
    if {$num == 0} {
	# if not DICOM, do the good old check
	if {[CheckVolumeExists [Volume($v,node) GetFullPrefix] \
		 [Volume($v,node) GetFilePattern] $lo $hi] != ""} {
	    return -1
	}
    } else {
	# DICOM requires another approach
	for {set i 0} {$i < $num} {incr i} {
	    if {[CheckFileExists [Volume($v,node) GetDICOMFileName $i] 0] == "0"} {
		return -1
	    }
	}
    }

    # End

	set Gui(progressText) "Reading [Volume($v,node) GetName]"

	puts "Reading volume: [Volume($v,node) GetName]..."
	Volume($v,vol) Read
	Volume($v,vol) Update
	puts "...finished reading [Volume($v,node) GetName]"

	# Mark this volume as saved
	set Volume($v,dirty) 0

	return 1
}
#-------------------------------------------------------------------------------
# .PROC MainVolumesWrite
# Writes out a volume created in the Slicer and an accompanying mrml file
# (the "Working.xml" file).
# 
# .ARGS
# int v ID number of the volume to write
# str prefix file prefix where the volume will be written
# .END
#-------------------------------------------------------------------------------
proc MainVolumesWrite {v prefix} {
	global Volume Gui Mrml tcl_platform

	if {$v == ""} {
		return
	}
	if {$prefix == ""} {
		tk_messageBox -message "Please provide a file prefix."
		return
	}

	# So don't write it if it's not dirty.
	if {$Volume($v,dirty) == 0} {
	    set answer [tk_messageBox -type yesno -message \
			    "This volume shold not be saved\nbecause it has not been changed\n\
 since the last time it was saved.\nDo you really want to save it?"]
	    if {$answer == "no"} {
		return
	    }
	}
    
	# Form and check file prefix
	set filePrefix $prefix
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

	# the MRML file will go in the directory where the volume was saved.
	# So the relative file prefix is just the name of the file.
	set name [file root [file tail $fileFull]]
	Volume($v,node) SetFilePrefix $name
	# Tell volume node where it should be written
	Volume($v,node) SetFullPrefix $fileFull

	# Determine if littleEndian
	if {$tcl_platform(byteOrder) == "littleEndian"} {
	    Volume($v,node) SetLittleEndian 1
	} else {
	    Volume($v,node) SetLittleEndian 0
	}

	# Write volume data
	set Gui(progressText) "Writing [Volume($v,node) GetName]"
	puts "Writing '$fileFull' ..."
	Volume($v,vol) Write
	puts " ...done."

	# put MRML file in dir where volume was saved, name it after the volume
	set filename [file join [file dirname $fileFull] $name.xml]

	# Write MRML file
	vtkMrmlTree tree
	tree AddItem Volume($v,node)
	tree Write $filename
	tree RemoveAllItems
	tree Delete
	puts "Saved MRML file: $filename"

	# Reset the pathnames to be relative to Mrml(dir)
	Volume($v,node) SetFilePrefix $filePrefix
	Volume($v,node) SetFullPrefix $fileFull

	# Wrote it, so not dirty (changed since read/wrote)
	set Volume($v,dirty) 0
}


#-------------------------------------------------------------------------------
# .PROC MainVolumesDelete
#
# DAVE fix
# Returns:
#  1 - success
#  0 - already deleted this volume
# .ARG
#   int m the id number of the volume to be deleted.
# .END
#-------------------------------------------------------------------------------
proc MainVolumesDelete {v} {
	global Dag Volume

	# If we've already deleted this volume, then return 0
	if {[info command Volume($v,vol)] == ""} {
		return 0
	}

	# Delete VTK objects (and remove commands from TCL namespace)
	Volume($v,vol)  Delete

	# Delete all TCL variables of the form: Volume($v,<whatever>)
	foreach name [array names Volume] {
		if {[string first "$v," $name] == 0} {
			unset Volume($name)
		}
	}

	return 1
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesBuildGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainVolumesBuildGUI {} {
	global fSlicesGUI Gui Model Slice Volume Lut

	#-------------------------------------------
	# Volumes Popup Window
	#-------------------------------------------
	set w .wVolumes
	set Gui(wVolumes) $w
	toplevel $w -bg $Gui(inactiveWorkspace) -class Dialog
	wm title $w "Volumes"
	wm iconname $w Dialog
	wm protocol $w WM_DELETE_WINDOW "wm withdraw $w"
	if {$Gui(pc) == "0"} {
		wm transient $w .
	}
	wm withdraw $w
	set f $w

	# Close button
	eval {button $f.bClose -text "Close" -command "wm withdraw $w"} $Gui(WBA)

	# Frames
	frame $f.fActive -bg $Gui(inactiveWorkspace)
	frame $f.fWinLvl -bg $Gui(activeWorkspace) -bd 2 -relief raised
	frame $f.fThresh -bg $Gui(activeWorkspace) -bd 2 -relief raised
	pack $f.fActive -side top -pady $Gui(pad) -padx $Gui(pad)
	pack $f.fWinLvl $f.fThresh -side top -pady $Gui(pad) -padx $Gui(pad) -fill x
	pack $f.bClose -side top -pady $Gui(pad)

	#-------------------------------------------
	# Popup->Active frame
	#-------------------------------------------
	set f $w.fActive

	eval {label $f.lActive -text "Active Volume: "} $Gui(WLA)\
		{-bg $Gui(inactiveWorkspace)}
	eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m} $Gui(WMBA)
	eval {menu $f.mbActive.m} $Gui(WMA)
	pack $f.lActive $f.mbActive -side left -padx $Gui(pad) -pady 0 

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Volume(mbActiveList) $f.mbActive
	lappend Volume(mActiveList)  $f.mbActive.m

	#-------------------------------------------
	# Popup->WinLvl frame
	#-------------------------------------------
	set f $w.fWinLvl

	#-------------------------------------------
	# Auto W/L
	#-------------------------------------------
	eval {label $f.lAuto -text "Window/Level:"} $Gui(WLA)
	frame $f.fAuto -bg $Gui(activeWorkspace)
	grid $f.lAuto $f.fAuto -pady $Gui(pad)  -padx $Gui(pad) -sticky e
	grid $f.fAuto -columnspan 2 -sticky w

	foreach value "1 0" text "Auto Manual" width "5 7" {
		eval {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
			-text "$text" -value "$value" -variable Volume(autoWindowLevel) \
			-command "MainVolumesSetParam AutoWindowLevel; MainVolumesRender" \
			} $Gui(WCA)
		pack $f.fAuto.rAuto$value -side left -fill x
	}

	#-------------------------------------------
	# W/L Sliders
	#-------------------------------------------
	foreach slider "Window Level" {
		eval {label $f.l${slider} -text "${slider}:"} $Gui(WLA)
		eval {entry $f.e${slider} -width 7 \
			-textvariable Volume([Uncap ${slider}])} $Gui(WEA)
		bind $f.e${slider} <Return>   \
			"MainVolumesSetParam ${slider}; MainVolumesRender"
		bind $f.e${slider} <FocusOut> \
			"MainVolumesSetParam ${slider}; MainVolumesRender"
		eval {scale $f.s${slider} -from 1 -to 1024 \
			-variable Volume([Uncap ${slider}]) -length 200 -resolution 1 \
			-command "MainVolumesSetParam ${slider}; MainVolumesRenderActive"\
			 } $Gui(WSA)
		bind $f.s${slider} <Leave> "MainVolumesRender"
		grid $f.l${slider} $f.e${slider} $f.s${slider} \
			-pady $Gui(pad) -padx $Gui(pad)
		grid $f.l$slider -sticky e
		grid $f.s$slider -sticky w
		set Volume(s$slider) $f.s$slider
	}
	# Append widgets to list that's refreshed in MainVolumesUpdateSliderRange
	lappend Volume(sWindowList) $f.sWindow
	lappend Volume(sLevelList) $f.sLevel

	#-------------------------------------------
	# Popup->Thresh frame
	#-------------------------------------------
	set f $w.fThresh

	#-------------------------------------------
	# Auto Threshold
	#-------------------------------------------
	eval {label $f.lAuto -text "Threshold:"} $Gui(WLA)
	frame $f.fAuto -bg $Gui(activeWorkspace)
	grid $f.lAuto $f.fAuto -pady $Gui(pad) -padx $Gui(pad) -sticky e
	grid $f.fAuto -columnspan 2 -sticky w

	foreach value "1 0" text "Auto Manual" width "5 7" {
		eval {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
			-text "$text" -value "$value" -variable Volume(autoThreshold) \
			-command "MainVolumesSetParam AutoThreshold; MainVolumesRender"} $Gui(WCA)
	}
	eval {checkbutton $f.cApply \
		-text "Apply" -variable Volume(applyThreshold) \
		-command "MainVolumesSetParam ApplyThreshold; MainVolumesRender" -width 6 \
		-indicatoron 0} $Gui(WCA)
	
	grid $f.fAuto.rAuto1 $f.fAuto.rAuto0 $f.cApply
	grid $f.cApply -padx $Gui(pad)

	#-------------------------------------------
	# Threshold Sliders
	#-------------------------------------------
	foreach slider "Lower Upper" {
		eval {label $f.l${slider} -text "${slider}:"} $Gui(WLA)
		eval {entry $f.e${slider} -width 7 \
			-textvariable Volume([Uncap ${slider}]Threshold)} $Gui(WEA)
			bind $f.e${slider} <Return>   \
				"MainVolumesSetParam ${slider}Threshold; MainVolumesRender"
			bind $f.e${slider} <FocusOut> \
				"MainVolumesSetParam ${slider}Threshold; MainVolumesRender"
		eval {scale $f.s${slider} -from 1 -to 1024 \
			-variable Volume([Uncap ${slider}]Threshold) -length 200 -resolution 1 \
			-command "MainVolumesSetParam ${slider}Threshold; MainVolumesRender"\
			 } $Gui(WSA)
		grid $f.l${slider} $f.e${slider} $f.s${slider} \
			 -padx $Gui(pad) -pady $Gui(pad)
		grid $f.l$slider -sticky e
		grid $f.s$slider -sticky w
		set Volume(s$slider) $f.s$slider
	}
	# Append widgets to list that's refreshed in MainVolumesUpdateSliderRange
	lappend Volume(sLevelList) $f.sLower
	lappend Volume(sLevelList) $f.sUpper

}

#-------------------------------------------------------------------------------
# .PROC MainVolumesPopupGo
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainVolumesPopupGo {Layer s X Y} {
	global Gui

	set vol [Slicer Get${Layer}Volume $s]
	if {$vol == ""} {
		set v ""
	} else {
		set v [[$vol GetMrmlNode] GetID]
	}

	MainVolumesPopup $v $X $Y
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesPopup
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainVolumesPopup {v X Y} {
	global Gui

	# Recreate window if user killed it
	if {[winfo exists $Gui(wVolumes)] == 0} {
		MainVolumesBuildGUI
	}
	
	MainVolumesSetActive $v

	ShowPopup $Gui(wVolumes) 0 0
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesUpdate
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainVolumesUpdate {v} {
	global Volume Slice 

	set n $Volume(idNone)

	# Update pipeline
	Volume($v,vol) Update
	foreach s $Slice(idList) {
 		if {$v == $Slice($s,backVolID)} {
			Slicer SetBackVolume $s Volume($n,vol)
			Slicer SetBackVolume $s Volume($v,vol)
		}
 		if {$v == $Slice($s,foreVolID)} {
			Slicer SetForeVolume $s Volume($n,vol)
			Slicer SetForeVolume $s Volume($v,vol)
		}
 		if {$v == $Slice($s,labelVolID)} {
			Slicer SetLabelVolume $s Volume($n,vol)
			Slicer SetLabelVolume $s Volume($v,vol)
		}
		MainSlicesSetOffset $s
	}
	Slicer ReformatModified
	Slicer Update

	# Update GUI
	if {$v == $Volume(activeID)} {
		# Refresh Volumes GUI with active volume's parameters
		MainVolumesSetActive $v
	}
	# The BuildUpper() function reset the offsets to be in the middle of
	# the volume, so I need to set them to what's on the GUI:
	foreach s $Slice(idList) {
		Slicer SetOffset $s $Slice($s,offset)
	}

}

#-------------------------------------------------------------------------------
# .PROC MainVolumesRender
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainVolumesRender {{scale ""}} {
	global Volume Slice 

	# Update slice that has this volume as input
	set v $Volume(activeID)

	set hit 0
	foreach s $Slice(idList) {
 		if {$v == $Slice($s,backVolID) || $v == $Slice($s,foreVolID)} {
			set hit 1
			Volume($v,vol) Update
			RenderSlice $s
		}
	}
	if {$hit == 1} {
		Render3D
	}
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesRenderActive
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainVolumesRenderActive {{scale ""}} {
	global Volume Slice 

	# Update slice that has this volume as input
	set v $Volume(activeID)

	set s $Slice(activeID)
 	if {$v == $Slice($s,backVolID) || $v == $Slice($s,foreVolID)} {
		Volume($v,vol) Update
		RenderSlice $s
	} else {
		MainVolumesRender
	}
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesSetActive
# 
# .ARGS
# int v The id of the volume to set active 
# .END
#-------------------------------------------------------------------------------
proc MainVolumesSetActive {v} {
	global Volume Lut Slice

	if {$Volume(freeze) == 1} {return}
	
	set Volume(activeID) $v
	if {$v == ""} {
		set Volume(activeID) $Volume(idNone)
	}

	if {$v == "NEW"} {
		
		# Change button text
		foreach mb $Volume(mbActiveList) {
			$mb config -text "NEW"
		}

		if {[IsModule Volumes] == 1} {		
		    # Use defaults to update GUI
		    MainVolumesSetGUIDefaults
		    # Update buttons
		    VolumesSetScanOrder $Volume(scanOrder)
		    VolumesSetScalarType $Volume(scalarType)

		    # Added by Attila Tanacs 10/17/2000
		    $Volume(dICOMFileListbox) delete 0 end
		    set Volume(dICOMFileList) {}
		    # End
		}

	} else {

		# Change button text
		foreach mb $Volume(mbActiveList) {
			$mb config -text [Volume($v,node) GetName]
		}

		# Slider range 
		# WARNING: This obviously must be set before window/level
		set Volume(rangeLow)    [Volume($v,vol) GetRangeLow]
		set Volume(rangeHigh)   [Volume($v,vol) GetRangeHigh]
		set Volume(rangeAuto)   [Volume($v,vol) GetRangeAuto]
		MainVolumesUpdateSliderRange

		# Update GUI
		foreach item "Window Level AutoWindowLevel UpperThreshold LowerThreshold \
			AutoThreshold ApplyThreshold Interpolate" {
			set Volume([Uncap $item]) [Volume($v,node) Get$item]
		}

		if {[IsModule Volumes] == 1} {

			# LUT menu
			$Volume(mbLUT) config -text $Lut([Volume($v,node) GetLUTName],name)

			if {$Volume(histogram) == "On"} {
				histMapper SetInput [Volume($v,vol) GetHistogramPlot]
				histWin Render
			}

			# Change button text on slice menu (in case name changed)
			foreach s $Slice(idList) {
 				if {$v == $Slice($s,backVolID)} {
					MainSlicesConfigGui $s fOrient.mbBackVolume$s \
						"-text [Volume($v,node) GetName]"
				}
				if {$v == $Slice($s,foreVolID)} {
					MainSlicesConfigGui $s fVolume.mbForeVolume$s \
						"-text [Volume($v,node) GetName]"
				}
				if {$v == $Slice($s,labelVolID)} {
					MainSlicesConfigGui $s fVolume.mbLabelVolume$s \
						"-text [Volume($v,node) GetName]"
				}
			}

			# Update Volumes->Props GUI
			foreach item "filePattern gantryDetectorTilt numScalars \
				name desc labelMap littleEndian" vtkname "FilePattern Tilt \
				NumScalars Name Description LabelMap LittleEndian" {
			    set Volume($item) [Volume($v,node) Get$vtkname]
            }
            
            # Added by Attila Tanacs 10/10/2000
            set Volume(dICOMFileList) {}
            for  {set i 0} {$i < [Volume($v,node) GetNumberOfDICOMFiles]} {incr i} {
                lappend Volume(dICOMFileList) [Volume($v,node) GetDICOMFileName $i]
            }
            $Volume(dICOMFileListbox) delete 0 end
            eval {$Volume(dICOMFileListbox) insert end} $Volume(dICOMFileList)
            # End

			# update menus
			VolumesSetScalarType [lindex "Char UnsignedChar Short {UnsignedShort} \ 
			{Int} UnsignedInt Long UnsignedLong Float Double" \
				[lsearch "2 3 4 5 6 7 8 9 10 11"  [Volume($v,node) GetScalarType]]]
			VolumesSetScanOrder [Volume($v,node) GetScanOrder]

			scan [Volume($v,node) GetImageRange] "%d %d" lo hi
			set Volume(lastNum) $hi
			set Volume(firstFile) [format $Volume(filePattern) \
				[Volume($v,node) GetFilePrefix] $lo]

			scan [Volume($v,node) GetSpacing] "%f %f %f" pix pix thick
			set Volume(pixelSize) $pix
			set Volume(sliceThickness) $thick
			# display default for spacing
			set Volume(sliceSpacing) 0

			scan [Volume($v,node) GetDimensions] "%d %d" width height
			set Volume(width) $width
			set Volume(height) $height

			# display default for readHeaders
			set Volume(readHeaders) 1
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesSetParam
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainVolumesSetParam {Param {value ""}} {
	global Volume Slice Lut

	# Initialize param, v, value
	set param [Uncap $Param]
	set v $Volume(activeID)
	if {$value == ""} {
		set value $Volume($param)
	} else {
		set Volume($param) $value
	}

	#
	# Window/Level/Threshold
	#
	if {[lsearch "AutoWindowLevel Level Window UpperThreshold LowerThreshold \
		AutoThreshold ApplyThreshold" $Param] != -1} {

		# If no change, return
		if {$value == [Volume($v,node) Get$Param]} {return}

		# Update value
		Volume($v,node) Set$Param $value

		# If changing window/level, then turn off AutoWindowLevel
		if {[lsearch "Level Window" $Param] != -1} {
			set Volume(autoWindowLevel) 0
			Volume($v,node) SetAutoWindowLevel $Volume(autoWindowLevel)
		}

		# If AutoWindowLevel, get the resulting window/level
		if {$Param == "AutoWindowLevel" && $value == 1} {
			Volume($v,vol) Update
			set Volume(window) [Volume($v,node) GetWindow]
			set Volume(level)  [Volume($v,node) GetLevel]
		}

		# If changing threshold, then turn off AutoThreshold
		if {[lsearch "UpperThreshold LowerThreshold" $Param] != -1} {
			set Volume(autoThreshold) 0
			Volume($v,node) SetAutoThreshold $Volume(autoThreshold)
		}

		# If changing threshold, then turn on ApplyThreshold
		if {[lsearch "UpperThreshold LowerThreshold AutoThreshold" $Param] != -1} {
			set Volume(applyThreshold) 1
			Volume($v,node) SetApplyThreshold $Volume(applyThreshold)
		}

		# If AutoThreshold, get the resulting upper/lower threshold
		if {$Param == "AutoThreshold"} {
			Volume($v,vol) Update
			set Volume(lowerThreshold) [Volume($v,node) GetLowerThreshold]
			set Volume(upperThreshold) [Volume($v,node) GetUpperThreshold]
		}

		if {$Param == "ApplyoThreshold"} {
			Volume($v,vol) Update
		}

	#
	# Range
	#
	} elseif {[lsearch "RangeAuto RangeLow RangeHigh" $Param] != -1} {

		# If no change, return
		if {$value == [Volume($v,vol) Get$Param]} {return}

		# Update value
		Volume($v,vol) Set$Param $value

		# If changing range, then turn off RangeAuto
		if {[lsearch "RangeLow RangeHigh" $Param] != -1} {
			set Volume(rangeAuto) 0
			Volume($v,vol) SetRangeAuto $Volume(rangeAuto)
		}

		# Clip window/level/threshold with the range
		Volume($v,vol) Update
		foreach item "Window Level UpperThreshold LowerThreshold" {
			set Volume([Uncap $item]) [Volume($v,node) Get$item]
		}

		# If RangeAuto, get the resulting range
		if {$Param == "RangeAuto" && $value == 1} {
			set Volume(rangeLow)  [Volume($v,vol) GetRangeLow]
			set Volume(rangeHigh) [Volume($v,vol) GetRangeHigh]
			MainVolumesUpdateSliderRange		

			# Refresh window/level/threshold
			set Volume(window) [Volume($v,node) GetWindow]
			set Volume(level)  [Volume($v,node) GetLevel]
			if {$Volume(autoThreshold) == "-1"} {
				Volume($v,node) SetLowerThreshold [Volume($v,vol) GetRangeLow]
				Volume($v,node) SetUpperThreshold [Volume($v,vol) GetRangeHigh]
			}
			set Volume(lowerThreshold) [Volume($v,node) GetLowerThreshold]
			set Volume(upperThreshold) [Volume($v,node) GetUpperThreshold]
		} else {
			MainVolumesUpdateSliderRange		
		}
	#
	# LUT
	#
	} elseif {$Param == "LutID"} {

		# Label 
		if {$value == $Lut(idLabel)} {
			Volume($v,vol) UseLabelIndirectLUTOn
		} else {
			Volume($v,vol) UseLabelIndirectLUTOff
			Volume($v,vol) SetLookupTable Lut($value,lut)
		}
		Volume($v,vol) Update

		Volume($v,node) SetLUTName $value
	
		if {[IsModule Volumes] == 1} {
			$Volume(mbLUT) config -text $Lut($value,name)
		}

		# Color of line in histogram
		eval Volume($v,vol) SetHistogramColor $Lut($value,annoColor)

		# Set LUT in mappers
		Slicer ReformatModified
		Slicer Update

	# 
	# Interpolate
	#
	} elseif {$Param == "Interpolate"} {
		Volume($v,node) SetInterpolate $value

		# Notify the Slicer that it needs to refresh the reformat portion
		# of the imaging pipeline
		Slicer ReformatModified
		Slicer Update

		Volume($v,vol) Update

	# 
	# Booboo
	#
 	} else {
		puts "MainVolumesSetParam: Unknown param=$param"
		return
	}

	if {[IsModule Volumes] == 1 && $Volume(histogram) == "On"} {
		# Recall GetHistogramPlot so the HistogramColor is updated
		histMapper SetInput [Volume($v,vol) GetHistogramPlot]
		histWin Render
	}
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesUpdateSliderRange
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainVolumesUpdateSliderRange {} {
	global Volume

	# Change GUI
	# width = hi - lo + 1 = (hi+1) - (lo-1) - 1
	set width [expr $Volume(rangeHigh) - $Volume(rangeLow) - 1]
	if {$width < 1} {set width 1}

	foreach s $Volume(sLevelList) {
		$s config -from $Volume(rangeLow) -to $Volume(rangeHigh)
	}
	foreach s $Volume(sWindowList) {
		$s config -from 1 -to $width
	}
}

# Set defaults for the Volumes-> Props GUI.
proc MainVolumesSetGUIDefaults {} {
    global Volume

    # Get defaults from VTK 
    vtkMrmlVolumeNode default
    set Volume(name) [default GetName]
    set Volume(filePattern) %s.%03d
    set Volume(scanOrder) [default GetScanOrder]
    set Volume(littleEndian) [default GetLittleEndian]
    #set Volume(resolution) [lindex [default GetDimensions] 0]
    set Volume(width) [lindex [default GetDimensions] 0]
    set Volume(height) [lindex [default GetDimensions] 1]
    set spacing [default GetSpacing]
    set Volume(pixelSize) [lindex $spacing 0]
    set Volume(sliceThickness) [lindex $spacing 2]
    set Volume(sliceSpacing) 0.0
    set Volume(gantryDetectorTilt) [default GetTilt]
    set Volume(desc) [default GetDescription]
    set Volume(numScalars) [default GetNumScalars]
    set Volume(scalarType) [lindex "Char UnsignedChar Short {UnsignedShort} \ 
    {Int} UnsignedInt Long UnsignedLong Float Double"\
	    [lsearch "2 3 4 5 6 7 8 9 10 11"  [default GetScalarType]]]
    default Delete

    # Added by Attila Tanacs 10/10/2000
    set Volume(numDICOMFiles) 0
    # End
    
    # Set GUI defaults
    set Volume(firstFile) ""
    set Volume(readHeaders) 1
    set Volume(labelMap) 0
    set Volume(lastNum) ""
}
