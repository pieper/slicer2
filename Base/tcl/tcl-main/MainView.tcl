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
# FILE:        MainView.tcl
# DATE:        01/20/2000 09:41
# LAST EDITOR: gering
# PROCEDURES:  
#   MainViewInit
#   MainViewBuildVTK
#   MainViewBuildGUI
#   MainViewSetBackgroundColor
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
#   MainViewSaveView
#   MainViewSaveViewPopup
#   MainViewWriteView
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MainViewInit
# .END
#-------------------------------------------------------------------------------
proc MainViewInit {} {
	global Module View Gui Path Preset Volume

	# The MainViewBuildGUI proc is called specifically
	lappend Module(procVTK)  MainViewBuildVTK

	set View(viewerHeightNormal) 656
	set View(viewerWidth)  956 
	set View(viewerHeight) 956 
	if {$Gui(pc) == 1} {
		set View(viewerHeightNormal) 400
		set View(viewerWidth)  700 
		set View(viewerHeight) 700 
	}

	# Configurable
	set View(mode) Normal
	set View(viewerWidth)  700 
	set View(viewerHeight) 700 
	set View(toolbarPosition) Top
	set View(bgColorR) .7
	set View(bgColorG) .7
	set View(bgColorB) .9
	set View(bgName) Blue
	set View(fov) 220.0
	set View(spin) 0
	set View(spinDir) Right
	set View(spinMs) 5 
	set View(spinDegrees) 2 
	set View(stereo) 0
	set View(stereoType) RedBlue
	set View(closeupVisibility) On
	set View(createMagWin) Yes

	# Bug in OpenGL on Windows98 version II ??
	if {$Gui(pc) == 1} {
		set View(createMagWin) No
		set View(closeupVisibility) Off
	}

	# Init
	set View(rotateDegrees) 15
	set View(baselineClippingRange) "1 2001"
	set View(viewUp)         "0 0 1"
	set View(position)       "0 750 0"
	set View(focalPoint)     "0 0 0"
	set View(clippingRange)  "1 2001"

	set View(magWin) Welcome
	set View(inWin) none
	set View(viewPrefix) view
	set View(ext) .tif

	# Preset View
	set Preset(1,viewUp)        "-0.5 0.866 0"
	set Preset(1,position)      "624 360 0"
	set Preset(1,focalPoint)    "0 0 0"
	set Preset(1,clippingRange) "1 2001"
	set Preset(2,viewUp)        "0.5 0.866 0"
	set Preset(2,position)      "-624 360 0"
	set Preset(2,focalPoint)    "0 0 0"
	set Preset(2,clippingRange) "1 2001"
	set Preset(3,viewUp)        "-0.224 -0.129 0.959"
	set Preset(3,position)      "602 348 186"
	set Preset(3,focalPoint)    "0 0 0"
	set Preset(3,clippingRange) "1 2001"

	# Preset Slice
	set Preset(1,opacity)      1.0
	set Preset(1,0,visibility) 0
	set Preset(1,0,backVolID)  0
	set Preset(1,0,foreVolID)  0
	set Preset(1,0,labelVolID) 0
	set Preset(1,0,orient)     Axial
	set Preset(1,0,offset)     0
	set Preset(1,0,zoom)       1.0
	set Preset(1,1,visibility) 0
	set Preset(1,1,backVolID)  0
	set Preset(1,1,foreVolID)  0
	set Preset(1,1,labelVolID) 0
	set Preset(1,1,orient)     Sagittal
	set Preset(1,1,offset)     0
	set Preset(1,1,zoom)       1.0
	set Preset(1,2,visibility) 0
	set Preset(1,2,backVolID)  0
	set Preset(1,2,foreVolID)  0
	set Preset(1,2,labelVolID) 0
	set Preset(1,2,orient)     Coronal
	set Preset(1,2,offset)     0
	set Preset(1,2,zoom)       1.0

	set Preset(2,opacity)      1.0
	set Preset(2,0,visibility) 0
	set Preset(2,0,backVolID)  0
	set Preset(2,0,foreVolID)  0
	set Preset(2,0,labelVolID) 0
	set Preset(2,0,orient)     Axial
	set Preset(2,0,offset)     0
	set Preset(2,0,zoom)       1.0
	set Preset(2,1,visibility) 0
	set Preset(2,1,backVolID)  0
	set Preset(2,1,foreVolID)  0
	set Preset(2,1,labelVolID) 0
	set Preset(2,1,orient)     Sagittal
	set Preset(2,1,offset)     0
	set Preset(2,1,zoom)       1.0
	set Preset(2,2,visibility) 0
	set Preset(2,2,backVolID)  0
	set Preset(2,2,foreVolID)  0
	set Preset(2,2,labelVolID) 0
	set Preset(2,2,orient)     Coronal
	set Preset(2,2,offset)     0
	set Preset(2,2,zoom)       1.0

	set Preset(3,opacity)      1.0
	set Preset(3,0,backVolID)  0
	set Preset(3,0,foreVolID)  0
	set Preset(3,0,labelVolID) 0
	set Preset(3,0,visibility) 0
	set Preset(3,0,orient)     Axial
	set Preset(3,0,offset)     0
	set Preset(3,0,zoom)       1.0
	set Preset(3,1,visibility) 0
	set Preset(3,1,backVolID)  0
	set Preset(3,1,foreVolID)  0
	set Preset(3,1,labelVolID) 0
	set Preset(3,1,orient)     Sagittal
	set Preset(3,1,offset)     0
	set Preset(3,1,zoom)       1.0
	set Preset(3,2,visibility) 0
	set Preset(3,2,backVolID)  0
	set Preset(3,2,foreVolID)  0
	set Preset(3,2,labelVolID) 0
	set Preset(3,2,orient)     Coronal
	set Preset(3,2,offset)     0
	set Preset(3,2,zoom)       1.0

	# Preset Locator
	set Preset(1,locator,visibility) 0
	set Preset(1,0,driver)       User
	set Preset(1,1,driver)       User
	set Preset(1,2,driver)       User

	set Preset(2,locator,visibility) 0
	set Preset(2,0,driver)       User
	set Preset(2,1,driver)       User
	set Preset(2,2,driver)       User

	set Preset(3,locator,visibility) 0
	set Preset(3,0,driver)       User
	set Preset(3,1,driver)       User
	set Preset(3,2,driver)       User

	# Preset Anno
	foreach item "box axes outline letters cross hashes mouse" {
		set Preset(1,anno,$item) 1
		set Preset(2,anno,$item) 1
		set Preset(3,anno,$item) 1
	}
	if {$Gui(pc) == 1} {
		set Preset(1,anno,box) 0
		set Preset(1,anno,axes) 0
		set Preset(1,anno,outline) 0
		set Preset(2,anno,box) 0
		set Preset(2,anno,axes) 0
		set Preset(2,anno,outline) 0
		set Preset(3,anno,box) 0
		set Preset(3,anno,axes) 0
		set Preset(3,anno,outline) 0
	}

	set Preset(filename)       cav.txt
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
	
	#  [ActiveOutput] -----> mag -> magCursor -> magMapper

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
	# Nav
	#	Top
	#	  Dir
	#	  Preset
	#     Move
	#       Rotate
	#   Bot
	#     Btns2
	#     Save
	#     Mode
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
	
	frame $f.fDir     -bg $Gui(activeWorkspace) -bd $Gui(borderWidth) -relief sunken
	frame $f.fPreset  -bg $Gui(activeWorkspace)
	frame $f.fMove    -bg $Gui(activeWorkspace)
	
	pack $f.fDir $f.fPreset $f.fMove -side left -padx 3 -pady 0

	#-------------------------------------------
	# View->Nav->Top->Dir Frame
	#-------------------------------------------
	set f $Gui(fNav).fTop.fDir
	
	# Create control for setting view Direction
	foreach pict "N R L A P S I" {
		image create photo iDir$pict \
			-file [ExpandPath [file join gui "dir$pict.ppm"]]
	}		
	eval {label $f.lDir -image iDirN -width 74 -height 74 -anchor w} $Gui(WLA)
	bind $f.lDir <1>      {if {[MainViewNavReset %x %y click] == 1} {Render3D}}
	bind $f.lDir <Enter>  {MainViewNavReset %x %y      }
	bind $f.lDir <Leave>  {MainViewNavReset %x %y leave}
	bind $f.lDir <Motion> {MainViewNavReset %x %y      }
	pack $f.lDir
	set Gui(fDir) $f.lDir

	#-------------------------------------------
	# View->Nav->Top->Preset Frame
	#-------------------------------------------
	set f $Gui(fNav).fTop.fPreset
	set View(fPreset) $Gui(fNav).fTop.fPreset
	
	# Preset Button
    foreach p "1 2 3" {
		set c "button $f.c$p -text $p -width 2 $Gui(WBA)"
			eval [subst $c]
		bind $f.c$p <ButtonPress>   "MainViewPreset $p Press"
		bind $f.c$p <ButtonRelease> "MainViewPreset $p Release"
		pack $f.c$p -side top -pady 2 
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
		-command "MainViewSaveView" $Gui(WBA)}; eval [subst $c]
	set c {entry $f.eSave -textvariable View(viewPrefix) $Gui(WEA)}
		eval [subst $c]
	bind $f.eSave <Return> {MainViewSaveViewPopup}
	pack $f.bSave -side left -padx 3
	pack $f.eSave -side left -padx 2 -expand 1 -fill x
	
	#-------------------------------------------
	# Nav->Mode
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
# .PROC MainViewSetBackgroundColor
# .END
#-------------------------------------------------------------------------------
proc MainViewSetBackgroundColor {{col ""}} {
    global View
    
    
    # set View(bgName) if called with an argument
    if {$col != ""} {
	if {$col == "Blue" || $col == "Black"} {
	    set View(bgName) $col
	} else {
	    return
	}   
    }	
	
    switch $View(bgName) {
	"Blue" {
	    set View(bgColorR) .7
	    set View(bgColorG) .7
	    set View(bgColorB) .9
	}
	"Black" {
	    set View(bgColorR) 0
	    set View(bgColorG) 0
	    set View(bgColorB) 0
	}
    }
    eval viewRen SetBackground \
	    $View(bgColorR) $View(bgColorG) $View(bgColorB)
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
proc MainMainViewPresetCallback {p} {
	global View Target Gui Preset Slice Locator Anno Model

	if {$Preset($p,state) == "Press"} {
		$View(fPreset).c$p config -activebackground red

		# Set preset value to the current
		
		# View
		set Preset($p,position)      [$View(viewCam) GetPosition]
		set Preset($p,viewUp)        [$View(viewCam) GetViewUp]
		set Preset($p,focalPoint)    [$View(viewCam) GetFocalPoint]
		set Preset($p,clippingRange) [$View(viewCam) GetClippingRange]

		# Slice
		foreach s $Slice(idList) {
			set Preset($p,$s,visibility) $Slice($s,visibility)
			set Preset($p,$s,orient)     $Slice($s,orient)
			set Preset($p,$s,offset)     $Slice($s,offset)
			set Preset($p,$s,zoom)       $Slice($s,zoom)
			set Preset($p,$s,backVolID)  $Slice($s,backVolID)
			set Preset($p,$s,foreVolID)  $Slice($s,foreVolID)
			set Preset($p,$s,labelVolID) $Slice($s,labelVolID)
		}
		set Preset($p,opacity) $Slice(opacity)

		# Locator
		if {[IsModule Locator] == 1} {
			foreach s $Slice(idList) {
				set Preset($p,$s,driver) $Locator($s,driver)
			}
			set Preset($p,locator,visibility) $Locator(visibility)
		}

		# Anno
		foreach item "box axes outline letters cross hashes mouse" {
			set Preset($p,anno,$item) $Anno($item)
		}		    

		# Models
		foreach m $Model(idList) {
			set Preset($p,$m,visibility) $Model($m,visibility)
puts "visibility saved as: $Preset($p,$m,visibility)"
			set Preset($p,$m,opacity)    $Model($m,opacity)
puts "opacity saved as: $Preset($p,$m,opacity)"
		}		    

	} else {
		# Set current to the preset value
		
		# View
		eval $View(viewCam) SetPosition      $Preset($p,position)
		eval $View(viewCam) SetViewUp        $Preset($p,viewUp)
		eval $View(viewCam) SetFocalPoint    $Preset($p,focalPoint)
		eval $View(viewCam) SetClippingRange $Preset($p,clippingRange)
		$View(viewCam) ComputeViewPlaneNormal
		$View(viewCam) OrthogonalizeViewUp
		MainViewLightFollowCamera
		
		# Slice
		foreach s $Slice(idList) {
			set Slice($s,visibility) $Preset($p,$s,visibility)
			MainSlicesSetVisibility $s
			MainSlicesSetVolume Back $s $Preset($p,$s,backVolID)
			MainSlicesSetVolume Fore $s $Preset($p,$s,foreVolID)
			MainSlicesSetVolume Label $s $Preset($p,$s,labelVolID)
			MainSlicesSetOrient $s $Preset($p,$s,orient)
			MainSlicesSetOffset	$s $Preset($p,$s,offset)
			MainSlicesSetZoom $s $Preset($p,$s,zoom)
		}
		MainSlicesSetOpacityAll $Preset($p,opacity)

		# Locator
		if {[IsModule Locator] == 1} {
			foreach s $Slice(idList) {
				LocatorSetDriver $s $Preset($p,$s,driver)
			}
			set Locator(visibility) $Preset($p,locator,visibility)
			LocatorSetVisibility	
		}

		# Anno
		foreach item "box axes outline letters cross hashes mouse" {
			set Anno($item) $Preset($p,anno,$item)
		}
		MainAnnoSetVisibility

		# Models
		foreach m $Model(idList) {
		    if {[info exists Preset($p,$m,visibility)] == 1} {
			MainModelsSetVisibility $m $Preset($p,$m,visibility)
		    }
		    if {[info exists Preset($p,$m,opacity)] == 1} {
			MainModelsSetOpacity $m $Preset($p,$m,opacity)
		    }
		}		    

	}
}

proc MainViewSavePresets {} {
	global Preset
	
	set fid [open $Preset(filename) w]
	foreach item [lsort [array names Preset]] {
		puts $fid "$item $Preset($item)"
	}
	close $fid
}

proc MainViewLoadPresets {} {
	global Preset
	
	set fid [open $Preset(filename) r]
	gets $fid line
	while {[eof $fid] == 0} {
		set key [lindex $line 0]
		set val [lreplace $line 0 0]
		set Preset($key) $val
		gets $fid line
	}
	close $fid
}

#-------------------------------------------------------------------------------
# .PROC MainViewPreset
# .END
#-------------------------------------------------------------------------------
proc MainViewPreset {p state} {
	global View Gui Preset

	if {$state == "Press"} {
		set Preset($p,state) $state
		after 250 "MainMainViewPresetCallback $p; RenderAll"
	} else {
		set Preset($p,state) $state
		$View(fPreset).c$p config -activebackground $Gui(activeButton) 
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

	if {$cmd == "leave"} {
		$Gui(fDir) config -cursor top_left_arrow -image iDirN
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
			
			$Gui(fDir) config -cursor hand2 -image iDir$dir

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
	$Gui(fDir) config -cursor top_left_arrow -image iDirN
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

	# Do nothing if no change
	if {$win == $View(magWin)} {return}

	if {$win == "Controls"} {
		# The gui may not be created yet
		if {[info exists Gui(fNav)] == 1} {
			raise $Gui(fNav)
		}
	} elseif {$win == "Welcome"} {
		if {[info exists Gui(fWelcome)] == 1} {
			raise $Gui(fWelcome)
		}
	} else {
		if {$View(createMagWin) == "Yes" && $View(closeupVisibility) == "On"} {
			if {[info exists Gui(fMagBorder)] == 1} {
				raise $Gui(fMagBorder)
				set s [string index $win 2]
				View(mag) SetInput [Slicer GetActiveOutput $s]
				magMapper SetInput [View(magCursor) GetOutput]
				magWin Render
			}
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
# .PROC MainViewSaveView
# .END
#-------------------------------------------------------------------------------
proc MainViewSaveView {} {
	global Mrml View

    # Prefix cannot be blank
	if {$View(viewPrefix) == ""} {
		tk_messageBox -message "Please specify a file name."
		return
	}

	# Get a unique filename by appending a number to the prefix
	set filename [MainFileFindUniqueName $Mrml(dir) $View(viewPrefix) $View(ext)]

	MainViewWriteView $filename
}

#-------------------------------------------------------------------------------
# .PROC MainViewSaveViewPopup
# Provide a popup for saving the 3D view to disk.
# See also: MainViewSaveView
# .END
#-------------------------------------------------------------------------------
proc MainViewSaveViewPopup {} {
	global View Mrml Gui

	# Cannot have blank prefix
	if {$View(viewPrefix) == ""} {
		set View(viewPrefix) view
	}

 	# Show popup initialized to the last file saved
	set filename [file join $Mrml(dir) $View(viewPrefix)]
	set dir [file dirname $filename]
	set typelist {
		{"TIFF File" {".tif"}}
		{"PPM File" {".ppm"}}
		{"BMP File" {".bmp"}}
		{"All Files" {*}}
	}
	set filename [tk_getSaveFile -title "Save 3D View" -defaultextension $View(ext)\
		-filetypes $typelist -initialdir "$dir" -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return}

	MainViewWriteView $filename
}

#-------------------------------------------------------------------------------
# .PROC MainViewWriteView
# .END
#-------------------------------------------------------------------------------
proc MainViewWriteView {filename} {
	global viewWin Mrml View Gui

	MainFileCreateDirectory $filename
	
	# Write it
	set ext [file extension $filename]
	switch $ext {
	".tif" {
		vtkWindowToImageFilter filter
		filter SetInput $viewWin

		vtkTIFFWriter writer
		writer SetInput [filter GetOutput]
		writer SetFileName $filename
		writer Write
		filter Delete
		writer Delete
	}
	".bmp" {
		vtkWindowToImageFilter filter
		filter SetInput $viewWin

		vtkBMPWriter writer
		writer SetInput [filter GetOutput]
		writer SetFileName $filename
		writer Write
		filter Delete
		writer Delete
	}
	".ppm" {
		$viewWin SetFileName $filename
		$viewWin SaveImageAsPPM
	}
	}
	puts "Saved view: $filename"

	# Store the new prefix for next time
	set root $Mrml(dir)
	set absPrefix [file rootname $filename]
	if {$Gui(pc) == 1} {
		set absPrefix [string tolower $absPrefix]
		set root [string tolower $Mrml(dir)]
	}
	if {[regexp "^$root/(\[^0-9\]*)(\[0-9\]*)" $absPrefix match relPrefix num] == 1} {
		set View(viewPrefix) $relPrefix
	} else {
		set View(viewPrefix) [file rootname $absPrefix]
	}
	set View(ext) [file extension $filename]
}

