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
# FILE:        MainVolumes.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   MainVolumesInit
#   MainVolumesBuildVTK
#   MainVolumesBuildMRML
#   MainVolumesCreate
#   MainVolumesDelete
#   MainVolumesBuildGUI
#   MainVolumesPopupGo
#   MainVolumesPopup
#   MainVolumesRender
#   MainVolumesRenderActive
#   MainVolumesSetActive
#   MainVolumesSetParam
#   MainVolumesUpdateSliderRange
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MainVolumesInit
# .END
#-------------------------------------------------------------------------------
proc MainVolumesInit {} {
	global Module Volume

	lappend Module(procGUI)  MainVolumesBuildGUI
	lappend Module(procVTK)  MainVolumesBuildVTK

	set Volume(numBuiltIn) 1
	set Volume(defaultOptions) "interpolate 1 autoThreshold 0  lowerThreshold -32768 upperThreshold 32767 showAbove -32768 showBelow 32767 edit None lutID 0 rangeAuto 1 rangeLow -1 rangeHigh 1001"

	set Volume(histWidth) 140
	set Volume(histHeight) 55

	# Append widgets to list that gets refreshed during UpdateMRML
	set Volume(mbActiveList) ""
	set Volume(mActiveList)  ""

	# Append widgets to list that's refreshed in MainVolumesUpdateSliderRange
	set Volume(sWindowList) ""
	set Volume(sLevelList) ""

	set Volume(idNone)      0
	set Volume(activeID)  $Volume(idNone)
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesBuildVTK
# .END
#-------------------------------------------------------------------------------
proc MainVolumesBuildVTK {} {
	global Volume Lut
	
	# Add Volumes for  None, Working, Result

	# None
	# -----------------------------------------------------
	set v $Volume(idNone)

	# MrmlVolumeNode
	vtkMrmlVolumeNode Volume($v,node)
	Volume($v,node) SetID $v
	Volume($v,node) SetDescription "NoneVolume=$v"
	Volume($v,node) SetName "None"
	Volume($v,node) SetLUTName 0

	# MrmlVolume
	vtkMrmlVolume Volume($v,vol)
	Volume($v,vol) SetMrmlNode Volume($v,node)
	Volume($v,vol) SetHistogramWidth $Volume(histWidth)
	Volume($v,vol) SetHistogramHeight $Volume(histHeight)

	# Have the slicer use this NoneVolume instead of its own
	Slicer SetNoneVolume Volume($v,vol)
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesBuildMRML
# .END
#-------------------------------------------------------------------------------
proc MainVolumesBuildMRML {} {
	global Volume Lut Gui

	# Build any new volumes
	#--------------------------------------------------------
	foreach v $Volume(idList) {
		if {[MainVolumesCreate $v] == 1} {
			# Success
		}
	}    

	# Delete any old volumes
	#--------------------------------------------------------
	foreach v $Volume(idListDelete) {
		if {[MainVolumesDelete $v] == 1} {
			# Success
		}
	}

	# Set the lut to use for label maps in each MrmlVolume 
	#--------------------------------------------------------
	foreach v $Volume(idList) {
		Volume($v,vol) SetLabelIndirectLUT Lut($Lut(idLabel),indirectLUT)
	}

	# Form the menus with the built-in volumes at the bottom.
	# (Move the built-in from the front of the list to the end)
	#--------------------------------------------------------
	set Volume(idListForMenu) \
		"[lreplace $Volume(idList) 0 [expr $Volume(numBuiltIn) - 1]] \
		$Volume(idNone)"

	# Active Volume menu
	foreach m $Volume(mActiveList) {
		$m delete 0 end
		foreach v $Volume(idListForMenu) {
			$m add command -label [Volume($v,node) GetName] \
				-command "MainVolumesSetActive $v"
		}
	}

	# The active volume may have been deleted 
	# DAVE
	#--------------------------------------------------------
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesCreate
#
# Returns:
#  1 - success
#  0 - already built this volume
# -1 - failed to read files
# .END
#-------------------------------------------------------------------------------
proc MainVolumesCreate {v} {
	global View Volume Gui Dag Lut

	# If we've already built this volume, then do nothing
	if {[info command Volume($v,node)] != ""} {
		return 0
	}

	# Skip imaginary volumes (id < 0)
	if {$v <= 0} {return 0}
	
	# Check that all files exist
	if {[CheckVolumeExists $Volume($v,filePrefix) \
		$Volume($v,filePattern) [lindex $Volume($v,imageRange) 0]\
		[lindex $Volume($v,imageRange) 1]] != ""} {
		set str "Unable to build the VTK objects for Volumes."
		puts $str
		tk_messageBox -message $str
		return -1
	}

	# If we don't specify headerSize, then it just reads the
	# last bytes in the file as the pictures.  This allows
	# variable file header lengths, as in is true of DICOM.

	# DAVE: don't interpolate label maps
	if {$Volume($v,labelMap) == 1} {
		set Volume($v,interpolate) 0
	}

	vtkMrmlVolumeNode Volume($v,node)
	Volume($v,node) SetID                        $v
	Volume($v,node) SetDescription               $Volume($v,desc)
	Volume($v,node) SetOptions                   $Volume($v,options)
	eval Volume($v,node) SetImageRange           $Volume($v,imageRange)
	eval Volume($v,node) SetDimensions           $Volume($v,dimensions)
	eval Volume($v,node) SetSpacing              $Volume($v,spacing)
	Volume($v,node) SetScalarTypeTo$Volume($v,scalarType)
	Volume($v,node) SetNumScalars                $Volume($v,numScalars)
	Volume($v,node) SetLittleEndian              $Volume($v,littleEndian)
	Volume($v,node) SetTilt                      $Volume($v,tilt)
	Volume($v,node) SetScanOrder                 $Volume($v,scanOrder)
	Volume($v,node) SetRasToIjkMatrix            $Volume($v,rasToIjkMatrix)
	Volume($v,node) SetRasToVtkMatrix            $Volume($v,rasToVtkMatrix)
	Volume($v,node) SetFilePrefix                $Volume($v,filePrefix)
	Volume($v,node) SetFilePattern               $Volume($v,filePattern)
	Volume($v,node) SetName                      $Volume($v,name)
	Volume($v,node) SetFullPrefix                $Volume($v,filePrefix)
	Volume($v,node) UseRasToVtkMatrixOn
	Volume($v,node) SetLUTName                   $Volume($v,lutID)
	Volume($v,node) SetLabelMap                  $Volume($v,labelMap)
	Volume($v,node) SetAutoWindowLevel           $Volume($v,autoWindowLevel)
	Volume($v,node) SetWindow                    $Volume($v,window)
	Volume($v,node) SetLevel                     $Volume($v,level)
	Volume($v,node) SetAutoThreshold             $Volume($v,autoThreshold)
	Volume($v,node) SetUpperThreshold            $Volume($v,upperThreshold)
	Volume($v,node) SetLowerThreshold            $Volume($v,lowerThreshold)
	Volume($v,node) SetInterpolate               $Volume($v,interpolate)

	# Registration
	vtkMatrix4x4 mat
	set matrixList $Volume($v,rasToRefMatrix)
	for {set row 0} { $row < 4 } {incr row} {
		for {set col 0} {$col < 4} {incr col} {
			mat SetElement $row $col [lindex $matrixList [expr $row*4+$col]]
		}
	}
	Volume($v,node) SetRasToWld mat
	mat Delete

	vtkMrmlVolume Volume($v,vol)
	Volume($v,vol) SetMrmlNode Volume($v,node)
	Volume($v,vol) SetLabelIndirectLUT Lut($Lut(idLabel),indirectLUT)
	Volume($v,vol) UseLabelIndirectLUTOff
	Volume($v,vol) SetLookupTable Lut([Volume($v,node) GetLUTName],lut)
	Volume($v,vol) SetHistogramHeight $Volume(histHeight)
	Volume($v,vol) SetHistogramWidth  $Volume(histWidth)
	Volume($v,vol) RangeAutoOn

	# Label maps use the Label indirectLUT
	if {$Volume($v,labelMap) == 1} {
		Volume($v,vol) UseLabelIndirectLUTOn
	}	

	Volume($v,vol) SetStartMethod     MainStartProgress
	Volume($v,vol) SetProgressMethod "MainShowProgress Volume($v,vol)"
	Volume($v,vol) SetEndMethod       MainEndProgress
	set Gui(progressText) "Reading [Volume($v,node) GetName]"

	puts "Reading volume: [Volume($v,node) GetName]..."
	Volume($v,vol) Read
	Volume($v,vol) Update
	puts "...finished reading [Volume($v,node) GetName]"

	return 1
}


#-------------------------------------------------------------------------------
# .PROC MainVolumesDelete
#
# DAVE fix
# Returns:
#  1 - success
#  0 - already deleted this volume
# .END
#-------------------------------------------------------------------------------
proc MainVolumesDelete {v} {
	global Dag Volume

	# If we've already deleted this volume, then return 0
	if {[info command Volume($v,node)] == ""} {
		return 0
	}

	# Skip imaginary volumes (id <= 0)
	if {$v <= 0} {return 0}
	
	# Delete VTK objects (and remove commands from TCL namespace)
	Volume($v,vol)  Delete
	Volume($v,node) Delete

	# Delete all TCL variables of the form: Volume($v,<whatever>)
	foreach name [array names Volume] {
		if {[string first "$v," $name] == 0} {
			unset Volume($name)
		}
	}

	# Delete ID from array
	set i [lsearch $Volume(idList) $v]
	set Volume(idList) [lreplace $Volume(idList) $i $i]

	# Delete node from dag
	set n [MRMLGetIndexOfNodeInDag $Dag(current) $v "Volume"]
	set Dag(current) [MRMLDeleteNode $Dag(current) $n]

	return 1
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesBuildGUI
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
	wm transient $w .
	wm withdraw $w
	set f $w

	# Close button
	set c {button $f.bClose -text "Close" \
		-command "wm withdraw $w" $Gui(WBA)}
		eval [subst $c]

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

	set c {label $f.lActive -text "Active Volume: " $Gui(WLA)\
		-bg $Gui(inactiveWorkspace)}; eval [subst $c]
	set c {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mbActive.m $Gui(WMA)}; eval [subst $c]
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
	set c {label $f.lAuto -text "Window/Level:" $Gui(WLA)}; eval [subst $c]
	frame $f.fAuto -bg $Gui(activeWorkspace)
	grid $f.lAuto $f.fAuto -pady $Gui(pad)  -padx $Gui(pad) -sticky e
	grid $f.fAuto -columnspan 2 -sticky w

	foreach value "1 0" text "Auto Manual" width "5 7" {
		set c {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
			-text "$text" -value "$value" -variable Volume(autoWindowLevel) \
			-command "MainVolumesSetParam AutoWindowLevel; MainVolumesRender" \
			$Gui(WCA)}; eval [subst $c]
		pack $f.fAuto.rAuto$value -side left -fill x
	}

	#-------------------------------------------
	# W/L Sliders
	#-------------------------------------------
	foreach slider "Window Level" {
		set c {label $f.l${slider} -text "${slider}:" $Gui(WLA)}
			eval [subst $c]
		set c {entry $f.e${slider} -width 7 \
			-textvariable Volume([Uncap ${slider}]) $Gui(WEA)}; eval [subst $c]
		bind $f.e${slider} <Return>   \
			"MainVolumesSetParam ${slider}; MainVolumesRender"
		bind $f.e${slider} <FocusOut> \
			"MainVolumesSetParam ${slider}; MainVolumesRender"
		set c {scale $f.s${slider} -from 1 -to 1024 \
			-variable Volume([Uncap ${slider}]) -length 200 -resolution 1 \
			-command "MainVolumesSetParam ${slider}; MainVolumesRenderActive"\
			 $Gui(WSA)}; eval [subst $c]
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
	set c {label $f.lAuto -text "Threshold:" $Gui(WLA)}
		eval [subst $c]
	frame $f.fAuto -bg $Gui(activeWorkspace)
	grid $f.lAuto $f.fAuto -pady $Gui(pad) -padx $Gui(pad) -sticky e
	grid $f.fAuto -columnspan 2 -sticky w

	foreach value "1 0 -1" text "Auto Manual None" width "5 7 5" {
		set c {radiobutton $f.fAuto.rAuto$value -width $width -indicatoron 0\
			-text "$text" -value "$value" -variable Volume(autoThreshold) \
			-command "MainVolumesSetParam AutoThreshold; MainVolumesRender" \
			$Gui(WCA)}; eval [subst $c]
		pack $f.fAuto.rAuto$value -side left -fill x
	}

	#-------------------------------------------
	# Threshold Sliders
	#-------------------------------------------
	foreach slider "Lower Upper" {
		set c {label $f.l${slider} -text "${slider}:" $Gui(WLA)}
			eval [subst $c]
		set c {entry $f.e${slider} -width 7 \
			-textvariable Volume([Uncap ${slider}]Threshold) $Gui(WEA)}; eval [subst $c]
			bind $f.e${slider} <Return>   \
				"MainVolumesSetParam ${slider}Threshold; MainVolumesRender"
			bind $f.e${slider} <FocusOut> \
				"MainVolumesSetParam ${slider}Threshold; MainVolumesRender"
		set c {scale $f.s${slider} -from 1 -to 1024 \
			-variable Volume([Uncap ${slider}]Threshold) -length 200 -resolution 1 \
			-command "MainVolumesSetParam ${slider}Threshold; MainVolumesRender"\
			 $Gui(WSA)}; eval [subst $c]
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
# .PROC MainVolumesRender
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
# .END
#-------------------------------------------------------------------------------
proc MainVolumesSetActive {{v ""}} {
	global Volume Lut

	# Optionally set activeID to v
	if {$v == ""} {
		set v $Volume(activeID)
	} else {
		set Volume(activeID) $v
	}

	# Slider range (obviously must be set before window/level)
	set Volume(rangeLow)    [Volume($v,vol) GetRangeLow]
	set Volume(rangeHigh)   [Volume($v,vol) GetRangeHigh]
	set Volume(rangeAuto)   [Volume($v,vol) GetRangeAuto]
	MainVolumesUpdateSliderRange

	# Update GUI
	foreach item "Window Level AutoWindowLevel UpperThreshold LowerThreshold \
		AutoThreshold Interpolate" {
		set Volume([Uncap $item]) [Volume($v,node) Get$item]
	}

	# Change button text
	foreach mb $Volume(mbActiveList) {
		$mb config -text [Volume($v,node) GetName]
	}
		
	if {[IsModule Volumes] == 1} {
		# LUT menu
		$Volume(mbLUT) config -text \
			$Lut([Volume($v,node) GetLUTName],name)

		if {$Volume(histogram) == "On"} {
			histMapper SetInput [Volume($v,vol) GetHistogramPlot]
			histWin Render
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainVolumesSetParam
# .END
#-------------------------------------------------------------------------------
proc MainVolumesSetParam {Param {value ""}} {
	global Volume Slice Lut InitProc

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
		AutoThreshold" $Param] != -1} {

		# If no change, return
		if {$value == [Volume($v,node) Get$Param]} {return}

		# Update value
		# When AutoThreshold is -1, that means NONE
		set none 0
		if {$Param == "AutoThreshold" && $value == "-1"} {
			# Turn off auto threshold
			set value 0
			set none 1
		}

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

		# If AutoThreshold, get the resulting upper/lower threshold
		if {$Param == "AutoThreshold"} {
			if {$none == 1} {
				Volume($v,node) SetLowerThreshold [Volume($v,vol) GetRangeLow]
				Volume($v,node) SetUpperThreshold [Volume($v,vol) GetRangeHigh]
			}
			Volume($v,vol) Update
			set Volume(lowerThreshold) [Volume($v,node) GetLowerThreshold]
			set Volume(upperThreshold) [Volume($v,node) GetUpperThreshold]
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

