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
# FILE:        MainInteractor.tcl
# DATE:        02/22/2000 11:11
# PROCEDURES:  
#   MainInteractorInit
#   MainInteractorBind
#   MainInteractorXY
#   MainInteractorCursor
#   MainInteractorKeyPress
#   MainInteractorMotion
#   MainInteractorB1
#   MainInteractorShiftB1
#   MainInteractorB1Release
#   MainInteractorShiftB1Release
#   MainInteractorB1Motion
#   MainInteractorControlB1Motion
#   MainInteractorAltB1Motion
#   MainInteractorB2Motion
#   MainInteractorB3Motion
#   MainInteractorPan
#   MainInteractorZoom
#   MainInteractorWindowLevel
#   MainInteractorThreshold
#   MainInteractorExpose
#   MainInteractorRender
#   MainInteractorEnter
#   MainInteractorExit
#   MainInteractorStartMotion
#   MainInteractorEndMotion
#   MainInteractorReset
#   PixelsToMm
#   Distance3D
#   Angle2D
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC MainInteractorInit
# .END
#-------------------------------------------------------------------------------
proc MainInteractorInit {} {
	global Interactor

	set Interactor(s)      0
	set Interactor(yLast)  0
	set Interactor(xLast)  0
	set Interactor(ysLast) 0
	set Interactor(xsLast) 0
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorBind
# .END
#-------------------------------------------------------------------------------
proc MainInteractorBind {widget} {
	global Gui

	# NOTE: <Motion> is not called when <B1-Motion> is called
	#       <Any-ButtonPress> is not called when <Button-1> is called
	
	# Cursor
	bind $widget <Enter>             {MainInteractorEnter %W %x %y;}
	bind $widget <Leave>             {MainInteractorExit %W}
	bind $widget <Expose>            {MainInteractorExpose %W}
	bind $widget <Motion>            {MainInteractorMotion %W %x %y}

	# Any mouse button
	bind $widget <Any-ButtonPress>   {MainInteractorStartMotion %W %x %y}
	bind $widget <Any-ButtonRelease> {MainInteractorEndMotion %W %x %y}
	# B1
	bind $widget <ButtonPress-1>     {MainInteractorB1 %W %x %y}
	bind $widget <ButtonRelease-1>   {MainInteractorB1Release %W %x %y}
	bind $widget <B1-Motion>         {MainInteractorB1Motion %W %x %y}
	# B2
	bind $widget <B2-Motion>         {MainInteractorB2Motion %W %x %y}
	# B3
	bind $widget <B3-Motion>         {MainInteractorB3Motion %W %x %y}

	# Shift-B1
	bind $widget <Shift-B1-Motion>        {MainInteractorB2Motion %W %x %y}
	bind $widget <Shift-ButtonPress-1>    {MainInteractorShiftB1 %W %x %y}
	bind $widget <Shift-ButtonRelease-1> \
		{MainInteractorShiftB1Release %W %x %y}

	# Control-B1
	bind $widget <Control-B1-Motion>      {MainInteractorControlB1Motion %W %x %y}
	bind $widget <Control-ButtonPress-1>  {MainInteractorShiftB1 %W %x %y}
	bind $widget <Control-ButtonRelease-1> \
		{MainInteractorShiftB1Release %W %x %y}

	# Alt-B1
	bind $widget <Alt-B1-Motion>          {MainInteractorAltB1Motion %W %x %y}
	bind $widget <Alt-ButtonPress-1>      {MainInteractorShiftB1 %W %x %y}
	bind $widget <Alt-ButtonRelease-1> \
		{MainInteractorShiftB1Release %W %x %y}

	# Keyboard
	bind $widget <KeyPress-r>        {MainInteractorReset %W %x %y}
	bind $widget <Up>                {MainInteractorKeyPress Up    %W %x %y}
	bind $widget <Down>              {MainInteractorKeyPress Down  %W %x %y}
	bind $widget <Left>              {MainInteractorKeyPress Left  %W %x %y}
	bind $widget <Right>             {MainInteractorKeyPress Right %W %x %y}
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorXY
#
# returns 'xs ys x y'
#
# (xs, ys) is the point relative to the lower, left corner 
# of the slice window (0-255 or 0-511).
#
# (x, y) is the point with Zoom and Double taken into consideration
# (zoom=2 means range is 64-128 instead of 1-256)
# .END
#-------------------------------------------------------------------------------
proc MainInteractorXY {s x y} {
    global Interactor

	# Compute screen coordinates
	set y [expr $Interactor(ySize) - 1 - $y]
	set xs $x
	set ys $y

	# Convert Screen coordinates to Reformatted image coordinates
	Slicer SetScreenPoint $s $x $y
	scan [Slicer GetReformatPoint] "%d %d" x y

	return "$xs $ys $x $y"
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorCursor
# .END
#-------------------------------------------------------------------------------
proc MainInteractorCursor {s xs ys x y} {
    global Anno View Interactor

	# pixel value
	set forePix [Slicer GetForePixel $s $x $y]
	set backPix [Slicer GetBackPixel $s $x $y]

	# Get RAS and IJK coordinates
	Slicer SetReformatPoint $s $x $y
	scan [Slicer GetWldPoint] "%g %g %g" xRas yRas zRas 
	scan [Slicer GetIjkPoint] "%g %g %g" xIjk yIjk zIjk

	# Write Annotation
	if {$Anno(mouse) == 1} {
		foreach name "$Anno(mouseList)" {
			if {$name != "msg"} {
				# Warning: actor may not exist yet, so check!
				if {[info command Anno($s,$name,actor)] != ""} {
					Anno($s,$name,actor) SetVisibility 1
				}
			}
		}
		if {[info command Anno($s,cur1,mapper)] != ""} {
			switch $Anno(cursorMode) {
			"RAS" {
				Anno($s,cur1,mapper) SetInput [format "R %.f" $xRas]
				Anno($s,cur2,mapper) SetInput [format "A %.f" $yRas]
				Anno($s,cur3,mapper) SetInput [format "S %.f" $zRas]
			}
			"IJK" {
				Anno($s,cur1,mapper) SetInput [format "I %.f" $xIjk]
				Anno($s,cur2,mapper) SetInput [format "J %.f" $yIjk]
				Anno($s,cur3,mapper) SetInput [format "K %.f" $zIjk]
			}
			"XY" {
				Anno($s,cur1,mapper) SetInput [format "X %.f" $x]
				Anno($s,cur2,mapper) SetInput [format "Y %.f" $y]
				Anno($s,cur3,mapper) SetInput " "
			}
			}
		}
		if {[info command Anno($s,curBack,mapper)] != ""} {
			Anno($s,curBack,mapper) SetInput [format "Bg %d" $backPix]
			Anno($s,curFore,mapper) SetInput [format "Fg %d" $forePix]
		}
	} else {
		foreach name "$Anno(mouseList)" {
			if {[info command Anno($s,$name,actor)] != ""} {
				Anno($s,$name,actor) SetVisibility 0
			}
		}
	}

	# Move cursor
	Slicer SetCursorPosition $s $xs $ys
	
	# Show close-up image
	if {$View(createMagWin) == "Yes" && $View(closeupVisibility) == "On"} {
		View(mag) SetX $x
		View(mag) SetY $y
	}
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorKeyPress
#
# Called when a key is pressed in the 2D window.
# Deals with Up, Down, Left and Right.
#
# Up and Down moves the slice offset.
# Left and Right calles EditApplyFilter from Edit.tcl
# .END
#-------------------------------------------------------------------------------
proc MainInteractorKeyPress {key widget x y} {
	global View Slice Interactor Module Editor

	# Determine which slice this is
	set s $Interactor(s)
	if {$s == ""} {return}

	switch $key {
	"Right" {
		MainSlicesSetOffset $s Next;
		MainSlicesRefreshClip $s

		scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 
		MainInteractorCursor $s $xs $ys $x $y
		MainInteractorRender
	}
	"Left" {
		MainSlicesSetOffset $s Prev;
		MainSlicesRefreshClip $s

		scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 
		MainInteractorCursor $s $xs $ys $x $y
		MainInteractorRender
	}
	"Up" {
		switch $Module(activeID) {
		"Editor" {
			$Editor(activeID)Apply
		}
		}
	}
	"Down" {
		switch $Module(activeID) {
		"Editor" {
			switch $Editor(activeID) {
			"EdDraw" {
				EdDrawUpdate NextMode
			}
			}
		}
		}
	}
	}
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorMotion
# .END
#-------------------------------------------------------------------------------
proc MainInteractorMotion {widget x y} {
	global Interactor

	set s $Interactor(s)
	scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

	# Cursor
	MainInteractorCursor $s $xs $ys $x $y

	# Render this slice
	MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorB1
# .END
#-------------------------------------------------------------------------------
proc MainInteractorB1 {widget x y} {
	global Interactor Module
	
	set s $Interactor(s)
	scan [MainInteractorStartMotion $widget $x $y] "%d %d %d %d" xs ys x y 

	# Here is a switch statement for the current active module,
	# but it really should be Peter Everett's stack for bindings.
	switch $Module(activeID) {
	"Editor" {
		EditorB1 $x $y
	}
	"Matrices" {
		MatricesB1 $x $y
	}
	}

	# Cursor
	MainInteractorCursor $s $xs $ys $x $y

	MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorShiftB1
# .END
#-------------------------------------------------------------------------------
proc MainInteractorShiftB1 {widget x y} {
	global Interactor
	
	set s $Interactor(s)
	scan [MainInteractorStartMotion $widget $x $y] "%d %d %d %d" xs ys x y 

	MainInteractorCursor $s $xs $ys $x $y
	MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorB1Release
# .END
#-------------------------------------------------------------------------------
proc MainInteractorB1Release {widget x y} {
	global Interactor Module
	
	set s $Interactor(s)
	scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

	# Cursor
	MainInteractorCursor $s $xs $ys $x $y
	
	switch $Module(activeID) {
	"Editor" {
		EditorB1Release $x $y
	}
	"Matrices" {
		MatricesB1Release $x $y
	}
	}

	Anno($s,msg,actor)  SetVisibility 0
	MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorShiftB1Release
# .END
#-------------------------------------------------------------------------------
proc MainInteractorShiftB1Release {widget x y} {
	global Interactor Module
	
	set s $Interactor(s)
	scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

	# Cursor
	MainInteractorCursor $s $xs $ys $x $y

	Anno($s,msg,actor)  SetVisibility 0
	RenderAll
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorB1Motion
# .END
#-------------------------------------------------------------------------------
proc MainInteractorB1Motion {widget x y} {
	global Interactor Module
	
	set s $Interactor(s)
	scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

	switch $Module(activeID) {
	"Editor" {
		EditorB1Motion $x $y
	}
	"Matrices" {
		MatricesB1Motion $x $y
	}
	}

	# Cursor
	MainInteractorCursor $s $xs $ys $x $y

	set Interactor(xLast)  $x
	set Interactor(yLast)  $y
	set Interactor(xsLast) $xs
	set Interactor(ysLast) $ys

	# Render this slice
	MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorControlB1Motion
# .END
#-------------------------------------------------------------------------------
proc MainInteractorControlB1Motion {widget x y} {
	global Interactor
	
	set s $Interactor(s)
	scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

	# W/L using screen coordinates so that the same number
	# of screen pixels covered (not % of image) produces the
	# same zoom factor. To put it another way, I want the 
	# user to have to drag the mouse a consistent distance 
	# across the mouse pad.
	#
	# BUG: I'm commenting this out because window/leveling for too long
	# causes a stack overflow.
#	MainInteractorWindowLevel $s $xs $ys $Interactor(xsLast) $Interactor(ysLast)

	# Cursor
	MainInteractorCursor $s $xs $ys $x $y

	set Interactor(xLast)  $x
	set Interactor(yLast)  $y
	set Interactor(xsLast) $xs
	set Interactor(ysLast) $ys

	# Render this slice
	MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorAltB1Motion
# .END
#-------------------------------------------------------------------------------
proc MainInteractorAltB1Motion {widget x y} {
	global Interactor
	
	set s $Interactor(s)
	scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

	# Threshold using screen coordinates so that the same number
	# of screen pixels covered (not % of image) produces the
	# same zoom factor. To put it another way, I want the 
	# user to have to drag the mouse a consistent distance 
	# across the mouse pad.
	#
	# BUG: I'm commenting this out because window/leveling for too long
	# causes a stack overflow.
#	MainInteractorThreshold $s $xs $ys $Interactor(xsLast) $Interactor(ysLast)

	# Cursor
	MainInteractorCursor $s $xs $ys $x $y

	set Interactor(xLast)  $x
	set Interactor(yLast)  $y
	set Interactor(xsLast) $xs
	set Interactor(ysLast) $ys

	# Render this slice
	MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorB2Motion
# .END
#-------------------------------------------------------------------------------
proc MainInteractorB2Motion {widget x y} {
	global Interactor
	
	set s $Interactor(s)
	scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

	# I want the image feature under the mouse to follow
	# the mouse around the screen during panning.
	# 
	MainInteractorPan $s $xs $ys $Interactor(xsLast) $Interactor(ysLast)

	# Cursor
	MainInteractorCursor $s $xs $ys $x $y

	set Interactor(xLast)  $x
	set Interactor(yLast)  $y
	set Interactor(xsLast) $xs
	set Interactor(ysLast) $ys

	# Render this slice
	MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorB3Motion
# .END
#-------------------------------------------------------------------------------
proc MainInteractorB3Motion {widget x y} {
	global Interactor

	set s $Interactor(s)
	scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

	# Zoom using screen coordinates so that the same number
	# of screen pixels covered (not % of image) produces the
	# same zoom factor. To put it another way, I want the 
	# user to have to drag the mouse a consistent distance 
	# across the mouse pad.
	#
	MainInteractorZoom $s $xs $ys $Interactor(xsLast) $Interactor(ysLast)

	# Cursor
	MainInteractorCursor $s $xs $ys $x $y

	set Interactor(xLast)  $x
	set Interactor(yLast)  $y
	set Interactor(xsLast) $xs
	set Interactor(ysLast) $ys

	# Render this slice
	MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorPan
# .END
#-------------------------------------------------------------------------------
proc MainInteractorPan {s x y xLast yLast} {
	global View

	set dx [expr $xLast - $x]
	set dy [expr $yLast - $y]
	Slicer GetZoomCenter
	scan [Slicer GetZoomCenter$s] "%g %g" cx cy

	set z [Slicer GetZoom $s]
	if {$View(mode) == "Quad512"} {
		set z [expr $z * 2.0]
	}

	if {[Slicer GetZoomAutoCenter $s] == 1} {
		Slicer SetZoomAutoCenter $s 0
		Slicer Update
	}
	set cx [expr $cx + $dx / $z]
	set cy [expr $cy + $dy / $z]	
	Slicer SetZoomCenter $s $cx $cy
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorZoom
# .END
#-------------------------------------------------------------------------------
proc MainInteractorZoom {s x y xLast yLast} {

	set dy [expr $yLast - $y]

	# log base b of x = log(x) / log(b)
	set b      1.02
	set zPrev  [Slicer GetZoom $s]
	set dyPrev [expr log($zPrev) / log($b)]

	set zoom [expr pow($b, ($dy + $dyPrev))]
	if {$zoom < 0.01} {
		set zoom 0.01
	}
	set z [format "%.2f" $zoom]

	Anno($s,msg,mapper)  SetInput "ZOOM: x $z"

	MainSlicesSetZoom $s $z
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorWindowLevel
# .END
#-------------------------------------------------------------------------------
proc MainInteractorWindowLevel {s x y xLast yLast} {
	global View Volume

	set dx [expr $xLast - $x]
	set dy [expr $yLast - $y]

	set v [[[Slicer GetBackVolume $s] GetMrmlNode] GetID]
	MainVolumesSetActive $v

	set window [Volume($v,node) GetWindow]
	set level  [Volume($v,node) GetLevel]

	set window [expr int($window - $dx)] 
	set level  [expr int($level  - $dy)]

	MainVolumesSetParam Window $window
	MainVolumesSetParam Level  $level

	set window [Volume($v,node) GetWindow]
	set level  [Volume($v,node) GetLevel]
	Anno($s,msg,mapper)  SetInput "Window: $window, Level: $level"
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorThreshold
# .END
#-------------------------------------------------------------------------------
proc MainInteractorThreshold {s x y xLast yLast} {
	global View Volume

	set dx [expr $xLast - $x]
	set dy [expr $yLast - $y]

	set v [[[Slicer GetBackVolume $s] GetMrmlNode] GetID]
	MainVolumesSetActive $v

	set lower [Volume($v,node) GetLowerThreshold]
	set upper [Volume($v,node) GetUpperThreshold]

	set lower [expr int($lower - $dx)] 
	set upper [expr int($upper - $dy)]

	MainVolumesSetParam LowerThreshold $lower
	MainVolumesSetParam UpperThreshold $upper

	set lower [Volume($v,node) GetLowerThreshold]
	set upper [Volume($v,node) GetUpperThreshold]
	Anno($s,msg,mapper)  SetInput "Threshold: $lower...$upper"
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorExpose
# a litle more complex than just "bind $widget <Expose> {%W Render}"
# we have to handle all pending expose events otherwise they que up.
# .END
#-------------------------------------------------------------------------------
proc MainInteractorExpose {widget} {

   # Do not render if we are already rendering
   if {[GetWidgetVariableValue $widget Rendering] == 1} {
      return
   }

   # empty the que of any other expose events
   SetWidgetVariableValue $widget Rendering 1
   update
   SetWidgetVariableValue $widget Rendering 0

   # ignore the region to redraw for now.
   $widget Render
}
 
#-------------------------------------------------------------------------------
# .PROC MainInteractorRender
# .END
#-------------------------------------------------------------------------------
proc MainInteractorRender {} {
	global Interactor View

	RenderSlice $Interactor(s)
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorEnter
# .END
#-------------------------------------------------------------------------------
proc MainInteractorEnter {widget x y} {
    global Interactor

	# Determine what slice this is
	set s [string index $widget [expr [string length $widget] - 4]]
	set Interactor(s) $s

	# Display this slice in the mag window
	MainViewSetWelcome sl$s

	# Focus
	set Interactor(oldFocus) [focus]
    focus $widget

	# Get the renderer window dimensions
    set Interactor(xSize) [lindex [$widget configure -width] 4]
    set Interactor(ySize) [lindex [$widget configure -height] 4]

	set Interactor(xCenter) [expr double($Interactor(xSize))/2.0]
	set Interactor(yCenter) [expr double($Interactor(ySize))/2.0]
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorExit
# .END
#-------------------------------------------------------------------------------
proc MainInteractorExit {widget} {
	global Interactor Anno

	set s $Interactor(s)
	
	# Center cursor
	Slicer SetCursorPosition $s \
		[expr int($Interactor(xCenter))] [expr int($Interactor(yCenter))]

	# Turn off cursor anno
	foreach name "$Anno(mouseList)" {
		if {[info command Anno($s,$name,actor)] != ""} {
			Anno($s,$name,actor) SetVisibility 0
		}
	}

	# Render
	MainInteractorRender

	# Return the focus
	focus $Interactor(oldFocus)
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorStartMotion
# .END
#-------------------------------------------------------------------------------
proc MainInteractorStartMotion {widget x y} {
	global Interactor

	set s $Interactor(s)
	MainSlicesSetActive $s

	scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 
	set Interactor(xLast)  $x
    set Interactor(yLast)  $y
	set Interactor(xsLast) $xs
    set Interactor(ysLast) $ys

	Anno($s,msg,mapper)  SetInput ""
	Anno($s,msg,actor)   SetVisibility 1

	return "$xs $ys $x $y"
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorEndMotion
# .END
#-------------------------------------------------------------------------------
proc MainInteractorEndMotion {widget x y} {
	global Interactor

	set s $Interactor(s)
	scan [MainInteractorXY $s $x $y] "%d %d %d %d" xs ys x y 

	# Cursor
	MainInteractorCursor $s $xs $ys $x $y

	Anno($s,msg,actor)  SetVisibility 0

	MainInteractorRender
}

#-------------------------------------------------------------------------------
# .PROC MainInteractorReset
# .END
#-------------------------------------------------------------------------------
proc MainInteractorReset {widget x y} {
	global Interactor

	MainSlicesResetZoomAll
	MainInteractorRender
	RenderSlices
}

#-------------------------------------------------------------------------------
# .PROC PixelsToMm
# .END
#-------------------------------------------------------------------------------
proc PixelsToMm {pix fov dim mag} {
	# mm = pix * fov/dim / mag
	# pix = mm * dim/fox * mag

	return [expr int($pix * $fov/$dim / $mag + 0.5)]
}

#-------------------------------------------------------------------------------
# .PROC Distance3D
# .END
#-------------------------------------------------------------------------------
proc Distance3D {x1 y1 z1 x2 y2 z2} {
	set dx [expr $x2 - $x1]
	set dy [expr $y2 - $y1]
	set dz [expr $z2 - $z1]
	return [expr sqrt($dx*$dx + $dy*$dy + $dz*$dz)]
}

#-------------------------------------------------------------------------------
# .PROC Angle2D
# .END
#-------------------------------------------------------------------------------
proc Angle2D {ax1 ay1 ax2 ay2 bx1 by1 bx2 by2} {

	# Form vector 'a'=[ax ay] with magnitude 'am' 
	set ax [expr $ax2 - $ax1]
	set ay [expr $ay2 - $ay1]
	set am [expr sqrt($ax*$ax + $ay*$ay)]

	# Form vector 'b'=[bx by] with magnitude 'bm' 
	set bx [expr $bx2 - $bx1]
	set by [expr $by2 - $by1]
	set bm [expr sqrt($bx*$bx + $by*$by)]

	# Find angle between a, b from their dot product
	set dot [expr $ax*$bx + $ay*$by]
	if {$am == 0 || $bm == 0} {return 0}
	set deg [expr acos($dot/($am*$bm))]

	# See if angle is negative from the cross product of a and b
	if {$ax*$by - $bx*$ay > 0} {
		set deg [expr -$deg] 
	}

	return [expr $deg*180/3.1415962]
}

