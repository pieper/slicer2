#=auto==========================================================================
# Copyright (c) 1999 Surgical Planning Lab, Brigham and Women's Hospital
#  
# Direct all questions regarding this copyright to slicer@ai.mit.edu.
# The following terms apply to all files associated with the software unless
# explicitly disclaimed in individual files.   
# 
# The authors hereby grant permission to use, copy, and distribute this
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
# FILE:        MainSlices.tcl
# DATE:        12/09/1999 14:09
# LAST EDITOR: gering
# PROCEDURES:  
#   MainSlicesInit
#   MainSlicesBuildVTK
#   MainSlicesBuildControlsForVolume
#   MainSlicesBuildControls
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
#   MainSlicesSetOffsetCallback
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
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC MainSlicesInit
# .END
#-------------------------------------------------------------------------------
proc MainSlicesInit {} {
	global Slice Module

	lappend Module(procVTK)  MainSlicesBuildVTK
	lappend Module(procMRML) MainSlicesUpdateMRML

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

#
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
# Slice(clipPlanes)             : instance of vtkImplicitBoolean 
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
# .END
#-------------------------------------------------------------------------------
proc MainSlicesBuildVTK {} {
	global View Volume Slice Model Gui View

	# Clipping
	vtkImplicitBoolean Slice(clipPlanes)
		Slice(clipPlanes) SetOperationTypeToIntersection

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

		viewRen AddActor Slice($s,outlineActor)
		viewRen AddActor Slice($s,planeActor)

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
# .END
#-------------------------------------------------------------------------------
proc MainSlicesBuildControlsForVolume {f s layer text} {
	global Gui

	# All Slices
	set c {menubutton $f.mb${layer}Volume -text "${text}:" -width 3 \
		-menu $f.mb${layer}Volume.m $Gui(WMBA)}; eval [subst $c]
	set c {menu $f.mb${layer}Volume.m $Gui(WMA)}; eval [subst $c]
	bind $f.mb${layer}Volume <Button-3> "MainVolumesPopupGo $layer $s %X %Y"

	# This Slice
	set c {menubutton $f.mb${layer}Volume${s} -text None -width 13 \
		-menu $f.mb${layer}Volume${s}.m $Gui(WMBA) -bg $Gui(slice$s)}
		eval [subst $c]
	set c {menu $f.mb${layer}Volume${s}.m $Gui(WMA)}; eval [subst $c]
	bind $f.mb${layer}Volume$s <Button-3> "MainVolumesPopupGo $layer $s %X %Y"			
	
	pack $f.mb${layer}Volume $f.mb${layer}Volume${s} \
		-pady 0 -padx 2 -side left -fill x
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesBuildControls
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

	set c {entry $f.eOffset -width 4 \
		-textvariable Slice($s,offset) $Gui(WEA)}; eval [subst $c]
		bind $f.eOffset <Return>   "MainSlicesSetOffsetCallback $s"
		bind $f.eOffset <FocusOut> "MainSlicesSetOffsetCallback $s"
	set c {scale $f.sOffset -from -$fov2 -to $fov2 \
		-variable Slice($s,offset) -length 180 -resolution 1.0 -command \
		"MainSlicesSetOffsetCallback $s" $Gui(WSA) -troughcolor $Gui(slice$s) }
		eval [subst $c]

	pack $f.sOffset $f.eOffset -side left -anchor w -padx 2 -pady 0

	# Visibility
	#-------------------------------------------

	# This Slice
	set c {checkbutton $f.cVisibility${s} \
		-variable Slice($s,visibility) -indicatoron 0 -text "V" \
		-command "MainSlicesSetVisibility ${s}; MainViewerHideSliceControls; Render3D" \
		$Gui(WCA) -selectcolor $Gui(slice$s)}; eval [subst $c]
	pack $f.cVisibility${s} -side left -padx 2

	# Menu on the Visibility checkbutton
	set c {menu $f.cVisibility${s}.men $Gui(WMA)}; eval [subst $c]
	set men $f.cVisibility${s}.men
	$men add check -label "All Visible" \
		-variable Slice(visibilityAll) \
		-command "MainSlicesSetVisibilityAll 1; MainViewerHideSliceControls; Render3D"
	$men add check -label "All Invisible" \
		-variable Slice(visibilityAll) \
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
	set c {menubutton $f.mbOrient -text "Or:" -width 3 -menu $f.mbOrient.m \
		$Gui(WMBA) -anchor e}; eval [subst $c]
	pack $f.mbOrient -side left -pady 0 -padx 2 -fill x

	set c {menu $f.mbOrient.m $Gui(WMA)}; eval [subst $c];
	foreach item "AxiSagCor Orthogonal Slices" {
		$f.mbOrient.m add command -label $item -command \
			"MainSlicesSetOrientAll $item; MainViewerHideSliceControls; RenderAll"
	}

	# This slice
	set c {menubutton $f.mbOrient${s} -text INIT -menu $f.mbOrient${s}.m \
		-width 13 $Gui(WMBA) -bg $Gui(slice$s)}; eval [subst $c]
	pack $f.mbOrient${s} -side left -pady 0 -padx 2 -fill x

	set c {menu $f.mbOrient${s}.m $Gui(WMA)}; eval [subst $c];
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

	MainSlicesBuildControlsForVolume $f $s Fore  Fg
	MainSlicesBuildControlsForVolume $f $s Label Lb
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc MainSlicesUpdateMRML {} {
	global Gui Slice Volume Module

	foreach s $Slice(idList) {
		
		# Volumes on slice
		#----------------------------
		foreach layer "Back Fore Label" baseSuffix "Orient Volume Volume" {
	
			# All Slices
			set suffix "f${baseSuffix}.mb${layer}Volume.m"
			
			foreach pre "$Slice($s,controls)" {
				set m $pre.$suffix

				$m delete 0 end
				foreach v $Volume(idListForMenu) {
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
				foreach v $Volume(idListForMenu) {
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
proc MainSlicesSetClipState {s} {
	global Gui Slice

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

	eval Slice($s,clipPlane) SetOrigin  $origin	 
	eval Slice($s,clipPlane) SetNormal $normal
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetFov
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
# .PROC MainSlicesSetOffsetCallback
# .END
#-------------------------------------------------------------------------------
proc MainSlicesSetOffsetCallback {s {value ""}} {
	global Slice InitProc

	# Load GUI faster
	if {![info exists InitProc(MainSlicesSetOffsetCallback$s)]} {
		set InitProc(MainSlicesSetOffsetCallback$s) 1
		return
	}

	MainSlicesSetOffset $s $value

	RenderBoth $s
}

#-------------------------------------------------------------------------------
# .PROC MainSlicesSetOffset
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

