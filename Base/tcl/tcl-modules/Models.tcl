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
# FILE:        Models.tcl
# DATE:        01/18/2000 12:17
# LAST EDITOR: gering
# PROCEDURES:  
#   ModelsInit
#   ModelsUpdateMRML
#   ModelsBuildGUI
#   ModelsSetPropertyType
#   ModelsPropsApply
#   ModelsPropsCancel
#   ModelsMeter
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC ModelsInit
# .END
#-------------------------------------------------------------------------------
proc ModelsInit {} {
	global Model Module

	# Define Tabs
	set m Models
	set Module($m,row1List) "Help Display Props Clip Meter"
	set Module($m,row1Name) "{Help} {Display} {Props} {Clip} {Meter}"
	set Module($m,row1,tab) Display
# Use these lines to add a second row of tabs
#	set Module($m,row2List) "Meter"
#	set Module($m,row2Name) "{Meter}"
#	set Module($m,row2,tab) Meter

	# Define Procedures
	set Module($m,procGUI) ModelsBuildGUI
	set Module($m,procMRML) ModelsUpdateMRML

	# Props
	set Model(propertyType) Basic
	set Model(name) ""
	# also Model(prefix)
	set Model(culling) 1
	set Model(scalarVisibility) 0
	set Model(scalarLo) 0
	set Model(scalarHi) 100
	set Model(desc) ""

	# Meter
	set Model(meter,first) 1
}

#-------------------------------------------------------------------------------
# .PROC ModelsUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc ModelsUpdateMRML {} {
	global Gui Model Slice Module Color Volume Label

	# Create the GUI for any new models
	foreach m $Model(idList) {
		MainModelsCreateGUI $Module(Models,fDisplay).fGrid $m
	}

	# Delete the GUI for any old models
	foreach m $Model(idListDelete) {
		MainModelsDeleteGUI $Module(Models,fDisplay).fGrid $m
	}

	# Refresh the GUI for all models (in case color changed)
	foreach m $Model(idList) {
		set c $Model($m,colorID)
		$Module(Models,fDisplay).fGrid.s$m config \
			-troughcolor [MakeColorNormalized [Color($c,node) GetDiffuseColor]]
	}
}

#-------------------------------------------------------------------------------
# .PROC ModelsBuildGUI
# .END
#-------------------------------------------------------------------------------
proc ModelsBuildGUI {} {
	global Gui Model Slice Module Label

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Display
	#   Title
	#   All
	#   Grid
	# Props
	#   Top
	#     Active
	#     Type
	#   Bot
	#     Basic
	#     Advanced
	# Clip
	#   Help
	#   Grid
	# Meter
	#   
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
Models are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Models $help
	MainHelpBuildGUI Models

	#-------------------------------------------
	# Display frame
	#-------------------------------------------
	set fDisplay $Module(Models,fDisplay)
	set f $fDisplay

	frame $f.fTitle -bg $Gui(activeWorkspace)
	frame $f.fAll -bg $Gui(activeWorkspace)
	frame $f.fGrid -bg $Gui(activeWorkspace)
	pack $f.fTitle $f.fAll $f.fGrid -side top -pady $Gui(pad)

	#-------------------------------------------
	# fDisplay->Title frame
	#-------------------------------------------
	set f $fDisplay.fTitle

	eval {label $f.lTitle -justify left -text \
		"Click the right mouse button on\nthe name of a model for options."} $Gui(WLA)
	pack $f.lTitle

	#-------------------------------------------
	# fDisplay->All frame
	#-------------------------------------------
	set f $fDisplay.fAll

	eval {button $f.bAll -text "Show All" -width 10 \
		-command "MainModelsSetVisibility All; Render3D"} $Gui(WBA)
	eval {button $f.bNone -text "Show None" -width 10 \
		-command "MainModelsSetVisibility None; Render3D"} $Gui(WBA)
	pack $f.bAll $f.bNone -side left -padx $Gui(pad) -pady 0

	#-------------------------------------------
	# fDisplay->Grid frame
	#-------------------------------------------
	# Done in MainModelsRefreshGUI

	#-------------------------------------------
	# Props frame
	#-------------------------------------------
	set fProps $Module(Models,fProps)
	set f $fProps

	frame $f.fTop -bg $Gui(backdrop) -relief sunken -bd 2
	frame $f.fBot -bg $Gui(activeWorkspace) -height 300
	pack $f.fTop $f.fBot -side top -pady $Gui(pad) -padx $Gui(pad) -fill x

	#-------------------------------------------
	# Props->Bot frame
	#-------------------------------------------
	set f $fProps.fBot

	foreach type "Basic Advanced" {
		frame $f.f${type} -bg $Gui(activeWorkspace)
		place $f.f${type} -in $f -relheight 1.0 -relwidth 1.0
		set Model(f${type}) $f.f${type}
	}
	raise $Model(fBasic)

	#-------------------------------------------
	# Props->Top frame
	#-------------------------------------------
	set f $fProps.fTop

	frame $f.fActive -bg $Gui(backdrop)
	frame $f.fType   -bg $Gui(backdrop)
	pack $f.fActive $f.fType -side top -fill x -pady $Gui(pad) -padx $Gui(pad)

	#-------------------------------------------
	# Props->Top->Active frame
	#-------------------------------------------
	set f $fProps.fTop.fActive

	eval {label $f.lActive -text "Active Model: "} $Gui(BLA)
	eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m} $Gui(WMBA)
	eval {menu $f.mbActive.m} $Gui(WMA)
	pack $f.lActive $f.mbActive -side left

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Model(mbActiveList) $f.mbActive
	lappend Model(mActiveList)  $f.mbActive.m

	#-------------------------------------------
	# Props->Top->Type frame
	#-------------------------------------------
	set f $fProps.fTop.fType

	eval {label $f.l -text "Properties:"} $Gui(BLA)
	frame $f.f -bg $Gui(backdrop)
	foreach p "Basic Advanced" {
		eval {radiobutton $f.f.r$p \
			-text "$p" -command "ModelsSetPropertyType" \
			-variable Model(propertyType) -value $p -width 8 \
			-indicatoron 0} $Gui(WCA)
		pack $f.f.r$p -side left -padx 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

	#-------------------------------------------
	# Props->Bot->Basic frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic

	frame $f.fPrefix  -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fName    -bg $Gui(activeWorkspace)
	frame $f.fColor   -bg $Gui(activeWorkspace)
	frame $f.fApply   -bg $Gui(activeWorkspace)
	pack $f.fPrefix $f.fName $f.fColor $f.fApply \
		-side top -fill x -pady $Gui(pad)

	#-------------------------------------------
	# Props->Bot->Advanced frame
	#-------------------------------------------
	set f $fProps.fBot.fAdvanced

	frame $f.fCulling -bg $Gui(activeWorkspace)
	frame $f.fScalars -bg $Gui(activeWorkspace) -relief groove -bd 3
	frame $f.fDesc    -bg $Gui(activeWorkspace)
	frame $f.fApply   -bg $Gui(activeWorkspace)
	pack $f.fCulling $f.fScalars $f.fDesc $f.fApply \
		-side top -fill x -pady $Gui(pad)

	#-------------------------------------------
	# Props->Bot->Basic->Name frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic.fName

	eval {label $f.l -text "Name:" } $Gui(WLA)
	eval {entry $f.e -textvariable Model(name)} $Gui(WEA)
	pack $f.l -side left -padx $Gui(pad)
	pack $f.e -side left -padx $Gui(pad) -expand 1 -fill x

	#-------------------------------------------
	# Props->Bot->Basic->Prefix frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic.fPrefix

	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.f -side top -pady $Gui(pad)

	eval {label $f.f.l -text "File Prefix (without .vtk)"} $Gui(WLA)
	eval {button $f.f.b -text "Browse..." -width 10 \
		-command "ModelsSetPrefix"} $Gui(WBA)
	pack $f.f.l $f.f.b -side left -padx $Gui(pad)

	eval {entry $f.eFile -textvariable Model(prefix) -width 50} $Gui(WEA)
	bind $f.eFile <Return> {ModelsSetPrefix}
	pack $f.eFile -side top -pady $Gui(pad) -padx $Gui(pad) -expand 1 -fill x

	#-------------------------------------------
	# Props->Bot->Basic->Color frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic.fColor

	eval {button $f.b -text "Color:" -command "ShowColors"} $Gui(WBA)
	eval {entry $f.e -width 20 \
		-textvariable Label(name)} $Gui(WEA) \
		{-bg $Gui(activeWorkspace) -state disabled}
	pack $f.b $f.e -side left -padx $Gui(pad) -pady $Gui(pad) -fill x

	lappend Label(colorWidgetList) $f.e

	#-------------------------------------------
	# Props->Bot->Basic->Apply frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic.fApply

	eval {button $f.bApply -text "Apply" \
		-command "ModelsPropsApply; RenderAll"} $Gui(WBA) {-width 8}
	eval {button $f.bCancel -text "Cancel" \
		-command "ModelsPropsCancel"} $Gui(WBA) {-width 8}
	grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)

	#-------------------------------------------
	# Props->Bot->Advanced->Culling frame
	#-------------------------------------------
	set f $fProps.fBot.fAdvanced.fCulling

	eval {label $f.l -text "Backface Culling:"} $Gui(WLA)
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side left -padx $Gui(pad)

	foreach text "{Yes} {No}" \
		value "1 0" \
		width "3 3" {
		eval {radiobutton $f.f.rMode$value -width $width \
			-text "$text" -value "$value" -variable Model(culling) \
			-indicatoron 0} $Gui(WCA)
		pack $f.f.rMode$value -side left -padx 0 -pady 0
	}

	#-------------------------------------------
	# Props->Bot->Advanced->Scalars frame
	#-------------------------------------------
	set f $fProps.fBot.fAdvanced.fScalars

	frame $f.fVisible -bg $Gui(activeWorkspace)
	frame $f.fRange   -bg $Gui(activeWorkspace)
	pack $f.fVisible $f.fRange -side top -pady $Gui(pad)
	set fVisible $f.fVisible
	set fRange $f.fRange

	# fVisible
	set f $fVisible

	eval {label $f.l -text "Scalars Visible:"} $Gui(WLA)
	frame $f.f -bg $Gui(activeWorkspace)
	pack $f.l $f.f -side left -padx $Gui(pad) -pady 0

	foreach text "{Yes} {No}" \
		value "1 0" \
		width "4 4" {
		eval {radiobutton $f.f.rMode$value -width $width \
			-text "$text" -value "$value" -variable Model(scalarVisibility) \
			-indicatoron 0} $Gui(WCA)
		pack $f.f.rMode$value -side left
	}

	# fRange
	set f $fRange

	eval {label $f.l -text "  Scalar Range:"} $Gui(WLA)
	eval {entry $f.eLo -width 6 -textvariable Model(scalarLo)} $Gui(WEA)
	eval {entry $f.eHi -width 6 -textvariable Model(scalarHi)} $Gui(WEA)
	pack $f.l $f.eLo $f.eHi -side left -padx $Gui(pad)

	#-------------------------------------------
	# Props->Bot->Advanced->Desc frame
	#-------------------------------------------
	set f $fProps.fBot.fAdvanced.fDesc

	eval {label $f.l -text "Optional Description:"} $Gui(WLA)
	eval {entry $f.e -textvariable Model(desc)} $Gui(WEA)
	pack $f.l -side top -padx $Gui(pad) -fill x -anchor w
	pack $f.e -side top -padx $Gui(pad) -expand 1 -fill x

	#-------------------------------------------
	# Props->Bot->Advanced->Apply frame
	#-------------------------------------------
	set f $fProps.fBot.fAdvanced.fApply

	eval {button $f.bApply -text "Apply" \
		-command "ModelsPropsApply; RenderAll"} $Gui(WBA) {-width 8}
	eval {button $f.bCancel -text "Cancel" \
		-command "ModelsPropsCancel"} $Gui(WBA) {-width 8}
	grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)


	#-------------------------------------------
	# Clip frame
	#-------------------------------------------
	set fClip $Module(Models,fClip)
	set f $fClip

	frame $f.fHelp -bg $Gui(activeWorkspace)
	frame $f.fGrid -bg $Gui(activeWorkspace)
	pack $f.fHelp $f.fGrid -side top -pady $Gui(pad)

	#-------------------------------------------
	# fClip->Grid frame
	#-------------------------------------------
	set f $fClip.fHelp

	eval {label $f.l  -justify left -text "The slices clip the models that\n\
		have clipping turn on.\n\n\
		To turn clipping on for a model,\n\
		check the box to the right of the\n\
		model's name on the Display page."} $Gui(WLA)
	pack $f.l

	#-------------------------------------------
	# fClip->Grid frame
	#-------------------------------------------
	set f $fClip.fGrid
	
	foreach s $Slice(idList) name "Red Yellow Green" {

		eval {label $f.l$s -text "$name Slice: "} $Gui(WLA)
		
		frame $f.f$s -bg $Gui(activeWorkspace)
		foreach text "Off + -" value "0 1 2" width "4 2 2" {
			eval {radiobutton $f.f$s.r$value -width $width \
				-text "$text" -value "$value" -variable Slice($s,clipState) \
				-indicatoron 0 \
				-command "MainSlicesSetClipState $s; MainModelsRefreshClip; Render3D" \
				} $Gui(WCA) {-bg $Gui(slice$s)}
			pack $f.f$s.r$value -side left -padx 0 -pady 0
		}
		grid $f.l$s $f.f$s -pady $Gui(pad)
	}


	#-------------------------------------------
	# Meter frame
	#-------------------------------------------
	set fMeter $Module(Models,fMeter)
	set f $fMeter

	foreach frm "Apply Results" {
		frame $f.f$frm -bg $Gui(activeWorkspace)
		pack  $f.f$frm -side top -pady $Gui(pad)
	}

	#-------------------------------------------
	# Meter->Apply frame
	#-------------------------------------------
	set f $fMeter.fApply

	set text "Measure Performance"
	eval {button $f.bMeasure -text "$text" \
		-width [expr [string length $text] + 1] \
		-command "ModelsMeter"} $Gui(WBA)
	pack $f.bMeasure

	#-------------------------------------------
	# Meter->Results frame
	#-------------------------------------------
	set f $fMeter.fResults

	frame $f.fTop -bg $Gui(activeWorkspace)
	frame $f.fBot -bg $Gui(activeWorkspace)
	pack $f.fTop $f.fBot -side top -pady $Gui(pad)

	set f $fMeter.fResults.fTop
	eval {label $f.l -justify left -text ""} $Gui(WLA)
	pack $f.l
	set Model(meter,msgTop) $f.l

	set f $fMeter.fResults.fBot
	eval {label $f.lL -justify left -text ""} $Gui(WLA)
	eval {label $f.lR -justify right -text ""} $Gui(WLA)
	pack $f.lL $f.lR -side left -padx $Gui(pad)
	set Model(meter,msgLeft) $f.lL
	set Model(meter,msgRight) $f.lR

}

#-------------------------------------------------------------------------------
# .PROC ModelsSetPropertyType
# .END
#-------------------------------------------------------------------------------
proc ModelsSetPropertyType {} {
	global Model
	
	raise $Model(f$Model(propertyType))
}

proc ModelsSetPrefix {} {
	global Model Mrml Color

	# Cannot have blank prefix
	if {$Model(prefix) == ""} {
		set Model(prefix) model
	}

 	# Show popup initialized to the last file saved
	set filename [file join $Mrml(dir) $Model(prefix)]
	set dir [file dirname $filename]
	set typelist {
		{"VTK Files" {.vtk}}
		{"All Files" {*}}
	}
	set filename [tk_getOpenFile -title "Open Model" -defaultextension ".vtk"\
		-filetypes $typelist -initialdir "$dir" -initialfile $filename]

	# Do nothing if the user cancelled
	if {$filename == ""} {return}

	# Store it as a relative prefix for next time
	set Model(prefix) [MainFileGetRelativePrefix $filename]

	# Guess the name based on the prefix
	set Model(name) [file tail $Model(prefix)]

	# Guess the color
	set name [string tolower $Model(name)]
	set guess [Color($Color(activeID),node) GetName]
	foreach c $Color(idList) {
		set n [string tolower [Color($c,node) GetName]]
		if {[string first $name $n] != -1} {
			set guess [Color($c,node) GetName]
		}
	}
	LabelsSetColor $guess
}

#-------------------------------------------------------------------------------
# .PROC ModelsPropsApply
# .END
#-------------------------------------------------------------------------------
proc ModelsPropsApply {} {
	global Model Label Module Mrml

	set m $Model(activeID)
	if {$m == ""} {return}

	if {$m == "NEW"} {
		# Ensure prefix not blank
		if {$Model(prefix) == ""} {
			tk_messageBox -message "Please enter a file prefix."
			return
		}
		set i $Model(nextID)
		incr Model(nextID)
		lappend Model(idList) $i
		vtkMrmlModelNode Model($i,node)
		set n Model($i,node)
		$n SetID               $i
		$n SetOpacity          1.0
		$n SetVisibility       1
		$n SetClipping         0

		# These get set down below, but we need them before MainUpdateMRML
		$n SetName $Model(name)
		$n SetFileName "$Model(prefix).vtk"
		$n SetFullFileName [file join $Mrml(dir) [$n GetFileName]]
		$n SetColor $Label(name)

		Mrml(dataTree) AddItem $n
		MainUpdateMRML

		# If failed, then it's no longer in the idList
		if {[lsearch $Model(idList) $i] == -1} {
			return
		}
		set Model(freeze) 0
		MainModelsSetActive $i
		set m $i
	}

	Model($m,node) SetName $Model(name)
	Model($m,node) SetFileName "$Model(prefix).vtk"
	Model($m,node) SetFullFileName [file join $Mrml(dir) [Model($m,node) GetFileName]]
	Model($m,node) SetDescription $Model(desc)
	MainModelsSetCulling $m $Model(culling)
	MainModelsSetScalarVisibility $m $Model(scalarVisibility)
	MainModelsSetScalarRange $m $Model(scalarLo) $Model(scalarHi)
	MainModelsSetColor $m $Label(name)

	# If tabs are frozen, then return to the "freezer"
	if {$Module(freezer) != ""} {
		set cmd "Tab $Module(freezer)"
		set Module(freezer) ""
		eval $cmd
	}
	
	MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC ModelsPropsCancel
# .END
#-------------------------------------------------------------------------------
proc ModelsPropsCancel {} {
	global Model Module

	# Reset props
	set m $Model(activeID)
	if {$m == "NEW"} {
		set m [lindex $Model(idList) 0]
	}
	set Model(freeze) 0
	MainModelsSetActive $m

	# Unfreeze
	if {$Module(freezer) != ""} {
		set cmd "Tab $Module(freezer)"
		set Module(freezer) ""
		eval $cmd
	}
}

#-------------------------------------------------------------------------------
# .PROC ModelsMeter
# .END
#-------------------------------------------------------------------------------
proc ModelsMeter {} {
	global Model

	# Count the polygons in each model
	set total 0
	set msgLeft ""
	set msgRight ""
	foreach m $Model(idList) {

		if {[info exists Model($m,nPolys)] == 0} {

			vtkTriangleFilter triangle
			triangle SetInput $Model($m,polyData)
			[triangle GetOutput] ReleaseDataFlagOn
			triangle Update
			set Model($m,nPolys) [[triangle GetOutput] GetNumberOfPolys]

			vtkStripper stripper
			stripper SetInput [triangle GetOutput]
			[stripper GetOutput] ReleaseDataFlagOff

			# polyData will survive as long as it's the input to the mapper
			set Model($m,polyData) [stripper GetOutput]
			$Model($m,polyData) Update
			Model($m,mapper) SetInput $Model($m,polyData)

			stripper SetOutput ""
			foreach p "triangle stripper" {
				$p SetInput ""
				$p Delete
			}
		}
#		puts "m=$m: ref=[$Model($m,polyData) GetReferenceCount]"

		set n $Model($m,nPolys)
		if {[Model($m,node) GetVisibility] == 1} {
			set total [expr $total + $n]
		}
		set msgLeft "$msgLeft\n[Model($m,node) GetName]"
		set msgRight "$msgRight\n$n"
	}


	# Compute rate
	set t [lindex [time {Render3D}] 0]
	if {$t > 0} {
		set rate [expr $total / ($t/1000000.0)]
	} else {
		set rate 0
	}

	set msgTop "\
Total visible polygons: $total\n\
Render time: [format "%.3f" [expr $t/1000000.0]]\n\
Polygons/sec rendered: [format "%.0f" $rate]"

	$Model(meter,msgTop) config -text $msgTop
	$Model(meter,msgLeft) config -text $msgLeft
	$Model(meter,msgRight) config -text $msgRight

	if {$Model(meter,first) == 1} {
		set Model(meter,first) 0
		ModelsMeter
	}
}
