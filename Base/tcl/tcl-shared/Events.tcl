#=auto==========================================================================
# (c) Copyright 2001 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================
# FILE:        Events.tcl
# PROCEDURES:  
#   EventsInit
#   pushHandler widget event script
#   popHandler widget event
#   pushEventManager manager
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
# Define module dependencies, set version info of this module, initialize
# module-level variables.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc EventsInit {} {
    global Events Module
    
    set m Events
    
    # Define Dependencies
    set Module($m,depend) ""
    
    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.14 $} {$Date: 2002/03/22 20:37:49 $}]
    
    # Props
    set Events(managerStack) ""

}

#-------------------------------------------------------------------------------
# .PROC pushHandler
# Push the current event handler for this widget onto its stack.  
# Bind a new one.
# .ARGS
# str widget name of the widget 
# str event event you are interested in
# str script action to bind to the event
# .END
#-------------------------------------------------------------------------------
proc pushHandler { widget event script } {
    global Events

    set oldHandler [bind $widget $event]
    if { [info exists Events(handlers,$widget,$event)] } {
    set Events(handlers,$widget,$event) \
        [linsert $Events(handlers,$widget,$event) 0 $oldHandler]
    } else {
    set Events(handlers,$widget,$event) [list $oldHandler]
    }
    bind $widget $event $script
}
    
#-------------------------------------------------------------------------------
# .PROC popHandler
# Pops the handler off the stack and binds it.
# .ARGS
# str widget
# str event
# .END
#-------------------------------------------------------------------------------
proc popHandler { widget event } {
    global Events
    if { [llength $Events(handlers,$widget,$event)] == 0 } {
    unset Events(handlers,$widget,$event)
    } else {
    set script [lindex $Events(handlers,$widget,$event) 0]
    set Events(handlers,$widget,$event) \
        [lreplace $Events(handlers,$widget,$event) 0 0]
    bind $widget $event $script
    DebugMsg "unbinding $widget $event"
    }
}

#-------------------------------------------------------------------------------
# .PROC pushEventManager
#  Use this to set all event bindings for your module.
#  Call this from your procEnter function so that your widget
#  bindings are only in effect when the user is using your module.
# .ARGS
#  list manager
# .END
#-------------------------------------------------------------------------------
proc pushEventManager { manager } {
    global Events Gui

#    upvar 1 $mgr manager

#    foreach entry [array names manager] {
#    set item [split $entry ,]
#    set widget [lindex $item 0]
#    set event [lindex $item 1]
#    set command $manager($entry)
#    pushHandler $widget $event $command
#    }

    foreach entry $manager {
    set widget [subst [lindex $entry 0]]
    set event [lindex $entry 1]
    set command [lindex $entry 2]    
    pushHandler $widget $event $command
    DebugMsg "pushing $widget $event $command"
    }
    set Events(managerStack) [concat $manager $Events(managerStack)]
}

#-------------------------------------------------------------------------------
# .PROC popEventManager
#  Use this in conjunction with pushEventManager to set all event 
#  bindings for your module. It will pop the current event manager
#  (yours) and restore the previous one. 
#  Call this from your procExit function. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc popEventManager {} {
    #    global [lindex $EventManagerStack 0]
    global Events
    
    DebugMsg "pop top event Manager"
    
    set manager [lindex $Events(managerStack) 0]
    
    foreach entry [array names $manager] {
    set item [split $entry ,]
    set widget [subst [lindex $item 0]]
    set event [lindex $item 1]
    set command [subst $${manager}($entry)]
    popHandler $widget $event
    DebugMsg "poping $widget $event"
    }
    set Events(managerStack) [lreplace $Events(managerStack) 0 0]
}
#    set widget [lindex $manager 0]
#    set event  [lindex $manager 1]
#    set command [lindex $manager 2]
#    
#    popHandler $widget $event
#
#    set Events(managerStack) [lreplace $Events(managerStack) 0 0]
#    DebugMsg "poping $widget $event"

