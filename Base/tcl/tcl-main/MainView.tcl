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
# FILE:        MainView.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   MainViewInit
#   MainViewBuildVTK
#   MainViewBuildGUI
#   MainViewSetFov
#   MainViewLightFollowCamera
#   MainMainViewPresetCallback
#   MainViewPreset
#   MainViewNavReset
#   MainViewRotate
#   MainViewNavRotate
#   MainViewSetStereo
#   MainViewSpin
#   MainViewSetWelcome
#   MainViewResetFocalPoint
#   MainViewSetFocalPoint
#   SaveScreen
#   SaveImage3D
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MainViewInit
# .END
#-------------------------------------------------------------------------------
proc MainViewInit {} {
	global Module View Gui

	# The MainViewBuildGUI proc is called specifically
	lappend Module(procVTK)  MainViewBuildVTK

	set View(createMagWin) Yes
	set View(createDirWin) Yes
	if {$Gui(pc) == 1} {
		set View(createMagWin) No
		set View(createDirWin) No
	}

	# Configurable
	set View(mode) Normal
	set View(viewerWidth)  700 
	set View(viewerHeight) 700 
	set View(viewerHeightNormal) 400
	set View(toolbarPosition) Top
	set View(closeupVisibility) Off
	set View(bgColorR) .7
	set View(bgColorG) .7
	set View(bgColorB) .9 
	set View(fov) 220.0
	set View(spin) 0
	set View(spinDir) Right
	set View(spinMs) 5 
	set View(spinDegrees) 2 
	set View(stereo) 0
	set View(stereoType) RedBlue

	# Init
	set View(rotateDegrees) 15
	set View(baselineClippingRange) "1 2001"
	set View(viewUp)         "0 0 1"
	set View(position)       "0 750 0"
	set View(focalPoint)     "0 0 0"
	set View(clippingRange)  "1 2001"
	set View(viewUp1)        "-0.5 0.866 0"
	set View(position1)      "624 360 0"
	set View(focalPoint1)    "0 0 0"
	set View(clippingRange1) "1 2001"
	set View(viewUp2)        "0.5 0.866 0"
	set View(position2)      "-624 360 0"
	set View(focalPoint2)    "0 0 0"
	set View(clippingRange2) "1 2001"
	set View(viewUp3)        "-0.224 -0.129 0.959"
	set View(position3)      "602 348 186"
	set View(focalPoint3)    "0 0 0"
	set View(clippingRange3) "1 2001"

	set View(magWin) none
	set View(inWin) none
	set View(image3D) $Gui(prefixSaveImage3D)
}

#-------------------------------------------------------------------------------
# .PROC MainViewBuildVTK
# .END
#-------------------------------------------------------------------------------
proc MainViewBuildVTK {} {
	global View Slice

	# Set background color
	eval viewRen SetBackground \
		$View(bgColorR) $View(bgColorG) $View(bgColorB)

	# Closeup magnification of the slice with the cursor over it
	#--------------------------------------------
	
	#  [ActiveOutput] -----> mag -> magCursor -\
	#                                           >-> magMapper
	#                           welcomeReader -/

	# Create closeup magnification
	vtkImageCloseUp2D View(mag)
	View(mag) SetInput [Slicer GetOutput 0]
	View(mag) SetRadius 12
	View(mag) SetMagnification 7
	set View(closeupMag) 7

	# Closeup Cursor
	vtkImageCrossHair2D View(magCursor)
	View(magCursor) SetInput [View(mag) GetOutput]
	View(magCursor) SetCursor 87 87
	View(magCursor) BullsEyeOn
	View(magCursor) SetBullsEyeWidth 7
	View(magCursor) ShowCursorOn 
	View(magCursor) IntersectCrossOff
	View(magCursor) SetCursorColor 1 1 .5 
	View(magCursor) SetNumHashes 0 
	View(magCursor) SetHashLength 6
	View(magCursor) SetHashGap 10 
	View(magCursor) SetMagnification 1 
}

#-------------------------------------------------------------------------------
# .PROC MainViewBuildGUI
# .END
#-------------------------------------------------------------------------------
proc MainViewBuildGUI {} {
	global Gui View Slice viewWin

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	#	Nav
	#		Top
	#			Dir
	#				Image
	#				Buttons
	#			Move
	#				Rotate
	#				Buttons
	#		Bot
	#-------------------------------------------

	#-------------------------------------------
	# View->Nav Frame
	#-------------------------------------------
	set f $Gui(fNav)
	
	frame $f.fTop -bg $Gui(activeWorkspace)
	frame $f.fBot -bg $Gui(activeWorkspace)
	pack $f.fTop -side top -padx 0 -pady 0 
	pack $f.fBot -side top -padx 0 -pady 0 -fill x

	#-------------------------------------------
	# View->Nav->Top Frame
	#-------------------------------------------
	set f $Gui(fNav).fTop
	
	frame $f.fDir     -bg $Gui(activeWorkspace)
	frame $f.fPreset  -bg $Gui(activeWorkspace)
	frame $f.fMove    -bg $Gui(activeWorkspace)
	
	pack $f.fDir $f.fPreset $f.fMove -side left -padx 3 -pady 0

	#-------------------------------------------
	# View->Nav->Top->Dir Frame
	#-------------------------------------------
	set f $Gui(fNav).fTop.fDir
	
	frame $f.fImage   -bg $Gui(activeWorkspace) \
		-bd $Gui(borderWidth) -relief sunken	
	
	pack $f.fImage -side left -padx 0 -pady 0
	
	#-------------------------------------------
	# View->Nav->Top->Dir->Image Frame
	#-------------------------------------------
	set f $Gui(fNav).fTop.fDir.fImage
	set Gui(fDir) $f
	
	# Create control for setting view Direction
	vtkPNMReader dirReader
		dirReader SetDataExtent 0 73 0 73 1 7
		dirReader SetFilePrefix [ExpandPath [file join gui view.ppm]]

	if {$View(createDirWin) == "Yes"} {
		
		MakeVTKImageWindow dir dirReader

		vtkTkImageWindowWidget $f.fDir -iw dirWin -width 74 -height 74 
		bind $f.fDir <Expose> {ExposeTkImageViewer %W %x %y %w %h}
		bind $f.fDir <1>      {if {[MainViewNavReset %x %y click] == 1} {Render3D}}
		bind $f.fDir <Enter>  {MainViewNavReset %x %y      }
		bind $f.fDir <Leave>  {MainViewNavReset %x %y leave}
		bind $f.fDir <Motion> {MainViewNavReset %x %y      }

		pack $f.fDir
		dirWin DoubleBufferOn
	}

	#-------------------------------------------
	# View->Nav->Top->Preset Frame
	#-------------------------------------------
	set f $Gui(fNav).fTop.fPreset
	set View(fPreset) $Gui(fNav).fTop.fPreset
	
	# Button
    foreach btn "1 2 3" {
		set c "button $f.c$btn -text $btn -width 2 $Gui(WBA)"
			eval [subst $c]
		bind $f.c$btn <ButtonPress>   "MainViewPreset $btn Press"
		bind $f.c$btn <ButtonRelease> "MainViewPreset $btn Release"
		pack $f.c$btn -side top -pady 2 
	}

	#-------------------------------------------
	# View->Nav->Top->Move Frame
	#-------------------------------------------
	set f $Gui(fNav).fTop.fMove
	
	frame $f.fRotate  -bg $Gui(activeWorkspace)
	
	# MainViewSpin button
    set c {checkbutton $f.cMainViewSpin \
        -text "Spin" -variable View(spin) -width 6 \
        -indicatoron 0 -command "MainViewSpin" $Gui(WCA)}
        eval [subst $c]

	pack $f.fRotate -side top -pady 2 
	pack $f.fRotate $f.cMainViewSpin -side top -pady 3 
	
	#-------------------------------------------
	# View->Nav->Top->Move->Rotate Frame
	#-------------------------------------------
	set f $Gui(fNav).fTop.fMove.fRotate
	
	# Create control for Rotating the view
	foreach pict "None Left Right Down Up" {
		image create photo iRotate${pict} \
		-file [ExpandPath [file join gui "rotate$pict.gif"]]			
	}
	label $f.lRotate -image iRotateNone -relief sunken -bd $Gui(borderWidth)
	bind $f.lRotate <1>      {MainViewNavRotate %W %x %y click}
	bind $f.lRotate <Enter>  {MainViewNavRotate %W %x %y      }
	bind $f.lRotate <Leave>  {MainViewNavRotate %W %x %y leave}
	bind $f.lRotate <Motion> {MainViewNavRotate %W %x %y      }

	pack $f.lRotate -side top -padx 0 -pady 0 
	
	#-------------------------------------------
	# View->Nav->Bot Frame
	#-------------------------------------------
	set f $Gui(fNav).fBot

	frame $f.fBtns2 -bg $Gui(activeWorkspace)
	frame $f.fSave -bg $Gui(activeWorkspace)
	frame $f.fMode -bg $Gui(activeWorkspace)

	pack $f.fBtns2 -side top -pady 2 
	pack $f.fSave -side top -pady 2 -fill x
	pack $f.fMode -side top -pady 2 

	#-------------------------------------------
	# View->Nav->Bot Frame
	#-------------------------------------------
	set f $Gui(fNav).fBot.fBtns2

    set c {label $f.lFov -text "FOV:" $Gui(WLA)}
		eval [subst $c]
	set c {entry $f.eFov -textvariable View(fov) -width 7 $Gui(WEA)}
		eval [subst $c]
		bind $f.eFov <Return> {MainViewSetFov; RenderAll}
	pack $f.lFov $f.eFov -side left -padx 5 -pady 0

	# Focalpoint button
    set c {button $f.bFocus \
        -text "Center" -width 7 \
        -command "MainViewResetFocalPoint; RenderAll" $Gui(WBA)}
        eval [subst $c]
 
	pack $f.bFocus -side top -padx 5 -pady 0

	#-------------------------------------------
	# Nav->Save
	#-------------------------------------------
	set f $Gui(fNav).fBot.fSave

	set c {button $f.bSave -text "Save 3D" -width 7 \
		-command "SaveScreen" $Gui(WBA)}; eval [subst $c]
	set c {entry $f.eSave -textvariable View(image3D) $Gui(WEA)}
		eval [subst $c]
	bind $f.eSave <Return> {SaveImage3D}
	pack $f.bSave -side left -padx 3
	pack $f.eSave -side left -padx 2 -expand 1 -fill x
	
	#-------------------------------------------
	# Nav->Save
	#-------------------------------------------
	set f $Gui(fNav).fBot.fMode

	foreach text "Normal 4x256 4x512 3D" value "Normal Quad256 Quad512 3D" {
		set c {radiobutton $f.rMode$value -text "$text" -value "$value" \
			-variable View(mode) -indicatoron 0 -command \
			"MainViewerSetMode" $Gui(WCA)}; eval [subst $c]
		pack $f.rMode$value -side left
	}

}

#-------------------------------------------------------------------------------
# .PROC MainViewSetFov
# .END
#-------------------------------------------------------------------------------
proc MainViewSetFov {} {
	global View Gui Slice

	Slicer SetFieldOfView $View(fov)
	MainViewNavReset 55 42 click
	MainViewNavReset 0 0 leave

	# Update slice offset, registration annotation
	MainAnnoSetFov
	MainSlicesSetFov
}

#-------------------------------------------------------------------------------
# .PROC MainViewLightFollowCamera
# .END
#-------------------------------------------------------------------------------
proc MainViewLightFollowCamera {} {
	global View
	
	# 3D Viewer
	set lights [viewRen GetLights]
	$lights InitTraversal
	set currentLight [$lights GetNextItem]
	if {$currentLight != ""} {
		eval $currentLight SetPosition   [$View(viewCam) GetPosition]
		eval $currentLight SetFocalPoint [$View(viewCam) GetFocalPoint]
	}
}

#-------------------------------------------------------------------------------
# .PROC MainMainViewPresetCallback
# .END
#-------------------------------------------------------------------------------
proc MainMainViewPresetCallback {btn} {
	global View Target Gui

	if {$View(state$btn) == "Press"} {
		$View(fPreset).c$btn config -activebackground red
		# Set preset value to current view
		set View(position$btn)      [$View(viewCam) GetPosition]
		set View(viewUp$btn)        [$View(viewCam) GetViewUp]
		set View(focalPoint$btn)    [$View(viewCam) GetFocalPoint]
		set View(clippingRange$btn) [$View(viewCam) GetClippingRange]
	} else {
		# Set view to the preset value
		eval $View(viewCam) SetPosition      $View(position$btn)
		eval $View(viewCam) SetViewUp        $View(viewUp$btn)
		eval $View(viewCam) SetFocalPoint    $View(focalPoint$btn)
		eval $View(viewCam) SetClippingRange $View(clippingRange$btn)
		$View(viewCam) ComputeViewPlaneNormal
		$View(viewCam) OrthogonalizeViewUp

		MainViewLightFollowCamera
	}
}

#-------------------------------------------------------------------------------
# .PROC MainViewPreset
# .END
#-------------------------------------------------------------------------------
proc MainViewPreset {btn state} {
	global View Gui

	if {$state == "Press"} {
		set View(state$btn) $state
		after 250 "MainMainViewPresetCallback $btn; Render3D"
	} else {
		set View(state$btn) $state
		$View(fPreset).c$btn config -activebackground $Gui(activeButton) 
	}
}

#-------------------------------------------------------------------------------
# .PROC MainViewNavReset
#
# Returns 1 if window should be rendered
# .END
#-------------------------------------------------------------------------------
proc MainViewNavReset {x y {cmd ""}} {
	global dirWin View Target Gui

	set iswin 0
	if {$View(createDirWin) == "Yes"} {
		set iswin 1
	}

	if {$cmd == "leave"} {
		if {$iswin == 1} {
			dirMapper SetZSlice 0
			dirWin Render
			$Gui(fDir) config -cursor top_left_arrow
		}
		return 0
	}
	set directions "R L A P S I"
	set xList  "10 65 55 17 40 40"
	set yList  "36 23 42 13 10 65"

	set fp [$View(viewCam) GetFocalPoint]
	set r [lindex $fp 0]
	set a [lindex $fp 1]
	set s [lindex $fp 2]

	foreach dir $directions {
		set i [lsearch $directions $dir]
		set X [lindex  $xList  $i]
		set Y [lindex  $yList  $i]

		if {$x > [expr $X - 10] && $x < [expr $X + 10] && \
			$y > [expr $Y - 10] && $y < [expr $Y + 10]} {
			
			if {$iswin == 1} {
				$Gui(fDir) config -cursor hand2
				set slice [expr $i + 1]
				if {$slice != [dirMapper GetZSlice]} {
					dirMapper SetZSlice $slice
					dirWin Render
				}
			}
			if {$cmd == "click"} {
				set d [expr $View(fov) * 3]

				switch $dir {
					R {
						$View(viewCam) SetPosition   [expr $r+$d] $a   $s
						$View(viewCam) SetViewUp     0   0   1
					}
					A {
						$View(viewCam) SetPosition   $r   [expr $a+$d] $s
						$View(viewCam) SetViewUp     0   0   1
					}
					S {
						$View(viewCam) SetPosition    $r   $a    [expr $s+$d]
						$View(viewCam) SetViewUp     0   1   0
					}
					L {
						$View(viewCam) SetPosition   [expr $r-$d] $a   $s
						$View(viewCam) SetViewUp     0   0   1
					}
					P {
						$View(viewCam) SetPosition    $r   [expr $a-$d] $s
						$View(viewCam) SetViewUp     0   0   1
					}
					I {
						$View(viewCam) SetPosition    $r   $a    [expr $s-$d]
						$View(viewCam) SetViewUp     0   1   0
					}
				}
				eval $View(viewCam) SetClippingRange $View(baselineClippingRange)
				$View(viewCam) ComputeViewPlaneNormal
				$View(viewCam) OrthogonalizeViewUp

				MainViewLightFollowCamera
				return 1
			}
			return 0
		}
	}
	if {$iswin == 1} {
		if {0 != [dirMapper GetZSlice]} {
			dirMapper SetZSlice 0
			dirWin Render
		}
		$Gui(fDir) config -cursor top_left_arrow
	}
}

#-------------------------------------------------------------------------------
# .PROC MainViewRotate
# .END
#-------------------------------------------------------------------------------
proc MainViewRotate {dir {deg rotate}} {
	global View

	if {$deg == "rotate"} {
		set p $View(rotateDegrees)
	} else {
		set p $deg
	}			
	set n [expr -$p]
	
	switch $dir {
		Down  {$View(viewCam) Elevation $p }
		Up    {$View(viewCam) Elevation $n }
		Left  {$View(viewCam) Azimuth $p }
		Right {$View(viewCam) Azimuth $n }
	}
	$View(viewCam) OrthogonalizeViewUp

	MainViewLightFollowCamera
	Render3D
}

#-------------------------------------------------------------------------------
# .PROC MainViewNavRotate
# .END
#-------------------------------------------------------------------------------
proc MainViewNavRotate {W x y {cmd ""}} {

	set directions "Up Down Left Right"
	set xList      "28 28 9 47"
	set yList      " 9 47 28 28"

	if {$cmd == "leave"} {
		$W config -cursor top_left_arrow -image iRotateNone
		return
	}
	foreach dir $directions {
		set i [lsearch $directions $dir]
		set X [lindex  $xList  $i]
		set Y [lindex  $yList  $i]

		if {$x > [expr $X - 10] && $x < [expr $X + 10] && \
			$y > [expr $Y - 10] && $y < [expr $Y + 10]} {
			$W config -cursor hand2 -image iRotate${dir}
			if {$cmd == "click"} {
			    MainViewRotate $dir
			}
			return
		}
	}
	$W config -cursor top_left_arrow -image iRotateNone
}

#-------------------------------------------------------------------------------
# .PROC MainViewSetStereo
# .END
#-------------------------------------------------------------------------------
proc MainViewSetStereo {} {
	global viewWin View

	if {$View(stereo) == "1"} {
		$viewWin SetStereoTypeTo$View(stereoType)
		$viewWin StereoRenderOn
	} else {
		$viewWin StereoRenderOff
	}
}

#-------------------------------------------------------------------------------
# .PROC MainViewSpin
# .END
#-------------------------------------------------------------------------------
proc MainViewSpin {} {
	global View

	if {$View(spin) == "1"} {
		MainViewRotate $View(spinDir) $View(spinDegrees)
		update idletasks
		after $View(spinMs) MainViewSpin
	}
}

#-------------------------------------------------------------------------------
# .PROC MainViewSetWelcome
# .END
#-------------------------------------------------------------------------------
proc MainViewSetWelcome {win} {
	global Edit Gui Slice View

	if {$View(createMagWin) == "No"} {
		set win Controls
	}

	# Do nothing if no change
	if {$win == $View(magWin)} {return}

	if {$win == "Controls"} {
		raise $Gui(fNav)
	} elseif {$win == "Welcome"} {
		if {$View(magWin) == "Controls"} {
			raise $Gui(fMagBorder)
		}
		magMapper SetInput [welcomeReader GetOutput]
		magWin Render
	} else {
		if {$View(magWin) == "Controls"} {
			raise $Gui(fMagBorder)
		}
		if {$View(closeupVisibility) == "On"} {
			set s [string index $win 2]
			View(mag) SetInput [Slicer GetActiveOutput $s]
			magMapper SetInput [View(magCursor) GetOutput]
			magWin Render
		}
	}
	
	set View(magWin) $win

}
		
#-------------------------------------------------------------------------------
# .PROC MainViewResetFocalPoint
# .END
#-------------------------------------------------------------------------------
proc MainViewResetFocalPoint {} {
	global View Slice
				
	MainViewSetFocalPoint 0 0 0

	# Zoom
	foreach s $Slice(idList) {
#		Slicer SetZoomCenter $s -1 -1
	}
}

#-------------------------------------------------------------------------------
# .PROC MainViewSetFocalPoint
# .END
#-------------------------------------------------------------------------------
proc MainViewSetFocalPoint {x y z} {
	global Slice View

	set View(focalPoint) "$x $y $z"
	eval $View(viewCam) SetFocalPoint $View(focalPoint)
	$View(viewCam) ComputeViewPlaneNormal
	$View(viewCam) OrthogonalizeViewUp

	MainViewLightFollowCamera

	#SLICES
	Slicer ComputeNTPFromCamera $View(viewCam)
	foreach s $Slice(idList) {
		if {[lsearch "Axial Sagittal Coronal Perp InPlane0 InPlane90 \
			InPlaneNeg90" [Slicer GetOrientString $s]] != -1} {
			MainSlicesSetOffset $s 0
		}
	}

	MainAnnoUpdateFocalPoint $x $y $z
}

#-------------------------------------------------------------------------------
# .PROC SaveScreen
# .END
#-------------------------------------------------------------------------------
proc SaveScreen {} {
	global viewWin Gui View

    set filename [GetSaveFile $Gui(root) $View(image3D) "" ppm "" 0]
    if {$filename == ""} {return}

	set filename [file join $Gui(root) $filename]
	$viewWin SetFileName $filename
	$viewWin SaveImageAsPPM
	puts "Saved screen: $filename"
}

#-------------------------------------------------------------------------------
# .PROC SaveImage3D
# .END
#-------------------------------------------------------------------------------
proc SaveImage3D {} {
	global View Gui viewWin

 	set typelist {
		{"PPM File" {".ppm"}}
		{"All Files" {*}}
	}
	set filename [GetSaveFile $Gui(root) $View(image3D)  \
		$typelist ppm "Save 3D Screen"]
	if {$filename == ""} {return}

	# Store for next time
	set code [DecodeFileName $filename]
	set Gui(prefixSaveImage3D) [lindex $code 1]
	set View(image3D) $Gui(prefixSaveImage3D)

	set filename [file join $Gui(root) $filename]
	$viewWin SetFileName $filename
	$viewWin SaveImageAsPPM
	puts "Saved image: $filename"
}

