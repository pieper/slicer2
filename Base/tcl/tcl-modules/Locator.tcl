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
# FILE:        Locator.tcl
# DATE:        01/18/2000 12:17
# LAST EDITOR: gering
# PROCEDURES:  
#   LocatorInit
#   LocatorBuildVTK
#   LocatorBuildGUI
#   LocatorSetDriverAll
#   LocatorSetDriver
#   LocatorSetVisibility
#   LocatorSetTransverseVisibility
#   LocatorSetColor
#   LocatorSetSize
#   LocatorSetMatrices
#   LocatorSetPosition
#   LocatorUseLocatorMatrix
#   LocatorFormat
#   LocatorPause
#   LocatorConnect
#   LocatorLoopFile
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC LocatorInit
# .END
#-------------------------------------------------------------------------------
proc LocatorInit {} {
	global Locator Module

	# Define Tabs
	set m Locator
	set Module($m,row1List) "Help Tracking Handpiece Server"
	set Module($m,row1Name) "Help Tracking Handpiece Server"
	set Module($m,row1,tab) Tracking

	# Define Procedures
	set Module($m,procGUI)   LocatorBuildGUI
	set Module($m,procVTK)   LocatorBuildVTK

	set Locator(nx) 0
	set Locator(ny) -1
	set Locator(nz) 0
	set Locator(tx) 1
	set Locator(ty) 0
	set Locator(tz) 0
	set Locator(px) 100 
	set Locator(py) 100
	set Locator(pz) 100
	LocatorFormat

	set Locator(visibility) 0
	set Locator(transverseVisibility) 1
	set Locator(normalLen) 100
	set Locator(transverseLen) 25
	set Locator(radius) 3.0
	set Locator(diffuseBlocked) ".9 .1 .1"
	set Locator(red) .9
	set Locator(green) .9
	set Locator(blue) .2
	set Locator(normalOffset) 0
	set Locator(transverseOffset) 0
	set Locator(crossOffset) 0

	set Locator(connect) 0
	set Locator(pause) 0
	set Locator(loop) 0
	set Locator(updatePeriodInMs) 0
	set Locator(server) File
	set Locator(file) loc.txt
	set Locator(fid) ""

	set Locator(0,driver) User
	set Locator(1,driver) User
	set Locator(2,driver) User
}

#-------------------------------------------------------------------------------
# .PROC LocatorBuildVTK
# .END
#-------------------------------------------------------------------------------
proc LocatorBuildVTK {} {
	global Gui Locator View Slice Target Volume

	#------------------------#
	# Construct handpiece
	#------------------------#
	vtkMatrix4x4 Locator(normalMatrix)
	vtkMatrix4x4 Locator(transverseMatrix)
	vtkMatrix4x4 Locator(tipMatrix)

	set Locator(actors) "normal transverse tip"

	set actor normal 
	MakeVTKObject Cylinder $actor
		${actor}Source SetRadius $Locator(radius) 
		${actor}Source SetHeight $Locator(normalLen)
		eval [${actor}Actor GetProperty] SetColor $Locator(diffuseBlocked)
		${actor}Actor SetUserMatrix Locator(normalMatrix)

	set actor transverse
	MakeVTKObject Cylinder ${actor}
		${actor}Source SetRadius $Locator(radius) 
		${actor}Source SetHeight [expr $Locator(transverseLen)]
		eval [${actor}Actor GetProperty] SetColor $Locator(diffuseBlocked)
		${actor}Actor SetUserMatrix Locator(transverseMatrix)
	
	set actor tip
	MakeVTKObject Sphere ${actor}
		${actor}Source SetRadius [expr 1.5 * $Locator(radius)] 
		eval [${actor}Actor GetProperty] SetColor $Locator(diffuseBlocked)
		${actor}Actor SetUserMatrix Locator(tipMatrix)
	
	LocatorSetMatrices
	LocatorSetVisibility
}


#-------------------------------------------------------------------------------
# .PROC LocatorBuildGUI
# .END
#-------------------------------------------------------------------------------
proc LocatorBuildGUI {} {
	global Gui Module Locator Slice

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Tracking
	# Handpiece
	#   
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Models are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Locator $help
	MainHelpBuildGUI Locator

	#-------------------------------------------
	# Tracking frame
	#-------------------------------------------
	set fTracking $Module(Locator,fTracking)
	set f $fTracking

	# Frames
	frame $f.fConn     -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fPos      -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fDriver   -bg $Gui(activeWorkspace) -relief groove -bd 3 
	frame $f.fVis      -bg $Gui(activeWorkspace) 

	pack $f.fConn $f.fPos $f.fDriver $f.fVis \
		-side top -padx $Gui(pad) -pady $Gui(pad) -fill x

	#-------------------------------------------
	# Tracking->Conn frame
	#-------------------------------------------
	set f $fTracking.fConn

	set c {label $f.l -text "Connection to Server" $Gui(WTA)}; eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

	set f $fTracking.fConn.f
	set c {checkbutton $f.cConnect \
		-text "Connect" -variable Locator(connect) -width 8 \
		-indicatoron 0 -command "LocatorConnect" $Gui(WCA)}
		eval [subst $c]
	set c {checkbutton $f.cPause \
		-text "Pause" -variable Locator(pause) -width 6 \
		-indicatoron 0 $Gui(WCA)}
		eval [subst $c]
	pack $f.cConnect $f.cPause -side left -pady $Gui(pad) -padx $Gui(pad)

	#-------------------------------------------
	# Tracking->Pos Frame
	#-------------------------------------------
	set f $fTracking.fPos

	set c {label $f.l -text "Position & Orientation" $Gui(WTA)}; eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

	set f $fTracking.fPos.f
	set c {label $f.l -text "" $Gui(WLA)}; eval [subst $c]
	foreach ax "x y z" text "R A S" {
		set c {label $f.l$ax -text $text -width 7 $Gui(WLA)}; eval [subst $c]
	}
	grid $f.l $f.lx $f.ly $f.lz -pady 2 -padx $Gui(pad) -sticky e

	foreach axis "N T P" var "n t p" {
		set c {label $f.l$axis -text "$axis:" $Gui(WLA)}; eval [subst $c]
		foreach ax "x y z" text "R A S" {
			set c {entry $f.e$axis$ax -justify right -width 7 \
				-textvariable Locator($var${ax}Str) $Gui(WEA)}; eval [subst $c]
			bind $f.e$axis$ax <Return> "LocatorSetPosition; Render3D"
		}
		grid $f.l$axis $f.e${axis}x $f.e${axis}y $f.e${axis}z \
			-pady $Gui(pad) -padx $Gui(pad) -sticky e
	}

	#-------------------------------------------
	# Tracking->Driver Frame
	#-------------------------------------------
	set f $fTracking.fDriver

	set c {label $f.lTitle -text "The Locator can Drive Slices" $Gui(WTA)}
		eval [subst $c]
	grid $f.lTitle -padx $Gui(pad) -pady 3 -columnspan 2
	
	foreach s $Slice(idList) {

		set c {menubutton $f.mbDriver${s} -text "User" \
			-menu $f.mbDriver${s}.m -width 12 -relief raised \
			-bd 2 $Gui(WMBA) -bg $Gui(slice$s)}; eval [subst $c]
		set Locator(mDriver$s) $f.mbDriver${s}.m
		set Locator(mbDriver$s) $f.mbDriver${s}
		set c {menu $f.mbDriver${s}.m $Gui(WMA)}; eval [subst $c];
		foreach item "User Locator" {
			$Locator(mDriver$s) add command -label $item \
				-command "LocatorSetDriver ${s} $item; RenderBoth $s"
		}
		if {$s == 0} {
			set c {menubutton $f.mbDriver -text "Driver: " \
				-menu $f.mbDriver.m -width 10 -relief raised \
				-bd 2 $Gui(WMBA) -anchor e}; eval [subst $c]

			set Locator(mDriver) $f.mbDriver.m
			set c {menu $Locator(mDriver) $Gui(WMA)}; eval [subst $c];
			foreach item "User Locator" {
				$f.mbDriver.m add command -label $item \
					-command "LocatorSetDriverAll $item; RenderAll"
			}
			grid $f.mbDriver $f.mbDriver${s} \
				-pady $Gui(pad) -padx $Gui(pad)
		} else {
			grid $f.mbDriver${s} -col 1 \
				-pady $Gui(pad) -padx $Gui(pad)
		}

	}

	#-------------------------------------------
	# Tracking->Vis Frame
	#-------------------------------------------
	set f $fTracking.fVis

	set c {checkbutton $f.cLocator \
		-text "Show Locator" -variable Locator(visibility) -width 16 \
		-indicatoron 0 -command "LocatorSetVisibility; Render3D" $Gui(WCA)}
		eval [subst $c]
	set c {checkbutton $f.cTransverse \
		-text "Handle" -variable Locator(transverseVisibility) -width 7 \
		-indicatoron 0 -command "LocatorSetTransverseVisibility; Render3D" $Gui(WCA)}
		eval [subst $c]
	pack $f.cLocator $f.cTransverse -side left -padx $Gui(pad)


	#-------------------------------------------
	# Handpiece frame
	#-------------------------------------------
	set fHandpiece $Module(Locator,fHandpiece)
	set f $fHandpiece

	# Frames
	frame $f.fOffset   -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fSize     -bg $Gui(activeWorkspace) -relief groove -bd 3 
	frame $f.fColor    -bg $Gui(activeWorkspace) -relief groove -bd 3 

	pack $f.fOffset $f.fSize $f.fColor \
		-side top -padx $Gui(pad) -pady $Gui(pad) -fill x

	#-------------------------------------------
	# Handpiece->Offset Frame
	#-------------------------------------------
	set f $fHandpiece.fOffset

	set c {label $f.l -text "Offset from Locator Tip" $Gui(WTA)}
		eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

	set f $fHandpiece.fOffset.f
	foreach axis "N T NxT" text "Normal Transverse {Normal x Transverse}" \
		var "normalOffset transverseOffset crossOffset" {
		set c {label $f.l$axis -text "$text:" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e$axis -textvariable Locator($var) \
			-width 7 $Gui(WEA)}; eval [subst $c]
		bind $f.e$axis <Return> "LocatorSetPosition; Render3D"
		grid $f.l$axis $f.e$axis -pady $Gui(pad) -padx $Gui(pad) -sticky e
		grid $f.e$axis -sticky w 
	}

	#-------------------------------------------
	# Handpiece->Size Frame
	#-------------------------------------------
	set f $fHandpiece.fSize

	set c {label $f.l -text "Size" $Gui(WTA)}
		eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

	set f $fHandpiece.fSize.f
	foreach var "normalLen transverseLen radius" \
		text "{Normal Length (mm)} {Transverse Length} Radius" {
		set c {label $f.l$var -text "$text:" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e$var -textvariable Locator($var) \
			-width 7 $Gui(WEA)}; eval [subst $c]
		bind $f.e$var <Return> "LocatorSetSize; LocatorSetMatrices; Render3D"
		grid $f.l$var $f.e$var -pady $Gui(pad) -padx $Gui(pad) -sticky e
		grid $f.e$var -sticky w
	}

	#-------------------------------------------
	# Handpiece->Color Frame
	#-------------------------------------------
	set f $fHandpiece.fColor

	set c {label $f.l -text "Color" $Gui(WTA)}
		eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

	set f $fHandpiece.fColor.f
	foreach slider "Red Green Blue" {

		set c {label $f.l${slider} -text "${slider}" $Gui(WLA)}
			eval [subst $c]

		set c {entry $f.e${slider} -textvariable Locator([Uncap $slider]) \
			-width 3 $Gui(WEA)}; eval [subst $c]
			bind $f.e${slider} <Return>   "LocatorSetColor; Render3D"
			bind $f.e${slider} <FocusOut> "LocatorSetColor; Render3D"

		set c {scale $f.s${slider} -from 0.0 -to 1.0 -length 100 \
			-variable Locator([Uncap $slider]) \
			-command "LocatorSetColor; Render3D" \
			-resolution 0.1 $Gui(WSA) -sliderlength 20} 
			eval [subst $c]
		set Locator(s$slider) $f.s$slider

		grid $f.l${slider} $f.e${slider} $f.s${slider}  \
			-pady 1 -padx 5 -sticky e
	}

	#-------------------------------------------
	# Server frame
	#-------------------------------------------
	set fServer $Module(Locator,fServer)
	set f $fServer

	# Frames
	frame $f.fGeneral   -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fSpecific  -bg $Gui(activeWorkspace) -relief groove -bd 3

	pack $f.fGeneral $f.fSpecific \
		-side top -padx $Gui(pad) -pady $Gui(pad) -fill x

	#-------------------------------------------
	# Server->General frame
	#-------------------------------------------
	set f $fServer.fGeneral

	set c {label $f.l -text "General" $Gui(WTA)}; eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

	set f $fServer.fGeneral.f
	foreach var "updatePeriodInMs" \
		text "{Update Period (ms)}" {
		set c {label $f.l$var -text "$text:" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e$var -textvariable Locator($var) \
			-width 7 $Gui(WEA)}; eval [subst $c]
		grid $f.l$var $f.e$var -pady $Gui(pad) -padx $Gui(pad) -sticky e
		grid $f.e$var -sticky w
	}
	bind $f.e$var <Return> "LocatorLoop"
	
	#-------------------------------------------
	# Server->Specific frame
	#-------------------------------------------
	set f $fServer.fSpecific

	set c {label $f.l -text "Specific" $Gui(WTA)}; eval [subst $c]
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side top -pady 3 -padx $Gui(pad)

	set f $fServer.fSpecific.f
	foreach var "file" \
		text "{File}" {
		set c {label $f.l$var -text "$text:" $Gui(WLA)}; eval [subst $c]
		set c {entry $f.e$var -textvariable Locator($var) \
			-width 15 $Gui(WEA)}; eval [subst $c]
		grid $f.l$var $f.e$var -pady $Gui(pad) -padx $Gui(pad) -sticky e
		grid $f.e$var -sticky w
	}
	
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetDriverAll
# .END
#-------------------------------------------------------------------------------
proc LocatorSetDriverAll {x} {
	global Slice 

	foreach s $Slice(idList) {
		LocatorSetDriver $s $x
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetDriver
# .END
#-------------------------------------------------------------------------------
proc LocatorSetDriver {s name} {
	global Locator
	
	# Change button text
	$Locator(mbDriver${s}) config -text $name

	# Change variable
	set Locator($s,driver) $name

	if {$name == "User"} {
		Slicer SetDriver $s 0
	} else {
		Slicer SetDriver $s 1
	}

	# Force recomputation of the reformat matrix
	Slicer SetDirectNTP \
		$Locator(nx) $Locator(ny) $Locator(nz) \
		$Locator(tx) $Locator(ty) $Locator(tz) \
		$Locator(px) $Locator(py) $Locator(pz) 
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetVisibility
# .END
#-------------------------------------------------------------------------------
proc LocatorSetVisibility {} {
	global Locator 

	foreach actor $Locator(actors) {
		${actor}Actor SetVisibility $Locator(visibility)
	}
	LocatorSetTransverseVisibility
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetTransverseVisibility
# .END
#-------------------------------------------------------------------------------
proc LocatorSetTransverseVisibility {} {
	global Locator 

	if {$Locator(visibility) == 1} {
		transverseActor SetVisibility $Locator(transverseVisibility)
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetColor
# .END
#-------------------------------------------------------------------------------
proc LocatorSetColor {{value ""}} {
	global Locator

	set color "$Locator(red) $Locator(green) $Locator(blue)"
	foreach actor $Locator(actors) {
		eval [${actor}Actor GetProperty] SetColor $color
	}

	foreach slider "Red Green Blue" {
		$Locator(s$slider) config -troughcolor [MakeColorNormalized $color]
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetSize
# .END
#-------------------------------------------------------------------------------
proc LocatorSetSize {} {
	global Locator

	normalSource SetRadius $Locator(radius) 
	normalSource SetHeight $Locator(normalLen)
	transverseSource SetRadius $Locator(radius) 
	transverseSource SetHeight [expr $Locator(transverseLen)]
	tipSource SetRadius [expr 1.5 * $Locator(radius)] 
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetMatrices
# .END
#-------------------------------------------------------------------------------
proc LocatorSetMatrices {} {
	global Locator

	# Find transform, N, that brings the locator coordinate frame 
	# into the scanner frame.  Then invert N to M and set it to the locator's
	# userMatrix to position the locator within the world space.
	#
	# 1.) Concatenate a translation, T, TO the origin which is (-x,-y,-z)
	#     where the locator's position is (x,y,z).
	# 2.) Concatenate the R matrix.  If the locator's reference frame has
	#     axis Ux, Uy, Uz, then Ux is the TOP ROW of R, Uy is the second, etc.
	# 3.) Translate the cylinder so its tip is at the origin instead
	#     of the center of its tube.  Call this matrix C.
	# Then: N = C*R*T, M = Inv(N)
	#
	# (See page 419 and 429 of "Computer Graphics", Hearn & Baker, 1997,
	#  ISBN 0-13-530924-7)
	# 
	# The alternative approach used here is to find the transform, M, that
	# moves the scanner coordinate frame to the locator's.  
	# 
	# 1.) Translate the cylinder so its tip is at the origin instead
	#     of the center of its tube.  Call this matrix C.
	# 2.) Concatenate the R matrix.  If the locator's reference frame has
	#     axis Ux, Uy, Uz, then Ux is the LEFT COL of R, Uy is the second,etc.
	# 3.) Concatenate a translation, T, FROM the origin which is (x,y,z)
	#     where the locator's position is (x,y,z).
	# Then: M = T*R*C

	vtkMatrix4x4 matrix
	vtkTransform transform

	# Locator's offset: x0, y0, z0
	set x0 $Locator(px)
	set y0 $Locator(py)
	set z0 $Locator(pz)

	# Locator's coordinate axis:
	# Ux = T
	set Ux(x) $Locator(tx)
	set Ux(y) $Locator(ty)
	set Ux(z) $Locator(tz)
	# Uy = -N
	set Uy(x) [expr - $Locator(nx)]
	set Uy(y) [expr - $Locator(ny)]
	set Uy(z) [expr - $Locator(nz)]
	# Uz = Ux x Uy
	set Uz(x) [expr $Ux(y)*$Uy(z) - $Uy(y)*$Ux(z)]
	set Uz(y) [expr $Uy(x)*$Ux(z) - $Ux(x)*$Uy(z)]
	set Uz(z) [expr $Ux(x)*$Uy(y) - $Uy(x)*$Ux(y)]

	# Ux
	matrix SetElement 0 0 $Ux(x)
	matrix SetElement 1 0 $Ux(y)
	matrix SetElement 2 0 $Ux(z)
	matrix SetElement 3 0 0
	# Uy
	matrix SetElement 0 1 $Uy(x)
	matrix SetElement 1 1 $Uy(y)
	matrix SetElement 2 1 $Uy(z)
	matrix SetElement 3 1 0
	# Uz
	matrix SetElement 0 2 $Uz(x)
	matrix SetElement 1 2 $Uz(y)
	matrix SetElement 2 2 $Uz(z)
	matrix SetElement 3 2 0
	# Bottom row
	matrix SetElement 0 3 0
	matrix SetElement 1 3 0
	matrix SetElement 2 3 0
	matrix SetElement 3 3 1

	# Set the vtkTransform to PostMultiply so a concatenated matrix, C,
	# is multiplied by the existing matrix, M: C*M (not M*C)
	transform PostMultiply
	# M = T*R*C

	# TIP PART

	transform Identity
	# T:
	transform Translate [expr $x0] [expr $y0] [expr $z0]
	transform GetMatrix Locator(tipMatrix)
	# VTK BUG (i shouldn't have to call modify for the matrix)?
	Locator(tipMatrix) Modified
	
	# NORMAL PART

	transform Identity
	# C:
	transform Translate 0 [expr $Locator(normalLen) / 2.0] 0
	# R:
	transform Concatenate matrix
	# T:
	transform Translate [expr $x0] [expr $y0] [expr $z0]
	transform GetMatrix Locator(normalMatrix)
	Locator(normalMatrix) Modified

	# TRANSVERSE PART

	transform Identity
	# C: 
	transform RotateZ 90
	transform Translate \
		[expr [expr $Locator(transverseLen) / 2.0] - $Locator(radius)] \
		[expr $Locator(normalLen)] \
		 0 
	# R:
	transform Concatenate matrix
	# T:
	transform Translate [expr $x0] [expr $y0] [expr $z0]
	transform GetMatrix Locator(transverseMatrix)
	Locator(transverseMatrix) Modified

	matrix Delete
	transform Delete
}

#-------------------------------------------------------------------------------
# .PROC LocatorSetPosition
# .END
#-------------------------------------------------------------------------------
proc LocatorSetPosition {{value ""}} {
	global Locator

	# Read matrix
	set Locator(px) $Locator(pxStr) 
	set Locator(py) $Locator(pyStr) 
	set Locator(pz) $Locator(pzStr)
	set Locator(nx) $Locator(nxStr)
	set Locator(ny) $Locator(nyStr)
	set Locator(nz) $Locator(nzStr)
	set Locator(tx) $Locator(txStr)
	set Locator(ty) $Locator(tyStr)
	set Locator(tz) $Locator(tzStr)

	LocatorUseLocatorMatrix
}

#-------------------------------------------------------------------------------
# .PROC LocatorUseLocatorMatrix
# .END
#-------------------------------------------------------------------------------
proc LocatorUseLocatorMatrix {} {
	global Locator Slice

	# Form arrays so we can use vector processing functions
	set P(x) $Locator(px)
	set P(y) $Locator(py)
	set P(z) $Locator(pz)
	set N(x) $Locator(nx)
	set N(y) $Locator(ny)
	set N(z) $Locator(nz)
	set T(x) $Locator(tx)
	set T(y) $Locator(ty)
	set T(z) $Locator(tz)

	# Ensure N, T orthogonal:
	#	C = N x T
	#	T = C x N
	Cross C N T
	Cross T C N

	# Ensure vectors are normalized
	Normalize N
	Normalize T
	Normalize C

	# Offset the Locator
	set n $Locator(normalOffset)
	set t $Locator(transverseOffset)
	set c $Locator(crossOffset)
	set Locator(px) [expr $P(x) + $N(x)*$n + $T(x)*$t + $C(x)*$c]
	set Locator(py) [expr $P(y) + $N(y)*$n + $T(y)*$t + $C(y)*$c]
	set Locator(pz) [expr $P(z) + $N(z)*$n + $T(z)*$t + $C(z)*$c]
	set Locator(nx) $N(x)
	set Locator(ny) $N(y)
	set Locator(nz) $N(z)
	set Locator(tx) $T(x)
	set Locator(ty) $T(y)
	set Locator(tz) $T(z)

	# Format display
	LocatorFormat
				
	# Position the rendered locator
	LocatorSetMatrices
			
	# Find slices with their input set to locator.
	# and set the slice matrix with the new locator data

	Slicer SetDirectNTP \
		$Locator(nx) $Locator(ny) $Locator(nz) \
		$Locator(tx) $Locator(ty) $Locator(tz) \
		$Locator(px) $Locator(py) $Locator(pz) 
}

#-------------------------------------------------------------------------------
# .PROC LocatorFormat
# .END
#-------------------------------------------------------------------------------
proc LocatorFormat {} {
	global Locator

	set Locator(nxStr) [format "%.2f" $Locator(nx)]
	set Locator(nyStr) [format "%.2f" $Locator(ny)]
	set Locator(nzStr) [format "%.2f" $Locator(nz)]
	set Locator(txStr) [format "%.2f" $Locator(tx)]
	set Locator(tyStr) [format "%.2f" $Locator(ty)]
	set Locator(tzStr) [format "%.2f" $Locator(tz)]
	set Locator(pxStr) [format "%.2f" $Locator(px)]
	set Locator(pyStr) [format "%.2f" $Locator(py)]
	set Locator(pzStr) [format "%.2f" $Locator(pz)]
}

#-------------------------------------------------------------------------------
# .PROC LocatorPause
# .END
#-------------------------------------------------------------------------------
proc LocatorPause {{cmd ""}} {
	global Locator

	if {$cmd != ""} { 
		set Locator(pause) $cmd
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorConnect
# .END
#-------------------------------------------------------------------------------
proc LocatorConnect {{value ""}} {
	global Gui  Locator

	if {$value != ""} {
		set Locator(connect) $value
	}

	# CONNECT
	if {$Locator(connect) == "1"} {
		switch $Locator(server) {
		"File" {
			if {[catch {set Locator(fid) [open $Locator(file) r]} errmsg] == 1} {
				puts $errmsg
				tk_messageBox -message $errmsg
				set Locator(loop) 0
				set Locator(connect) 0
				return
			}
			set Locator(loop) 1
			LocatorLoopFile
		}
		}

	# DISCONNECT
	} else {
		switch $Locator(server) {
		"File" {
			if {[catch {close $Locator(fid)} errmsg] == 1} {
				puts $errmsg
			}
		}
		}
		set Locator(loop) 0
	}
}

#-------------------------------------------------------------------------------
# .PROC LocatorLoopFile
# .END
#-------------------------------------------------------------------------------
proc LocatorLoopFile {} {
	global Slice Volume Locator
	
	if {$Locator(loop) == 1} {

		if {$Locator(pause) == 1} {
			update
			after $Locator(updatePeriodInMs) LocatorLoopFile
			return
		}

		# Read matrix
		if {[eof $Locator(fid)] == 1} {
			LocatorConnect 0
			return
		}
		gets $Locator(fid) line
		scan $line "%d %g %g %g %g %g %g %g %g %g" t nx ny nz tx ty tz px py pz

		set Locator(nx) $nx
		set Locator(ny) $ny
		set Locator(nz) $nz
		set Locator(tx) $tx
		set Locator(ty) $ty
		set Locator(tz) $tz
		set Locator(px) $px
		set Locator(py) $py
		set Locator(pz) $pz

		LocatorUseLocatorMatrix

		# Render the slices that the locator is driving
		foreach s $Slice(idList) {
			if {[Slicer GetDriver $s] == 1} {
				RenderSlice $s
			}
		}
		Render3D
		# Call update instead of update idletasks so that we
		# process user input like changing slice orientation
		update
		after $Locator(updatePeriodInMs) LocatorLoopFile
	}
}


