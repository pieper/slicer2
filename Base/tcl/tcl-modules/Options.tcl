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
# FILE:        Options.tcl
# DATE:        01/20/2000 09:41
# LAST EDITOR: gering
# PROCEDURES:  
#   OptionsInit
#   OptionsUpdateMRML
#   OptionsBuildGUI
#   OptionsSetPropertyType
#   OptionsSetPrefix
#   OptionsPropsApply
#   OptionsPropsCancel
#   OptionsMeter
#==========================================================================auto=

#-------------------------------------------------------------------------------
# .PROC OptionsInit
# .END
#-------------------------------------------------------------------------------
proc OptionsInit {} {
	global Options Module

	# Define Tabs
	set m Options
	set Module($m,row1List) "Help Props"
	set Module($m,row1Name) "{Help} {Props}"
	set Module($m,row1,tab) Props

	# Define Procedures
	set Module($m,procGUI)  OptionsBuildGUI
	set Module($m,procMRML) OptionsUpdateMRML

	# Define Dependencies
	set Module($m,depend) ""

	# Props
	set Options(propertyType) Basic
	set Options(program) "slicer"
	set Options(contents) "presets"
#	set Options(options) ""
}

#-------------------------------------------------------------------------------
# .PROC OptionsUpdateMRML
# .END
#-------------------------------------------------------------------------------
proc OptionsUpdateMRML {} {

}

#-------------------------------------------------------------------------------
# .PROC OptionsBuildGUI
# .END
#-------------------------------------------------------------------------------
proc OptionsBuildGUI {} {
	global Gui Options Module

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
Options are fun. Do you like models, Ron?
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Options $help
	MainHelpBuildGUI Options


	#-------------------------------------------
	# Props frame
	#-------------------------------------------
	set fProps $Module(Options,fProps)
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
		set Options(f${type}) $f.f${type}
	}
	raise $Options(fBasic)

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

	eval {label $f.lActive -text "Active Option: "} $Gui(BLA)
	eval {menubutton $f.mbActive -text "None" -relief raised -bd 2 -width 20 \
		-menu $f.mbActive.m} $Gui(WMBA)
	eval {menu $f.mbActive.m} $Gui(WMA)
	pack $f.lActive $f.mbActive -side left

	# Append widgets to list that gets refreshed during UpdateMRML
	lappend Options(mbActiveList) $f.mbActive
	lappend Options(mActiveList)  $f.mbActive.m

	#-------------------------------------------
	# Props->Top->Type frame
	#-------------------------------------------
	set f $fProps.fTop.fType

	eval {label $f.l -text "Properties:"} $Gui(BLA)
	frame $f.f -bg $Gui(backdrop)
	foreach p "Basic Advanced" {
		eval {radiobutton $f.f.r$p \
			-text "$p" -command "OptionsSetPropertyType" \
			-variable Options(propertyType) -value $p -width 8 \
			-indicatoron 0} $Gui(WCA)
		pack $f.f.r$p -side left -padx 0
	}
	pack $f.l $f.f -side left -padx $Gui(pad) -fill x -anchor w

	#-------------------------------------------
	# Props->Bot->Basic frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic

	frame $f.fProgram  -bg $Gui(activeWorkspace)
	frame $f.fContents -bg $Gui(activeWorkspace)
	frame $f.fApply    -bg $Gui(activeWorkspace)
	pack $f.fProgram $f.fContents $f.fApply \
		-side top -fill x -pady $Gui(pad)

	#-------------------------------------------
	# Props->Bot->Advanced frame
	#-------------------------------------------
	set f $fProps.fBot.fAdvanced

	frame $f.fApply    -bg $Gui(activeWorkspace)
	pack $f.fApply \
		-side top -fill x -pady $Gui(pad)

	#-------------------------------------------
	# Props->Bot->Basic->Program frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic.fProgram

	eval {label $f.l -text "Program:" } $Gui(WLA)
	eval {entry $f.e -textvariable Options(program)} $Gui(WEA)
	pack $f.l -side left -padx $Gui(pad)
	pack $f.e -side left -padx $Gui(pad) -expand 1 -fill x

	#-------------------------------------------
	# Props->Bot->Basic->Contents frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic.fContents

	eval {label $f.l -text "Contents:" } $Gui(WLA)
	eval {entry $f.e -textvariable Options(contents)} $Gui(WEA)
	pack $f.l -side left -padx $Gui(pad)
	pack $f.e -side left -padx $Gui(pad) -expand 1 -fill x

	#-------------------------------------------
	# Props->Bot->Basic->Apply frame
	#-------------------------------------------
	set f $fProps.fBot.fBasic.fApply

	eval {button $f.bApply -text "Apply" \
		-command "OptionsPropsApply; RenderAll"} $Gui(WBA) {-width 8}
	eval {button $f.bCancel -text "Cancel" \
		-command "OptionsPropsCancel"} $Gui(WBA) {-width 8}
	grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)

	#-------------------------------------------
	# Props->Bot->Advanced->Apply frame
	#-------------------------------------------
	set f $fProps.fBot.fAdvanced.fApply

	eval {button $f.bApply -text "Apply" \
		-command "OptionsPropsApply; RenderAll"} $Gui(WBA) {-width 8}
	eval {button $f.bCancel -text "Cancel" \
		-command "OptionsPropsCancel"} $Gui(WBA) {-width 8}
	grid $f.bApply $f.bCancel -padx $Gui(pad) -pady $Gui(pad)

}

#-------------------------------------------------------------------------------
# .PROC OptionsSetPropertyType
# .END
#-------------------------------------------------------------------------------
proc OptionsSetPropertyType {} {
	global Options
	
	raise $Options(f$Options(propertyType))
}


#-------------------------------------------------------------------------------
# .PROC OptionsPropsApply
# .END
#-------------------------------------------------------------------------------
proc OptionsPropsApply {} {
	global Options Module Mrml

	# Validate program
	if {$Options(program) == ""} {
		tk_messageBox -message "Please enter a program that will recognize this option."
		return
	}
	if {[ValidateName $Options(program)] == 0} {
		tk_messageBox -message "The program can consist of letters, digits, dashes, or underscores"
		return
	}

	# Validate contents
	if {$Options(contents) == ""} {
		tk_messageBox -message "Please enter the contents of this option."
		return
	}
	if {[ValidateName $Options(contents)] == 0} {
		tk_messageBox -message "The contents can consist of letters, digits, dashes, or underscores"
		return
	}

	set m $Options(activeID)
	if {$m == ""} {return}

	if {$m == "NEW"} {
		# Ensure prefix not blank
		if {$Options(prefix) == ""} {
			tk_messageBox -message "Please enter a file prefix."
			return
		}
		set i $Options(nextID)
		incr Options(nextID)
		lappend Options(idList) $i
		vtkMrmlOptionsNode Options($i,node)
		set n Options($i,node)
		$n SetID               $i

		# These get set down below, but we need them before MainUpdateMRML
		$n SetProgram  $Options(program)
		$n SetContents $Options(contents)
#		$n SetOptions  $Options(options)

		Mrml(dataTree) AddItem $n
		MainUpdateMRML

		# If failed, then it's no longer in the idList
		if {[lsearch $Options(idList) $i] == -1} {
			return
		}
		set Options(freeze) 0
		MainOptionsSetActive $i
		set m $i
	}

	Options($m,node) SetProgram  $Options(program)
	Options($m,node) SetContents $Options(contents)
#	Options($m,node) SetOptions  $Options(options)

	# If tabs are frozen, then return to the "freezer"
	if {$Module(freezer) != ""} {
		set cmd "Tab $Module(freezer)"
		set Module(freezer) ""
		eval $cmd
	}
	
	MainUpdateMRML
}

#-------------------------------------------------------------------------------
# .PROC OptionsPropsCancel
# .END
#-------------------------------------------------------------------------------
proc OptionsPropsCancel {} {
	global Options Module

	# Reset props
	set m $Options(activeID)
	if {$m == "NEW"} {
		set m [lindex $Options(idList) 0]
	}
	set Options(freeze) 0
	MainOptionsSetActive $m

	# Unfreeze
	if {$Module(freezer) != ""} {
		set cmd "Tab $Module(freezer)"
		set Module(freezer) ""
		eval $cmd
	}
}
