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
# FILE:        Guidance.tcl
# DATE:        12/10/1999 08:40
# LAST EDITOR: gering
# PROCEDURES:  
#   Distance3D
#   GuidanceInit
#   GuidanceBuildVTK
#   GuidanceBuildGUI
#   GuidanceSetFocalPointToTarget
#   GuidanceSetTargetVisibility
#   GuidanceSetActiveTarget
#   GuidanceViewTrajectory
#==========================================================================auto=
# Guidance.tcl

# Register this file as a Module
lappend Module(idList) Guidance

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
# .PROC GuidanceInit
# .END
#-------------------------------------------------------------------------------
proc GuidanceInit {} {
	global Guidance Target Module

	# Define Tabs
	set m Guidance
	set Module($m,row1List) "Help Target"
	set Module($m,row1Name) "Help Target"
	set Module($m,row1,tab) Target

	# Define Procedures
	set Module($m,procGUI) GuidanceBuildGUI
	set Module($m,procVTK) GuidanceBuildVTK

	# Target
	set Target(idList) "0 1"

	set Target(0,name) Red
	set Target(0,diffuseColor) "1 .5 .5"
	set Target(0,x) 0
	set Target(0,y) 0
	set Target(0,z) 0
	set Target(0,visibility) 0 
	set Target(0,radius) 4
	set Target(0,focalPoint) 0

	set Target(1,name) Yellow
	set Target(1,diffuseColor) "1 1 .5"
	set Target(1,x) 0
	set Target(1,y) 0
	set Target(1,z) 0
	set Target(1,visibility) 0 
	set Target(1,radius) 4
	set Target(1,focalPoint) 0

	set Target(active0) 1
	set Target(active1) 0
	set Target(focalPoint) 0
	set Target(visibility) 0
	set Target(xStr) ""
	set Target(yStr) ""
	set Target(zStr) ""

}

#-------------------------------------------------------------------------------
# .PROC GuidanceBuildVTK
# .END
#-------------------------------------------------------------------------------
proc GuidanceBuildVTK {} {
	global Target

	#---------------------#
	# Target
	#---------------------#
	set Target(activeID) [lindex $Target(idList) 0]

	foreach t $Target(idList) {
		MakeVTKObject Sphere target$t
		target${t}Source SetRadius $Target($t,radius)
		eval [target${t}Actor GetProperty] SetColor $Target($t,diffuseColor)
		target${t}Actor SetVisibility $Target($t,visibility)
	}		
}

#-------------------------------------------------------------------------------
# .PROC GuidanceBuildGUI
# .END
#-------------------------------------------------------------------------------
proc GuidanceBuildGUI {} {
	global Gui Target Guidance Module

	set texts "LR PA IS"
	set axi "X Y Z"


	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Target
	#	Active
	#   Vis
	#   Title
	#   Pos
	#   Buttons
	#
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Models are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Guidance $help
	MainHelpBuildGUI Guidance

	#-------------------------------------------
	# Target frame
	#-------------------------------------------
	set fTarget $Module(Guidance,fTarget)
	set f $fTarget

	frame $f.fActive -bg $Gui(activeWorkspace)
	frame $f.fVis    -bg $Gui(activeWorkspace)
	frame $f.fTitle -bg $Gui(activeWorkspace)
	frame $f.fPos   -bg $Gui(activeWorkspace)
	frame $f.fButtons -bg $Gui(activeWorkspace)
	pack $f.fTitle $f.fActive $f.fVis $f.fPos $f.fButtons \
		-side top -padx $Gui(pad) -pady $Gui(pad)

	#-------------------------------------------
	# Target->Title Frame
	#-------------------------------------------
	set f $fTarget.fTitle

	set c {label $f.lTitle -text "Target Position" $Gui(WTA)}; eval [subst $c]
	pack $f.lTitle

	#-------------------------------------------
	# Target->Active frame
	#-------------------------------------------
	set f $fTarget.fActive

	# Active Target
	set c {label $f.lActive -text "Target:" $Gui(WLA)}; eval [subst $c]
	set c {label $f.lBlank -text " " $Gui(WLA)}; eval [subst $c]
	frame $f.fActive -bg $Gui(activeWorkspace)
	foreach mode "0 1" name "Red Yellow" {
		set c {checkbutton $f.fActive.c$mode \
			-text "$name" -variable Target(active$mode) -width 7 \
			-indicatoron 0 -command "GuidanceSetActiveTarget $mode; Render3D"\
			 $Gui(WCA)}
			eval [subst $c]
		pack $f.fActive.c$mode -side left -padx 0
	}
	pack $f.lActive $f.lBlank $f.fActive -side left -padx $Gui(pad)

	#-------------------------------------------
	# Target->Vis frame
	#-------------------------------------------
	set f $fTarget.fVis

	# Visibility
	set c {checkbutton $f.cTarget \
		-text "Show Target" -variable Target(visibility) -width 18 \
		-indicatoron 0 -command "GuidanceSetTargetVisibility; Render3D" \
		 $Gui(WCA)}
		eval [subst $c]            
	pack $f.cTarget
	
	#-------------------------------------------
	# Target->Pos frame
	#-------------------------------------------
	set f $fTarget.fPos

	# Position Sliders
	foreach slider $axi text $texts {
		set c {label $f.l${slider} -text "$text:" $Gui(WLA)}
			eval [subst $c]

		set c {entry $f.e${slider} \
			-textvariable Target([Uncap ${slider}]Str) \
			-width 7 $Gui(WEA)}; eval [subst $c]
			bind $f.e${slider} <Return> \
				"GuidanceSetTargetPosition $slider; Render3D"
			bind $f.e${slider} <FocusOut> \
				"GuidanceSetTargetPosition $slider; Render3D"

		set c {scale $f.s${slider} -from -180 -to 180 -length 120 \
			-variable Target([Uncap ${slider}]Str) \
			-command "GuidanceSetTargetPosition $slider; Render3D" \
			-resolution 1 $Gui(WSA)} 
			eval [subst $c]
	}

	# Grid
	grid $f.lX $f.eX $f.sX  -padx $Gui(pad) -pady $Gui(pad)
	grid $f.lX -sticky e
	grid $f.lY $f.eY $f.sY -padx $Gui(pad) -pady $Gui(pad)
	grid $f.lY -sticky e
	grid $f.lZ $f.eZ $f.sZ  -padx $Gui(pad) -pady $Gui(pad)
	grid $f.lZ -sticky e

	#-------------------------------------------
	# Target->Buttons frame
	#-------------------------------------------
	set f $fTarget.fButtons

	set c {button $f.bFocus -text "Use as Focal Point" -width 18 \
		-command "GuidanceSetFocalPointToTarget; RenderAll" $Gui(WBA)}
		eval [subst $c]
	set c {button $f.cTrajectory -text "View Trajectory" -width 18 \
		-command "GuidanceViewTrajectory; RenderAll" $Gui(WBA)}
		eval [subst $c]

	pack $f.bFocus $f.cTrajectory \
		-side top -pady $Gui(pad) -padx 0

}

#-------------------------------------------------------------------------------
# GuidanceSetTargetPosition
#
# 'value' comes from the sliders, but is unused here.
#-------------------------------------------------------------------------------
proc GuidanceSetTargetPosition {{value ""}} {
	global Target InitProc

	# Load GUI faster
	if {![info exists InitProc(GuidanceSetTargetPosition)]} {
		set InitProc(GuidanceSetTargetPosition) 1
		return
	}

	set t $Target(activeID)
	set Target($t,x) $Target(xStr)
	set Target($t,y) $Target(yStr)
	set Target($t,z) $Target(zStr)

	target${t}Actor SetPosition $Target($t,x) $Target($t,y) $Target($t,z)
}

#-------------------------------------------------------------------------------
# .PROC GuidanceSetFocalPointToTarget
# .END
#-------------------------------------------------------------------------------
proc GuidanceSetFocalPointToTarget {} {
	global Slice View Target

	set t $Target(activeID)
	MainViewSetFocalPoint $Target($t,x) $Target($t,y) $Target($t,z)
}

#-------------------------------------------------------------------------------
# .PROC GuidanceSetTargetVisibility
# .END
#-------------------------------------------------------------------------------
proc GuidanceSetTargetVisibility {} {
	global Target

	set t $Target(activeID)
	set Target($t,visibility) $Target(visibility)

	target${t}Actor SetVisibility $Target($t,visibility)
}

#-------------------------------------------------------------------------------
# .PROC GuidanceSetActiveTarget
# .END
#-------------------------------------------------------------------------------
proc GuidanceSetActiveTarget {{t ""}} {
	global Target

	if {$t == ""} {
		set t $Target(activeID)
	} else {
		set Target(activeID) $t
	}

   # Change button status
 	set Target(active0) 0
	set Target(active1) 0
	set Target(active$t) 1

	foreach param "visibility focalPoint" {
		set Target($param) $Target($t,$param)
	}
	foreach param "x y z" {
		set Target(${param}Str) [format "%.2f" $Target($t,$param)]
	}
}

#-------------------------------------------------------------------------------
# .PROC GuidanceViewTrajectory
# .END
#-------------------------------------------------------------------------------
proc GuidanceViewTrajectory {} {
	global Target View Slice

	set t1 [lindex $Target(idList) 0]
	set t2 [lindex $Target(idList) 1]

	set dx [expr $Target($t2,x) - $Target($t1,x)]
	set dy [expr $Target($t2,y) - $Target($t1,y)]
	set dz [expr $Target($t2,z) - $Target($t1,z)]
	set d [expr sqrt($dx*$dx + $dy*$dy + $dz*$dz)]
	if {$d == 0} {
		tk_messageBox -message "The targets should be in different locations."
		return
	}

	set fpx [expr ($Target($t1,x) + $Target($t2,x))/2.0]
	set fpy [expr ($Target($t1,y) + $Target($t2,y))/2.0]
	set fpz [expr ($Target($t1,z) + $Target($t2,z))/2.0]
	scan [$View(viewCam) GetPosition] "%f %f %f" vpx vpy vpz
	set a [Distance3D $fpx $fpy $fpz $vpx $vpy $vpz]

	set vpx [expr $fpx + 1.0*$dx*$a/$d]	
	set vpy [expr $fpy + 1.0*$dy*$a/$d]	
	set vpz [expr $fpz + 1.0*$dz*$a/$d]	

	$View(viewCam) SetFocalPoint $fpx $fpy $fpz
	$View(viewCam) SetPosition   $vpx $vpy $vpz
	$View(viewCam) SetViewUp 0 1 0
	eval $View(viewCam) SetClippingRange $View(baselineClippingRange)
	$View(viewCam) ComputeViewPlaneNormal
	$View(viewCam) OrthogonalizeViewUp

	MainViewLightFollowCamera

	set Slice(visibilityAll) 1
	MainSlicesSetVisibilityAll
	MainSlicesSetOrientAll Orthogonal
}


