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
# FILE:        Events.tcl
# PROCEDURES:  
#   EventsInit
#   pushHandler
#   popHandler
#   pushEventManager
#   popEventManager
#==========================================================================auto=
# Events.tcl
# 10/16/98 Peter C. Everett peverett@bwh.harvard.edu: Created

# This utility supports the pushing and popping of event handlers
# in order for different "tools" to have different bindings from
# existing ones, and to later restore the old bindings. -P. C. Everett
#
# An event manager is an array where the names take the form:
# widget,event and the values take the form: {command}.
# pushEventManager and popEventManager allow the pushing and popping
# of an entire set of event handlers, as in the case of changing
# tools.
#
######################################################################
#-------------------------------------------------------------------------------
# .PROC EventsInit
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EventsInit {} {
	global EventManagerStack Module

	set m Events

	# Define Dependencies
	set Module($m,depend) ""

        # Set version info
        lappend Module(versions) [ParseCVSInfo $m \
		{$Revision: 1.6 $} {$Date: 2000/02/28 17:56:22 $}]

	# Props
	set EventManagerStack ""

}

#-------------------------------------------------------------------------------
# .PROC pushHandler
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc pushHandler { widget event script } {
    global Handlers
    set oldHandler [bind $widget $event]
    if { [info exists Handlers($widget,$event)] } {
	set Handlers($widget,$event) \
		[linsert $Handlers($widget,$event) 0 $oldHandler]
    } else {
	set Handlers($widget,$event) [list $oldHandler]
    }
    bind $widget $event $script
}
	
#-------------------------------------------------------------------------------
# .PROC popHandler
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc popHandler { widget event } {
    global Handlers
    if { [llength $Handlers($widget,$event)] == 0 } {
	unset Handlers($widget,$event)
    } else {
	set script [lindex $Handlers($widget,$event) 0]
	set Handlers($widget,$event) \
		[lreplace $Handlers($widget,$event) 0 0]
	bind $widget $event $script
    }
}

#-------------------------------------------------------------------------------
# .PROC pushEventManager
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc pushEventManager { mgr } {
    global EventManagerStack Gui

    upvar 1 $mgr manager

    foreach entry [array names manager] {
	set item [split $entry ,]
	set widget [lindex $item 0]
	set event [lindex $item 1]
	set command $manager($entry)
	pushHandler $widget $event $command
    }
    set EventManagerStack [concat manager $EventManagerStack]
}

#-------------------------------------------------------------------------------
# .PROC popEventManager
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc popEventManager {} {
    global EventManagerStack
    global [lindex $EventManagerStack 0]
    
    set manager [lindex $EventManagerStack 0]
    foreach entry [array names $manager] {
	set item [split $entry ,]
	set widget [subst [lindex $item 0]]
	set event [lindex $item 1]
	set command [subst $${manager}($entry)]
	popHandler $widget $event
    }
    set EventManagerStack [lreplace $EventManagerStack 0 0]
}
