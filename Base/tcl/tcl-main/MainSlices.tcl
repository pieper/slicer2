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
# FILE:        MainSlices.tcl
# PROCEDURES:  
#   MainSlicesInit
#   MainSlicesBuildVTK
#   MainSlicesBuildControlsForVolume
#   MainSlicesBuildControls s F
#   MainSlicesUpdateMRML
#   MainSlicesVolumeParam
#   MainSlicesSetClipState
#   MainSlicesRefreshClip
#   MainSlicesSetFov
#   MainSlicesCenterCursor
#   MainSlicesKeyPress
#   MainSlicesSetActive
#   MainSlicesSetVolumeAll
#   MainSlicesSetVolume
#   MainSlicesSetOffsetInit
#   MainSlicesSetOffset
#   MainSlicesSetSliderRange
#   MainSlicesSetOrientAll
#   MainSlicesSetOrient
#   MainSlicesResetZoomAll
#   MainSlicesSetZoomAll
#   MainSlicesConfigGui
#   MainSlicesSetZoom
#   MainSlicesSetVisibilityAll
#   MainSlicesSetVisibility
#   MainSlicesSetOpacityAll
#   MainSlicesSetFadeAll
#   MainSlicesSave
#   MainSlicesSavePopup
#   MainSlicesWrite
#   MainSlicesStorePresets
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC MainSlicesInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesInit {} {
	global Slice Module

	# Define Procedures
	lappend Module(procVTK)  MainSlicesBuildVTK
	lappend Module(procMRML) MainSlicesUpdateMRML
	lappend Module(procStorePresets) MainSlicesStorePresets
	lappend Module(procRecallPresets) MainSlicesRecallPresets

	# Preset Defaults
	set Module(Slices,presets) "opacity='1.0' fade='0' \
0,visibility='0' 0,backVolID='0' 0,foreVolID='0' 0,labelVolID='0' \
0,orient='Axial' 0,offset='0' 0,zoom='1.0' 0,clipState='1'\
1,visibility='0' 1,backVolID='0' 1,foreVolID='0' 1,labelVolID='0' \
1,orient='Sagittal' 1,offset='0' 1,zoom='1.0' 1,clipState='1' \
2,visibility='0' 2,backVolID='0' 2,foreVolID='0' 2,labelVolID='0' \
2,orient='Coronal' 2,offset='0' 2,zoom='1.0' 2,clipState='1'"

        # Set version info
        lappend Module(versions) [ParseCVSInfo MainSlices \
		{$Revision: 1.26 $} {$Date: 2001/03/23 22:40:42 $}]

	# Initialize Variables
	set Slice(idList) "0 1 2"

	set Slice(opacity) 0.5
	set Slice(visibilityAll) 0
	set Slice(activeID) 0 
	set Slice(0,controls) ""
	set Slice(1,controls) ""
	set Slice(2,controls) ""
	set Slice(0,offset) 0
	set Slice(1,offset) 0
	set Slice(2,offset) 0
	set Slice(0,addedFunction) 0
	set Slice(1,addedFunction) 0
	set Slice(2,addedFunction) 0

	set Slice(xAnno) 0  
	set Slice(yAnno) 0 
	set Slice(xScrAnno) 0 
	set Slice(yScrAnno) 0

	foreach s $Slice(idList) {
		set Slice($s,id) 0
		set Slice($s,visibility) 0
		set Slice($s,clipState) 1
		set Slice($s,zoom) 1 
		set Slice($s,driver) User
		set Slice($s,orient) Axial
		set Slice($s,offsetAxia) 0
		set Slice($s,offsetSagittal) 0
		set Slice($s,offsetCoronal) 0
		set Slice($s,offsetInPlane) 0
		set Slice($s,offsetInPlane90) 0
		set Slice($s,offsetInPlaneNeg90) 0
		set Slice($s,offsetPerp) 0
		set Slice($s,offsetOrigSlice) Auto
		set Slice($s,offsetAxiSlice) Auto
		set Slice($s,offsetCorSlice) Auto
		set Slice($s,offsetSagSlice) Auto
		set Slice($s,backVolID) 0
		set Slice($s,foreVolID) 0
		set Slice($s,labelVolID) 0
	}
}

#-------------------------------------------------------------------------------
# Variables
#-------------------------------------------------------------------------------
#
# Slice(activeID)               :
# Slice(idList)                 : Usually 0 1 2: the 3 slice showing windows.
# Slice(num)                    : # of slices in idList.
# Slice(visibilityAll)          : 1 = AllSlices Visible in 3D. 0 = Not.
# Slice(xAnno)                  :  Not Sure, Used in SlicesEvents.tcl
# Slice(xScrAnno)               : 
# Slice(yAnno)                  : 
# Slice(yScrAnno)               : 
# Slice(clipPlanes)             : instance of vtkImplicitBoolean, used to
#                                 determine clip regions
# Slice(clipType)               : "Intersection" or "Union", deals with the
#                                 way to deal with clipping from many slices
#
#  id is a number in Slice(idList)
#                               
# Slice(id,addedFunction)       : 1 = Addedfunction for clipping. 0 = None.
# Slice(id,backVolID)           : id of the Volume in the background.
# Slice(id,clipState)           : 1 and 2 use clipping. 0 is no clipping.
# Slice(id,clipPlane)           : instance of vtkPlane, used for clipping.
# Slice(id,driver)              : Set to "User" in slicer.config. Never Used.
# Slice(id,foreVolID)           : id of the Volume in the foreground
# Slice(id,labelVolID)          : id of the Label Volume 
# Slice(id,offset)              : The value on the offset slider
# Slice(id,offsetAxiSlice)      : 
# Slice(id,offsetAxial)         : No Idea
# Slice(id,offsetCorSlice)      :
# Slice(id,offsetCoronal)       :
# Slice(id,offsetInPlane)       :
# Slice(id,offsetInPlane90)     :
# Slice(id,offsetInPlaneNeg90)  :
# Slice(id,offsetOrigSlice)     :
# Slice(id,offsetPerp)          :
# Slice(id,offsetSagSlice)      :
# Slice(id,offsetSagittal)      :
# Slice(id,orient)              : 
# Slice(id,visibility)          : Is the plane visible in the 3D viewer?
# Slice(id,zoom)                : 1,2 4 or 8 : The Zoom on the slice.
#


#-------------------------------------------------------------------------------
# .PROC MainSlicesBuildVTK
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesBuildVTK {} {
	global View Volume Slice Model Gui View

	# Clipping
	vtkImplicitBoolean Slice(clipPlanes)
		Slice(clipPlanes) SetOperationTypeToIntersection

        set Slice(clipType) "Intersection"

	foreach s $Slice(idList) {
		vtkPlane Slice($s,clipPlane)
		Slice(clipPlanes) AddFunction Slice($s,clipPlane)
		MainSlicesRefreshClip $s
	}

	foreach s $Slice(idList) {
		vtkTexture Slice($s,texture)
		Slice($s,texture) SetInput [Slicer GetOutput $s]

		vtkPlaneSource Slice($s,planeSource)

		vtkPolyDataMapper Slice($s,planeMapper)
		Slice($s,planeMapper) SetInput [Slice($s,planeSource) GetOutput]

		vtkActor Slice($s,planeActor)
		Slice($s,planeActor) SetScale      $View(fov) $View(fov) 1.0
		Slice($s,planeActor) SetTexture    Slice($s,texture)
		Slice($s,planeActor) SetMapper     Slice($s,planeMapper)
		Slice($s,planeActor) SetUserMatrix [Slicer GetReformatMatrix $s]
		Slice($s,planeActor) SetVisibility $Slice($s,visibility) 
		[Slice($s,planeActor) GetProperty] SetAmbient 1
		[Slice($s,planeActor) GetProperty] SetDiffuse 0
		[Slice($s,planeActor) GetProperty] SetSpecular 0

		vtkOutlineSource Slice($s,outlineSource)
		Slice($s,outlineSource) SetBounds -0.5 0.5 -0.5 0.5 0.0 0.0

		vtkPolyDataMapper Slice($s,outlineMapper)
		Slice($s,outlineMapper) SetInput [Slice($s,outlineSource) GetOutput]
		Slice($s,outlineMapper) ImmediateModeRenderingOn

		vtkActor Slice($s,outlineActor)
		Slice($s,outlineActor) SetMapper     Slice($s,outlineMapper)
		Slice($s,outlineActor) SetScale      $View(fov) $View(fov) 1.0
		Slice($s,outlineActor) SetUserMatrix [Slicer GetReformatMatrix $s]
		Slice($s,outlineActor) SetVisibility $Slice($s,visibility) 

		MainAddActor Slice($s,outlineActor)
		MainAddActor Slice($s,planeActor)

		# Clip
		MainSlicesSetClipState $s
	}

	# Color of slice outline
	[Slice(0,outlineActor) GetProperty] SetColor 0.9 0.1 0.1
	[Slice(1,outlineActor) GetProperty] SetColor 0.9 0.9 0.1
	[Slice(2,outlineActor) GetProperty] SetColor 0.1 0.9 0.1
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesBuildControlsForVolume
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesBuildControlsForVolume {f s layer text} {
	global Gui

	# All Slices
	eval {menubutton $f.mb${layer}Volume -text "${text}:" -width 3 \
		-menu $f.mb${layer}Volume.m} $Gui(WMBA)
	eval {menu $f.mb${layer}Volume.m} $Gui(WMA)
	bind $f.mb${layer}Volume <Button-3> "MainVolumesPopupGo $layer $s %X %Y"

	# This Slice
	eval {menubutton $f.mb${layer}Volume${s} -text None -width 13 \
		-menu $f.mb${layer}Volume${s}.m} $Gui(WMBA) {-bg $Gui(slice$s)}
	eval {menu $f.mb${layer}Volume${s}.m} $Gui(WMA)
	bind $f.mb${layer}Volume$s <Button-3> "MainVolumesPopupGo $layer $s %X %Y"			
	
	pack $f.mb${layer}Volume $f.mb${layer}Volume${s} \
		-pady 0 -padx 2 -side left -fill x
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesBuildControls
# 
# Called from MainViewer.tcl in MainViewerBuildGUI.
#
# .ARGS
#  int s the id of the Slice
#  str F the name of the Slice Window
# .END
#-------------------------------------------------------------------------------
proc MainSlicesBuildControls {s F} {
	global Gui View Slice

	lappend Slice($s,controls) $F

	frame $F.fOffset -bg $Gui(activeWorkspace)
	frame $F.fOrient -bg $Gui(activeWorkspace)
	frame $F.fVolume -bg $Gui(activeWorkspace)

	pack $F.fOffset $F.fOrient $F.fVolume \
		-fill x -side top -padx 0 -pady 3

	# Offset
	#-------------------------------------------
	set f $F.fOffset
	set fov2 [expr $View(fov) / 2]

	eval {entry $f.eOffset -width 4 -textvariable Slice($s,offset)} $Gui(WEA)
		bind $f.eOffset <Return>   "MainSlicesSetOffset $s; RenderBoth $s"
		bind $f.eOffset <FocusOut> "MainSlicesSetOffset $s; RenderBoth $s"
	eval {scale $f.sOffset -from -$fov2 -to $fov2 \
		-variable Slice($s,offset) -length 160 -resolution 1.0 -command \
		"MainSlicesSetOffsetInit $s $f.sOffset"} $Gui(WSA) \
		{-troughcolor $Gui(slice$s)}

	pack $f.sOffset $f.eOffset -side left -anchor w -padx 2 -pady 0

	# Visibility
	#-------------------------------------------

	# This Slice
	eval {checkbutton $f.cVisibility${s} \
		-variable Slice($s,visibility) -indicatoron 0 -text "V" -width 2 \
		-command "MainSlicesSetVisibility ${s}; \
		MainViewerHideSliceControls; Render3D"} $Gui(WCA) \
		{-selectcolor $Gui(slice$s)}
	pack $f.cVisibility${s} -side left -padx 2

	# Menu on the Visibility checkbutton
	eval {menu $f.cVisibility${s}.men} $Gui(WMA)
	set men $f.cVisibility${s}.men
	$men add command -label "All Visible" \
		-command "MainSlicesSetVisibilityAll 1; MainViewerHideSliceControls; Render3D"
	$men add command -label "All Invisible" \
		-command "MainSlicesSetVisibilityAll 0; MainViewerHideSliceControls; Render3D"
	$men add command -label "Reset zoom" -command \
		"MainSlicesResetZoomAll; MainViewerHideSliceControls; RenderSlices"
	$men add command -label "Zoom all x2" -command \
		"MainSlicesSetZoomAll 2; MainViewerHideSliceControls; RenderSlices"
	$men add command -label "Zoom all x3" -command \
		"MainSlicesSetZoomAll 3; MainViewerHideSliceControls; RenderSlices"
	$men add command -label "Auto Window/Level" -command \
		"MainSlicesVolumeParam $s AutoWindowLevel 1"
	$men add command -label "No Threshold" -command \
		"MainSlicesVolumeParam $s AutoThreshold -1"
	$men add command -label "-- Close Menu --" -command "$men unpost"
	bind $f.cVisibility${s} <Button-3> "$men post %X %Y"

	# Orientation
	#-------------------------------------------
	set f $F.fOrient

	# All Slices
	eval {menubutton $f.mbOrient -text "Or:" -width 3 -menu $f.mbOrient.m} \
		$Gui(WMBA) {-anchor e}
	pack $f.mbOrient -side left -pady 0 -padx 2 -fill x

	eval {menu $f.mbOrient.m} $Gui(WMA)
	foreach item "AxiSagCor Orthogonal Slices" {
		$f.mbOrient.m add command -label $item -command \
			"MainSlicesSetOrientAll $item; MainViewerHideSliceControls; RenderAll"
	}

	# This slice
	eval {menubutton $f.mbOrient${s} -text INIT -menu $f.mbOrient${s}.m \
		-width 13} $Gui(WMBA) {-bg $Gui(slice$s)}
	pack $f.mbOrient${s} -side left -pady 0 -padx 2 -fill x

	eval {menu $f.mbOrient${s}.m} $Gui(WMA)
	foreach item "[Slicer GetOrientList]" {
		$f.mbOrient${s}.m add command -label $item -command \
			"MainSlicesSetOrient ${s} $item; MainViewerHideSliceControls; RenderBoth $s"
	}

	# Background Volume
	#-------------------------------------------
	MainSlicesBuildControlsForVolume $f $s Back Bg

	# Foreground/Label Volumes row
	#-------------------------------------------
	set f $F.fVolume

	MainSlicesBuildControlsForVolume $f $s Label Lb
	MainSlicesBuildControlsForVolume $f $s Fore  Fg
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesUpdateMRML
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesUpdateMRML {} {
	global Gui Slice Volume Module

	# See if the volume for each layer actually exists.
	# If not, use the None volume
	#
	set n $Volume(idNone)
	foreach s $Slice(idList) {
 		foreach layer "back fore label" {
			if {[lsearch $Volume(idList) $Slice($s,${layer}VolID)] == -1} {
				MainSlicesSetVolume [Cap $layer] $s $n
			}
		}
	}

	foreach s $Slice(idList) {
		
		# Volumes on slice
		#----------------------------
		foreach layer "Back Fore Label" baseSuffix "Orient Volume Volume" {
	
			# All Slices
			set suffix "f${baseSuffix}.mb${layer}Volume.m"
			
			foreach pre "$Slice($s,controls)" {
				set m $pre.$suffix

				$m delete 0 end
				foreach v $Volume(idList) {
					$m add command -label [Volume($v,node) GetName] \
						-command "MainSlicesSetVolumeAll $layer $v; \
						MainViewerHideSliceControls; RenderAll"
				}
			}

			# Current Slice
			set suffix "f${baseSuffix}.mb${layer}Volume${s}.m"

			foreach pre "$Slice($s,controls)" {
				set m $pre.$suffix

				$m delete 0 end
				foreach v $Volume(idList) {
					$m add command -label [Volume($v,node) GetName] \
						-command "MainSlicesSetVolume ${layer} ${s} $v; \
						MainViewerHideSliceControls; RenderBoth $s"
				}
			}
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesVolumeParam
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesVolumeParam {s param value} {

	set v [[[Slicer GetBackVolume $s] GetMrmlNode] GetID]
	MainVolumesSetActive $v
	MainVolumesSetParam $param $value
	RenderAll
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetClipState
#
#  Assumes Slice(id,clipState) is set
#  Makes appropriate changes to Slice(clipPlanes) 
#
# Usage: MainSlicesSetClipState id
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetClipState {s {state ""}} {
	global Gui Slice

	if {$state != ""} {
		set Slice($s,clipState) $state
	}
	set state $Slice($s,clipState)

	if {$state == "1"} {
		MainSlicesRefreshClip $s
		if {$Slice($s,addedFunction) == 0} {
			Slice(clipPlanes) AddFunction Slice($s,clipPlane)
			set Slice($s,addedFunction) 1
		}
	} elseif {$state == "2"} {
		MainSlicesRefreshClip $s
		if {$Slice($s,addedFunction) == 0} {
			Slice(clipPlanes) AddFunction Slice($s,clipPlane)
			set Slice($s,addedFunction) 1
		}
	} else {
		Slice(clipPlanes) RemoveFunction Slice($s,clipPlane)
		set Slice($s,addedFunction) 0
	}
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesRefreshClip
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesRefreshClip {s} {
	global Slice
	
	# Set normal and orient of slice
	if {$Slice($s,clipState) == "1"} {
		set sign 1
	} elseif {$Slice($s,clipState) == "2"} {
		set sign -1
	} else {
		return
	}
	set mat [Slicer GetReformatMatrix $s]

	set origin "[$mat GetElement 0 3] \
		[$mat GetElement 1 3] [$mat GetElement 2 3]"

	set normal "[expr $sign*[$mat GetElement 0 2]] \
		[expr $sign*[$mat GetElement 1 2]] \
		[expr $sign*[$mat GetElement 2 2]]"

	# WARNING: objects may not exist yet!
	if {[info command Slice($s,clipPlane)] != ""} {
		eval Slice($s,clipPlane) SetOrigin  $origin	 
		eval Slice($s,clipPlane) SetNormal $normal
	}
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetFov
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetFov {} {
	global Slice View

	foreach s $Slice(idList) {
		MainSlicesSetSliderRange $s
		Slice($s,planeActor)   SetScale $View(fov) $View(fov) 1.0
		Slice($s,outlineActor) SetScale $View(fov) $View(fov) 1.0
	}
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesCenterCursor
#
# 
# Called when the mouse exits a window
# Usage: CenterCursor sliceid
# .END
#-------------------------------------------------------------------------------
proc MainSlicesCenterCursor {s} {
	global View

	if {$View(mode) == "Quad512"} {
		Slicer SetCursorPosition $s 256 256
	} else {
		Slicer SetCursorPosition $s 128 128
	}
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesKeyPress
#
# Called when a key is pressed in the 2D window.
# Deals with Up, Down, Left and Right.
#
#  Up and Down moves the slice offset.
#  Left and Right calles EditApplyFilter from Edit.tcl
# .END
#-------------------------------------------------------------------------------
proc MainSlicesKeyPress {key} {
	global View Slice Toolbar Edit

	# Determine which slice this is
	set win $View(inWin)
	if {$win == "none"} {return}
	set s [string index $win 2]
	switch $key {
	  "Up" {
	    MainSlicesSetOffset $s Next; SliceMouseAnno;
		MainSlicesRefreshClip $s
		# I could Render3D here, but I'd prefer speed.
	  }
	  "Down" {
	    MainSlicesSetOffset $s Prev; SliceMouseAnno;
		MainSlicesRefreshClip $s
	  }
	  "Left" {
		if {[IsModule Edit] == 1} {
			if {$Toolbar(mode) == "Edit" && $Edit(op) == "Draw"} {
				EditApplyFilter Draw
			}
		}
	  }
	  "Right" {
		if {[IsModule Edit] == 1} {
			if {$Toolbar(mode) == "Edit" && $Edit(op) == "Draw"} {
				EditApplyFilter Draw
			}
		}
	  }
	}
}

 
#-------------------------------------------------------------------------------
# .PROC MainSlicesSetActive
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetActive {{s ""}} {
	global Edit Slice View Gui

	if {$s == $Slice(activeID)} {return}
	
	if {$s == ""} {
		set s $Slice(activeID)
	} else {
		set Slice(activeID) $s
	}

	Slicer SetActiveSlice $s
	Slicer Update

	# Redraw mag with polygon drawing
	RenderSlices
	MainViewSetWelcome sl$s
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetVolumeAll
#
#
# Sets all the layers to be the same volume.
# Layer is Fore,Back,Label
#
# Usage: MainSlicesSetVolumeAll Layer id
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetVolumeAll {Layer v} {
	global Slice Volume

	# Check if volume exists and use the None if not
	if {[lsearch $Volume(idList) $v] == -1} {
		set v $Volume(idNone)
	}
	
	# Fields in the Slice array are uncapitalized
	set layer [Uncap $Layer]

	# Set the volume in the Slicer
	Slicer Set${Layer}Volume Volume($v,vol)
	Slicer Update

	foreach s $Slice(idList) {
		set Slice($s,${layer}VolID) $v

		# Change button text
		if {$Layer == "Back"} {
			MainSlicesConfigGui $s fOrient.mb${Layer}Volume$s \
				"-text [Volume($v,node) GetName]"
		} else {
			MainSlicesConfigGui $s fVolume.mb${Layer}Volume$s \
				"-text [Volume($v,node) GetName]"
		}

		# Always update Slider Range when change volume or orient
		MainSlicesSetSliderRange $s
	}
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetVolume
# Layer can be Back, Fore,Label
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetVolume {Layer s v} {
	global Slice Volume Model Lut

	# Check if volume exists and use the None if not
	if {[lsearch $Volume(idList) $v] == -1} {
		set v $Volume(idNone)
	}
	
	# Fields in the Slice array are uncapitalized
	set layer [Uncap $Layer]
	
	# If no change, return
	if {$v == $Slice($s,${layer}VolID)} {return}
	set Slice($s,${layer}VolID) $v

	# Change button text
	if {$Layer == "Back"} {
		MainSlicesConfigGui $s fOrient.mb${Layer}Volume$s \
			"-text [Volume($v,node) GetName]"
	} else {
		MainSlicesConfigGui $s fVolume.mb${Layer}Volume$s \
			"-text [Volume($v,node) GetName]"
	}

	# Set the volume in the Slicer
	Slicer Set${Layer}Volume $s Volume($v,vol)
	Slicer Update

	# Always update Slider Range when change volume or orient
	MainSlicesSetSliderRange $s
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetOffsetInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetOffsetInit {s widget {value ""}} {

	# This prevents Tk from calling RenderBoth when it first creates
	# the slider, but before a user uses it.

	$widget config -command "MainSlicesSetOffset $s; RenderBoth $s"
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetOffset
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetOffset {s {value ""}} {
	global Slice

	if {$value == ""} {
		set value $Slice($s,offset)
	} elseif {$value == "Prev"} {
		set value [expr $Slice($s,offset) - 1]
		set Slice($s,offset) $value
	} elseif {$value == "Next"} {
		set value [expr $Slice($s,offset) + 1]
		set Slice($s,offset) $value
	}
	set Slice($s,offset) $value
	
	Slicer SetOffset $s $value

	MainSlicesRefreshClip $s
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetSliderRange
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetSliderRange {s} {
	global Slice 

	set lo [Slicer GetOffsetRangeLow  $s]
	set hi [Slicer GetOffsetRangeHigh $s]

	MainSlicesConfigGui $s fOffset.sOffset "-from $lo -to $hi"

	# Update Offset 
	set Slice($s,offset) [Slicer GetOffset $s]
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetOrientAll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetOrientAll {orient} {
	global Slice View

	Slicer ComputeNTPFromCamera $View(viewCam)

	Slicer SetOrientString $orient

	foreach s $Slice(idList) {
		set orient [Slicer GetOrientString $s]
		set Slice($s,orient) $orient

		# Always update Slider Range when change Back volume or orient
		MainSlicesSetSliderRange $s

		# Set slider to the last used offset for this orient
		set Slice($s,offset) [Slicer GetOffset $s]

		# Change text on menu button
		MainSlicesConfigGui $s fOrient.mbOrient$s "-text $orient"
		$Slice($s,lOrient) config -text $orient

		# Anno
		switch $Slice($s,orient) {
			"Axial" {
				Anno($s,top,mapper)   SetInput A
				Anno($s,bot,mapper)   SetInput P
				Anno($s,left,mapper)  SetInput R
				Anno($s,right,mapper) SetInput L
			}
			"AxiSlice" {
				Anno($s,top,mapper)   SetInput A
				Anno($s,bot,mapper)   SetInput P
				Anno($s,left,mapper)  SetInput R
				Anno($s,right,mapper) SetInput L
			}
			"Sagittal" {
				Anno($s,top,mapper)   SetInput S
				Anno($s,bot,mapper)   SetInput I
				Anno($s,left,mapper)  SetInput A 
				Anno($s,right,mapper) SetInput P 
			}
			"SagSlice" {
				Anno($s,top,mapper)   SetInput S
				Anno($s,bot,mapper)   SetInput I
				Anno($s,left,mapper)  SetInput A 
				Anno($s,right,mapper) SetInput P 
			}
			"Coronal" {
				Anno($s,top,mapper)   SetInput S
				Anno($s,bot,mapper)   SetInput I
				Anno($s,left,mapper)  SetInput R
				Anno($s,right,mapper) SetInput L
			}
			"CorSlice" {
				Anno($s,top,mapper)   SetInput S
				Anno($s,bot,mapper)   SetInput I
				Anno($s,left,mapper)  SetInput R
				Anno($s,right,mapper) SetInput L
			}
			default {
				Anno($s,top,mapper)   SetInput " " 
				Anno($s,bot,mapper)   SetInput " " 
				Anno($s,left,mapper)  SetInput " " 
				Anno($s,right,mapper) SetInput " " 
			}
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetOrient
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetOrient {s orient} {
	global Slice Volume View

	Slicer ComputeNTPFromCamera $View(viewCam)

	Slicer SetOrientString $s $orient
	set Slice($s,orient) [Slicer GetOrientString $s]

	# Always update Slider Range when change Back volume or orient
	MainSlicesSetSliderRange $s

	# Set slider to the last used offset for this orient
	set Slice($s,offset) [Slicer GetOffset $s]

	# Change text on menu button
	MainSlicesConfigGui $s fOrient.mbOrient$s "-text $orient"
	$Slice($s,lOrient) config -text $orient

	# Anno
	switch $orient {
		"Axial" {
			Anno($s,top,mapper)   SetInput A
			Anno($s,bot,mapper)   SetInput P
			Anno($s,left,mapper)  SetInput R
			Anno($s,right,mapper) SetInput L
		}
		"AxiSlice" {
			Anno($s,top,mapper)   SetInput A
			Anno($s,bot,mapper)   SetInput P
			Anno($s,left,mapper)  SetInput R
			Anno($s,right,mapper) SetInput L
		}
		"Sagittal" {
			Anno($s,top,mapper)   SetInput S
			Anno($s,bot,mapper)   SetInput I
			Anno($s,left,mapper)  SetInput A 
			Anno($s,right,mapper) SetInput P 
		}
		"SagSlice" {
			Anno($s,top,mapper)   SetInput S
			Anno($s,bot,mapper)   SetInput I
			Anno($s,left,mapper)  SetInput A 
			Anno($s,right,mapper) SetInput P 
		}
		"Coronal" {
			Anno($s,top,mapper)   SetInput S
			Anno($s,bot,mapper)   SetInput I
			Anno($s,left,mapper)  SetInput R
			Anno($s,right,mapper) SetInput L
		}
		"CorSlice" {
			Anno($s,top,mapper)   SetInput S
			Anno($s,bot,mapper)   SetInput I
			Anno($s,left,mapper)  SetInput R
			Anno($s,right,mapper) SetInput L
		}
		default {
			Anno($s,top,mapper)   SetInput " " 
			Anno($s,bot,mapper)   SetInput " " 
			Anno($s,left,mapper)  SetInput " " 
			Anno($s,right,mapper) SetInput " " 
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesResetZoomAll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesResetZoomAll {} {
	global Slice

	foreach s $Slice(idList) {
		MainSlicesSetZoom $s 1
		Slicer SetZoomAutoCenter $s 1
	}
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetZoomAll
#
# Sets the zoom on all slices
# and displays the result
#
# Usage: MainSlicesSetZoomAll zoom
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetZoomAll {zoom} {
	global Slice

	# Change Slice's Zoom variable
	foreach s $Slice(idList) {
		set Slice($s,zoom) $zoom
	}
	Slicer SetZoom $zoom
	Slicer Update
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesConfigGui
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesConfigGui {s gui config} {
	global Gui Module Slice

	foreach f $Slice($s,controls) {
		eval $f.$gui config $config
	}
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetZoom
#
# Sets the zoom on a Slice id
# and displays the result
#
# Usage: MainSlicesSetZoom id zoom
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetZoom {s zoom} {
	global Slice

	# Change Slice's Zoom variable
	set Slice($s,zoom) $zoom
	
	Slicer SetZoom $s $zoom
	Slicer Update
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetVisibilityAll
#
# Set the visibility of all the slices on the 3D viewer
# to be visibilityAll. Called when the cVisibility button is clicked on."
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetVisibilityAll {{value ""}} {
	global Slice Anno

	if {$value != ""} {
		set Slice(visibilityAll) $value
	}

	foreach s $Slice(idList) {
		set Slice($s,visibility) $Slice(visibilityAll)

		Slice($s,planeActor) SetVisibility $Slice($s,visibility) 

		if {$Anno(outline) == 1} {
			Slice($s,outlineActor) SetVisibility $Slice($s,visibility)
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetVisibility
#
# Set the visibility of a slice on the 3D viewer
# to be it's visibility.  
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetVisibility {s} {
	global Slice Anno

	Slice($s,planeActor)   SetVisibility $Slice($s,visibility) 

	if {$Anno(outline) == 1} {
		Slice($s,outlineActor) SetVisibility $Slice($s,visibility)
	} 
	
	# If any slice is invisible, then Slice(visibilityAll) should be 0
	set Slice(visibilityAll) 1
	foreach s $Slice(idList) {
		if {$Slice($s,visibility) == 0} {
			set Slice(visibilityAll) 0
		}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetOpacityAll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetOpacityAll {{value ""}} {
	global Slice
	
	if {$value == ""} {
		set value $Slice(opacity)
	} else {
		set Slice(opacity) $value
	}
	Slicer SetForeOpacity $value
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetFadeAll
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetFadeAll {{value ""}} {
	global Slice
	
	if {$value == ""} {
		set value $Slice(fade)
	} else {
		set Slice(fade) $value
	}
	Slicer SetForeFade $value
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSave
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSave {} {
	global Mrml Slice

    # Prefix cannot be blank
	if {$Slice(prefix) == ""} {
		tk_messageBox -message "Please specify a file name."
		return
	}

	# Get a unique filename by appending a number to the prefix
	set filename [MainFileFindUniqueName $Mrml(dir) $Slice(prefix) $Slice(ext)]

	MainSlicesWrite $filename
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSavePopup
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSavePopup {} {
	global Slice Mrml Gui

	# Cannot have blank prefix
	if {$Slice(prefix) == ""} {
		set Slice(prefix) view
	}

 	# Show popup initialized to the last file saved
	set filename [file join $Mrml(dir) $Slice(prefix)]
	set dir [file dirname $filename]
	set typelist {
		{"TIFF File" {".tif"}}
		{"PPM File" {".ppm"}}
		{"BMP File" {".bmp"}}
		{"All Files" {*}}
	}
	set filename [tk_getSaveFile -title "Save Slice" -defaultextension $Slice(ext)\
		-filetypes $typelist -initialdir "$dir" -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return}

	MainSlicesWrite $filename
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesWrite
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesWrite {filename} {
	global viewWin Mrml Slice Gui

	MainFileCreateDirectory $filename
	
	# Write it
	set s $Slice(activeID)
	set ext [file extension $filename]
    set success 0
	switch $ext {
	".tif" {
	    set success 1
		vtkWindowToImageFilter filter
		filter SetInput sl${s}Win

		vtkTIFFWriter writer
		writer SetInput [filter GetOutput]
		writer SetFileName $filename
		writer Write
		filter Delete
		writer Delete
	}
	".bmp" {
	    set success 1
		vtkWindowToImageFilter filter
		filter SetInput sl${s}Win

		vtkBMPWriter writer
		writer SetInput [filter GetOutput]
		writer SetFileName $filename
		writer Write
		filter Delete
		writer Delete
	}
	".ppm" {
	    set success 1
		vtkWindowToImageFilter filter
		filter SetInput sl${s}Win

		vtkPNMWriter writer
		writer SetInput [filter GetOutput]
		writer SetFileName $filename
		writer Write
		filter Delete
		writer Delete
	}
	}
	if {$success == "0"} {
	    puts "Unable to save view.  Did you choose a filename extension?"
	    return
	}
	puts "Saved view: $filename"

	# Store the new prefix and extension for next time
	set root $Mrml(dir)
	set absPrefix [file rootname $filename]
	if {$Gui(pc) == 1} {
		set absPrefix [string tolower $absPrefix]
		set root [string tolower $Mrml(dir)]
	}
	if {[regexp "^$root/(\[^0-9\]*)(\[0-9\]*)" $absPrefix match relPrefix num] == 1} {
		set Slice(prefix) $relPrefix
	} else {
		set Slice(prefix) [file rootname $absPrefix]
	}
	set Slice(ext) [file extension $filename]
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesStorePresets
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc MainSlicesStorePresets {p} {
	global Preset Slice

	foreach s $Slice(idList) {
		set Preset(Slices,$p,$s,visibility) $Slice($s,visibility)
		set Preset(Slices,$p,$s,orient)     $Slice($s,orient)
		set Preset(Slices,$p,$s,offset)     $Slice($s,offset)
		set Preset(Slices,$p,$s,zoom)       $Slice($s,zoom)
		set Preset(Slices,$p,$s,clipState)  $Slice($s,clipState)
		set Preset(Slices,$p,$s,backVolID)  $Slice($s,backVolID)
		set Preset(Slices,$p,$s,foreVolID)  $Slice($s,foreVolID)
		set Preset(Slices,$p,$s,labelVolID) $Slice($s,labelVolID)
	}
	set Preset(Slices,$p,opacity) $Slice(opacity)
	set Preset(Slices,$p,fade) $Slice(fade)
}
	    
proc MainSlicesRecallPresets {p} {
	global Preset Slice

	foreach s $Slice(idList) {
		set Slice($s,visibility) $Preset(Slices,$p,$s,visibility)
		MainSlicesSetVisibility $s
		MainSlicesSetVolume Back $s $Preset(Slices,$p,$s,backVolID)
		MainSlicesSetVolume Fore $s $Preset(Slices,$p,$s,foreVolID)
		MainSlicesSetVolume Label $s $Preset(Slices,$p,$s,labelVolID)
		MainSlicesSetOrient $s $Preset(Slices,$p,$s,orient)
		MainSlicesSetOffset	$s $Preset(Slices,$p,$s,offset)
		MainSlicesSetZoom $s $Preset(Slices,$p,$s,zoom)
		MainSlicesSetClipState $s $Preset(Slices,$p,$s,clipState)
	}
	MainSlicesSetOpacityAll $Preset(Slices,$p,opacity)
	MainSlicesSetFadeAll $Preset(Slices,$p,fade)
}
