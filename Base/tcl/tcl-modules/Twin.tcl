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
# FILE:        Twin.tcl
# DATE:        02/16/2000 09:13
# LAST EDITOR: gering
# PROCEDURES:  
#   TwinInit
#   TwinBuildGUI
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC TwinInit
# .END
#-------------------------------------------------------------------------------
proc TwinInit {} {
	global Twin Module

	# Define Tabs
	set m Twin
	set Module($m,row1List) "Help Twin"
	set Module($m,row1Name) "Help Twin"
	set Module($m,row1,tab) Twin

	# Define Procedures
	set Module($m,procGUI) TwinBuildGUI
	set Module($m,procVTK) TwinBuildVTK

	set Twin(mode) Off
	set Twin(xPos) 0
	set Twin(yPos) 0
	set Twin(width) 400
	set Twin(height) 300
	set Twin(screen) 0
}

proc TwinBuildVTK {} {
	global Twin viewWin twinWin

	vtkXDisplayWindow Twin(display)

	vtkImager twinRen

	vtkImageFrameSource Twin(src)
	Twin(src) SetExtent 0 [expr $Twin(width)-1] 0 [expr $Twin(height)-1]
	Twin(src) SetRenderWindow $viewWin
		
	vtkImageMapper Twin(mapper)
	Twin(mapper) SetColorWindow 255
	Twin(mapper) SetColorLevel 127.5
	Twin(mapper) SetInput [Twin(src) GetOutput]
	
	vtkActor2D Twin(actor)
	Twin(actor) SetMapper Twin(mapper)
	twinRen AddActor2D Twin(actor)
}

#-------------------------------------------------------------------------------
# .PROC TwinBuildGUI
# .END
#-------------------------------------------------------------------------------
proc TwinBuildGUI {} {
	global Gui Twin Module

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Twin
	#   Grid
	#   Mode 
	#
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Models are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Twin $help
	MainHelpBuildGUI Twin

	#-------------------------------------------
	# Twin frame
	#-------------------------------------------
	set fTwin $Module(Twin,fTwin)
	set f $fTwin

	frame $f.fGrid -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fMode -bg $Gui(activeWorkspace)
	pack $f.fMode $f.fGrid \
		-side top -pady $Gui(pad) -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Twin->Mode Frame
	#-------------------------------------------
	set f $fTwin.fMode
	
    set c {label $f.l -text "Mode: " $Gui(WLA)}
		eval [subst $c]
	pack $f.l -side left -padx $Gui(pad) -pady 0

	foreach value "On Pause Off" width "3 6 4" {
		set c {radiobutton $f.r$value -width $width \
			-text "$value" -value "$value" -variable Twin(mode) \
			-indicatoron 0 -command "TwinApply" $Gui(WCA)}
			eval [subst $c]
		pack $f.r$value -side left -padx 0 -pady 0
	}
		
	#-------------------------------------------
	# Twin->Grid Frame
	#-------------------------------------------
	set f $fTwin.fGrid
	
	# Entry fields (the loop makes a frame for each variable)
	foreach param "xPos yPos width height screen" \
		name "{X Position} {Y Position} {Width} {Height} {Screen Number}" {

	    set c {label $f.l$param -text "$name:" $Gui(WLA)}; eval [subst $c]
	    set c {entry $f.e$param -width 5 -textvariable Twin($param) $Gui(WEA)}
	    eval [subst $c]

		grid $f.l$param $f.e$param -padx $Gui(pad) -pady $Gui(pad) -sticky e
		grid $f.e$param -sticky w
	}

	set c {button $f.b -text "Apply" -command "TwinApply" $Gui(WBA)}; eval [subst $c]
	grid $f.b -columnspan 2 -padx $Gui(pad) -pady $Gui(pad) 
}

proc TwinApply {} {
	global Twin twinWin

	if {$Twin(mode) == "On"} {
		# If window does not exist, create it
		if {[info exists twinWin] == 0 || [info command $twinWin] == ""} {
			set twinWin [Twin(display) GetImageWindow $Twin(screen)]
			$twinWin AddImager twinRen
		}
		$twinWin SetPosition $Twin(xPos) $Twin(yPos)
		$twinWin SetSize $Twin(width) $Twin(height)
		Twin(src) SetExtent 0 [expr $Twin(width)-1] 0 [expr $Twin(height)-1]
		Render3D

	} elseif {$Twin(mode) == "Off"} {
		# If window exists, delete it
		if {[info exists twinWin] == 1 && [info command $twinWin] != ""} {
			$twinWin Delete
		}
	}
}
