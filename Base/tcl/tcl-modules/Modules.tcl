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
# FILE:        Modules.tcl
# DATE:        12/09/1999 14:15
# LAST EDITOR: gering
# PROCEDURES:  
#   ModulesInit
#   ModulesBuildGUI
#   ModulesApply
#   ModulesAll
#   ModulesOrderGUI
#   ModulesUp
#   ModulesDown
#==========================================================================auto=


#-------------------------------------------------------------------------------
# .PROC ModulesInit
# .END
#-------------------------------------------------------------------------------
proc ModulesInit {} {
	global Module Modules

	# Define Tabs
	set m Modules
	set Module($m,row1List) "Help Order"
	set Module($m,row1Name) "{Help} {Order}"
	set Module($m,row1,tab) Order

	# Define Procedures
	set Module($m,procGUI) ModulesBuildGUI

	foreach m $Module(idList) {
		set Module($m,visibility) 1
	}
}

#-------------------------------------------------------------------------------
# .PROC ModulesBuildGUI
# .END
#-------------------------------------------------------------------------------
proc ModulesBuildGUI {} {
	global Module Modules Gui

	#-------------------------------------------
	# Frame Hierarchy:
	#-------------------------------------------
	# Help
	# Order
	#-------------------------------------------

	#-------------------------------------------
	# Help frame
	#-------------------------------------------
	set help "
The next time you start the Slicer, it will load only the 
modules that have their buttons pressed in on the <B>Order</B> tab.
<P>
The order of modules will be determined by their order on 
the tab.  Click the <B>Up</B> and <B>Down</B> buttons to 
organize them.
"
	regsub -all "\n" $help {} help
	MainHelpApplyTags Modules $help
	MainHelpBuildGUI Modules

	#-------------------------------------------
	# Order frame
	#-------------------------------------------
	set fOrder $Module(Modules,fOrder)
	set f $fOrder

	frame $f.fTitle -bg $Gui(activeWorkspace)
	frame $f.fApply -bg $Gui(activeWorkspace)
	frame $f.fGrid -bg $Gui(activeWorkspace)
	pack $f.fTitle $f.fApply $f.fGrid -side top -pady $Gui(pad)

	#-------------------------------------------
	# Order->Title frame
	#-------------------------------------------
	set f $fOrder.fTitle

	set c {label $f.lTitle -text "\
Leave a button unpressed (out) for the 
Slicer to ignore that module the next 
time it runs." \
		$Gui(WLA)}; eval [subst $c]
	pack $f.lTitle

	#-------------------------------------------
	# Order->Apply frame
	#-------------------------------------------
	set f $fOrder.fApply

	set c {button $f.bApply -text "Apply" \
		-command "ModulesApply" $Gui(WBA)}; eval [subst $c]
	set c {button $f.bAll -text "Load ALL Modules" \
		-command "ModulesAll" $Gui(WBA)}; eval [subst $c]
	pack $f.bApply $f.bAll -side left -padx $Gui(pad)
	set Modules(bApply) $f.bApply
	set Modules(bAll) $f.bAll

	#-------------------------------------------
	# Order->Grid frame
	#-------------------------------------------
	set f $fOrder.fGrid
	set Modules(fOrder) $f

	ModulesOrderGUI
}

#-------------------------------------------------------------------------------
# .PROC ModulesApply
# .END
#-------------------------------------------------------------------------------
proc ModulesApply {} {
	global Module

	set ordList ""
	set supList ""
	foreach m $Module(idList) {
		if {$Module($m,visibility) == 1} {
			lappend ordList $m
		} else {
			lappend supList $m
		}
	}

	# Write the modules into 2 lists: ordered, suppressed
	if {$ordList != ""} {
		if {[catch {set ord [open OrderedModules.txt w]} errmsg] == 1} {
			puts $errmsg
			return
		}
		foreach m $ordList {
			puts $ord $m
		}
		close $ord
	}
	if {$supList != ""} {
		if {[catch {set sup [open SuppressedModules.txt w]} errmsg] == 1} {
			puts $errmsg
			return
		}
		foreach m $supList {
			puts $sup $m
		}
		close $sup
	}
}

#-------------------------------------------------------------------------------
# .PROC ModulesAll
# .END
#-------------------------------------------------------------------------------
proc ModulesAll {} {
	global Modules Module

	if {[catch {file delete OrderedModules.txt} errmsg] == 1} {
		puts $errmsg
	}
	if {[catch {file delete SuppressedModules.txt} errmsg] == 1} {
		puts $errmsg
	}
	$Modules(bApply) config -state disabled
	$Modules(bAll) config -state disabled
	foreach m $Module(idList) {
		set Module($m,visibility) 1
	}
	ModulesOrderGUI
}

#-------------------------------------------------------------------------------
# .PROC ModulesOrderGUI
# .END
#-------------------------------------------------------------------------------
proc ModulesOrderGUI {} {
	global Module Modules Gui
	
	set f $Modules(fOrder)
	foreach m $Module(idList) {

		# Delete from last time
		destroy $f.c$m
		destroy $f.bUp$m
		destroy $f.bDown$m

		# Name / Visible
		set c {checkbutton $f.c$m \
			-text $m -variable Module($m,visibility) -width 17 \
			-indicatoron 0 $Gui(WCA)}
			eval [subst $c]

		# Move buttons
		set c {button $f.bUp$m -text "Up" -width 3 \
			-command "ModulesUp $m" $Gui(WBA)}; eval [subst $c]
		set c {button $f.bDown$m -text "Down" -width 5\
			-command "ModulesDown $m" $Gui(WBA)}; eval [subst $c]

		grid $f.c$m $f.bUp$m $f.bDown$m -pady 2 -padx 5
	}
}

#-------------------------------------------------------------------------------
# .PROC ModulesUp
# .END
#-------------------------------------------------------------------------------
proc ModulesUp {m} {
	global Module

	set j [lsearch $Module(idList) $m]
	if {$j == 0} {return}
	set i [expr $j - 1]
	set n [lindex $Module(idList) $i]
	set Module(idList) [lreplace $Module(idList) $i $j $n]
	set Module(idList) [linsert  $Module(idList) $i $m]

	ModulesOrderGUI
}

#-------------------------------------------------------------------------------
# .PROC ModulesDown
# .END
#-------------------------------------------------------------------------------
proc ModulesDown {m} {
	global Module

	set i [lsearch $Module(idList) $m]
	if {$i == [expr [llength $Module(idList)] - 1]} {return}
	set j [expr $i + 1]
	set n [lindex $Module(idList) $j]
	set Module(idList) [lreplace $Module(idList) $i $j $m]
	set Module(idList) [linsert  $Module(idList) $i $n]

	ModulesOrderGUI
}

