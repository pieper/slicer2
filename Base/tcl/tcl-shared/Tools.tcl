# Tools.tcl
# 11/30/98 Peter C. Everett peverett@bwh.harvard.edu: Created

#-------------------------------------------------------------------------------
# ToolsInit
# Initializes global variables used in creating/managing tool bars
#-------------------------------------------------------------------------------
proc ToolsInit {} {
	global env
	set home [file join $env(SLICER_HOME) program]
	set homebitmaps [file join $home bitmaps]

	foreach foo [exec ls [file join $home bitmaps]] {
		image create bitmap $foo -file [file join $homebitmaps $foo]
		}

	}

#-------------------------------------------------------------------------------
# ToolBar
# each element of args has the form {toolname toolimage toolenter toolexit}
# where the global variable "name(tool)" is set to "toolname" when the tool
# is selected, the bitmap image "toolimage" is used in the toolbar, and
# the routines "toolenter" and "toolexit" are called on entering and exiting
# each tool in the toolbar, respectively.
#-------------------------------------------------------------------------------
proc ToolBar { frame barname args } {
	global $barname Gui

	set f [frame $frame -bg $Gui(activeWorkspace) -cursor hand2]
	set ${barname}(frame) $f
	foreach tool $args {
		set toolname [lindex $tool 0]
		set toolimage [lindex $tool 1]
		set toolenter [lindex $tool 2]
		set toolexit [lindex $tool 3]
		set tooltip [lindex $tool 4]
		if { $toolenter == "" } {
			set toolenter ToolStub
			}
		if { $toolexit == "" } {
			set toolexit ToolStub
			}
		if { $tooltip == "" } {
			set tooltip $toolname
			}

		if { $toolimage == "" } {
			set c {radiobutton $f.rb$toolname -indicatoron 0 \
				-command "ToolUpdate $barname $toolenter $toolexit" \
				-text $toolname $Gui(WCA) \
				-variable ${barname}(tool) \
				-value $toolname }
			eval [subst $c]
		} else {
			set c {radiobutton $f.rb$toolname -indicatoron 0 \
				-command "ToolUpdate $barname $toolenter $toolexit" \
				-image $toolimage $Gui(WCA) \
				-variable ${barname}(tool) \
				-value $toolname }
			eval [subst $c]
			}
		pack $f.rb$toolname -side left
		TooltipAdd $f.rb$toolname $tooltip
		}

	set ${barname}(prevtool) ""
	set ${barname}(toolexit) ""
	pack $f

	return $f
}

proc ToolUpdate { barname toolenter toolexit } {
	global $barname

	# For debugging:
	# puts "ToolUpdate $barname $toolenter $toolexit"

	set toolname [subst $${barname}(tool)]
	set prevtool [subst $${barname}(prevtool)]
	if { $prevtool != $toolname } {
		set prevexit [subst $${barname}(toolexit)]
		if { $prevexit != "" } {
			$prevexit $prevtool
			}
		set ${barname}(toolexit) $toolexit
		set ${barname}(prevtool) $toolname
		if { $toolenter != "" } {
			$toolenter $toolname
			}
		}
	}

proc ToolStub { args } {
	}
