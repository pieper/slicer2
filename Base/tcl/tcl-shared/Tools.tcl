#=auto==========================================================================
# Copyright (c) 2000 Surgical Planning Lab, Brigham and Women's Hospital
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
# FILE:        Tools.tcl
# PROCEDURES:  
#   ToolUpdate
#   ToolStub
#==========================================================================auto=
# Tools.tcl
# 11/30/98 Peter C. Everett peverett@bwh.harvard.edu: Created

#-------------------------------------------------------------------------------
# ToolsInit
# Initializes global variables used in creating/managing tool bars
#-------------------------------------------------------------------------------
proc ToolsInit {} {
	global env
	set home [file join $env(SLICER_HOME) program]
	set homebitmaps [file join $home gui bitmaps]

	foreach foo [exec ls [file join $home gui bitmaps]] {
		if { [file extension $foo] == ".bmp" } {
		  image create bitmap $foo -file [file join $homebitmaps $foo]
		  }
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

#-------------------------------------------------------------------------------
# .PROC ToolUpdate
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------
# .PROC ToolStub
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc ToolStub { args } {
	}
