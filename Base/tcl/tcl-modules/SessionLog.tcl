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
# FILE:        SessionLog.tcl
# PROCEDURES:  
#   SessionLogInit
#   SessionLogBuildGUI
#   SessionLogEnter
#   SessionLogExit
#   SessionLogShowLog
#   SessionLogWriteLog
#   SessionLogReadLog
#   SessionLogShowRandomFortune
#   SessionLogInitRandomFortune
#==========================================================================auto=

#-------------------------------------------------------------------------------
#  Description
#  This module outputs a record of a segmentation session.
#  This will be used to compare efficiency of segmentation methods.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  Variables
#  These are the variables defined by this module.
# 
#  widget SessionLog(textBox)  the text box widget
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# .PROC SessionLogInit
#  The "Init" procedure is called automatically by the slicer.  
#  It puts information about the module into a global array called Module, 
#  and it also initializes module-level variables.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogInit {} {
    global SessionLog Module Volume Model
    
    set m SessionLog
    
    # Set up GUI tabs
    set Module($m,row1List) "Help Start Log"
    set Module($m,row1Name) "{Help} {Start Here} {Log}"
    set Module($m,row1,tab) Start

    # Register procedures that will be called 
    set Module($m,procGUI) SessionLogBuildGUI
    set Module($m,procEnter) SessionLogEnter
    set Module($m,procExit) SessionLogExit
    set Module($m,procSessionLog) SessionLogLog

    #   Record any other modules that this one depends on.
    set Module($m,depend) ""

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
	    {$Revision: 1.3 $} {$Date: 2001/02/27 23:45:37 $}]

    # Initialize module-level variables
    set SessionLog(fileName)  ""
    set SessionLog(currentlyLogging) 0

    # event bindings
    set SessionLog(eventManager)  ""

    # call init functions
    SessionLogInitRandomFortune
}


# NAMING CONVENTION:
#-------------------------------------------------------------------------------
#
# Use the following starting letters for names:
# t  = toplevel
# f  = frame
# mb = menubutton
# m  = menu
# b  = button
# l  = label
# s  = slider
# i  = image
# c  = checkbox
# r  = radiobutton
# e  = entry
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# .PROC SessionLogBuildGUI
#
# Create the Graphical User Interface.
# .END
#-------------------------------------------------------------------------------
proc SessionLogBuildGUI {} {
    global Gui SessionLog Module Volume Model
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Start" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(SessionLog,fStart)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
	# Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Start
    #   Top
    #   Middle
    #   Bottom
    #     FileLabel
    #     CountDemo
    # Bindings
    #   TextBox
    #-------------------------------------------
    
    #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The SessionLog module records info about your segmentation session
    for research purposes and to improve the Slicer.
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>Tons o' Start:</B> This tab is a demo for developers.
    "
    regsub -all "\n" $help {} help
    # remove emacs-style indentation from the 'html'
    regsub -all "    " $help {} help
    MainHelpApplyTags SessionLog $help
    MainHelpBuildGUI SessionLog
    
    #-------------------------------------------
    # Start frame
    #-------------------------------------------
    set fStart $Module(SessionLog,fStart)
    set f $fStart
    
    foreach frame "Top Middle Bottom" {
	frame $f.f$frame -bg $Gui(activeWorkspace)
	pack $f.f$frame -side top -padx $Gui(pad) -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # Start->Top frame
    #-------------------------------------------
    set f $fStart.fTop

    # file browse box
    DevAddFileBrowse $f SessionLog fileName "Log File:" [] \
	    "txt" [] "Save" "Select Log File" \
	    "Choose the log file for this grayscale volume."\
	    "Absolute"
    
    #-------------------------------------------
    # Start->Middle frame
    #-------------------------------------------
    set f $fStart.fMiddle
    
    DevAddLabel $f.lLogging "Logging is off."
    pack $f.lLogging -side top -padx $Gui(pad) -fill x
    set SessionLog(lLogging) $f.lLogging

    #-------------------------------------------
    # Start->Bottom frame
    #-------------------------------------------
    set f $fStart.fBottom
    
    # make frames inside the Bottom frame for nice layout
    foreach frame "Start" {
	frame $f.f$frame -bg $Gui(activeWorkspace) 
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    #-------------------------------------------
    # Start->Bottom->Start frame
    #-------------------------------------------
    set f $fStart.fBottom.fStart

    DevAddButton $f.bStart "Start Logging" SessionLogStartLogging
    TooltipAdd $f.bStart "Start logging each time before editing."

    pack $f.bStart -side top -padx $Gui(pad) -pady $Gui(pad)

    #-------------------------------------------
    # Log frame
    #-------------------------------------------
    set fLog $Module(SessionLog,fLog)
    set f $fLog

    foreach frame "TextBox" {
	frame $f.f$frame -bg $Gui(activeWorkspace) 
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

    $f.fTextBox config -relief groove -bd 3 
    
    #-------------------------------------------
    # Log->TextBox frame
    #-------------------------------------------
    set f $fLog.fTextBox

    # this is a convenience proc from tcl-shared/Developer.tcl
    DevAddButton $f.bBind "Show Current Log" SessionLogShowLog
    pack $f.bBind -side top -pady $Gui(pad) -padx $Gui(pad) -fill x
    
    # here's the text box widget from tcl-shared/Widgets.tcl
    set SessionLog(textBox) [ScrolledText $f.tText]
    pack $f.tText -side top -pady $Gui(pad) -padx $Gui(pad) \
	    -fill x -expand true

    
}

#-------------------------------------------------------------------------------
# .PROC SessionLogEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogEnter {} {
    global SessionLog
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $SessionLog(eventManager)

    # clear the text box
    $SessionLog(textBox) delete 1.0 end

}

#-------------------------------------------------------------------------------
# .PROC SessionLogExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager
}

proc SessionLogStartLogging {} {
    global SessionLog env
    
    if {$SessionLog(currentlyLogging) == 1} {
	tk_messageBox -message "Already logging."
	return
    }

    # make sure we have a filename.
    if {$SessionLog(fileName) == ""} {
	tk_messageBox -message "Please choose a filename first."
	return
    }
    
    # let users know we are logging
    set red [MakeColor "200 60 60"]
    $SessionLog(lLogging) config -text \
	    "Logging is on." \
	    -fg $red
    SessionLogShowRandomFortune

    # in case any module wants to know if we are logging or not
    set SessionLog(currentlyLogging) 1

    # set up things this module is going to log
    set SessionLog(log,startTime) [clock format [clock seconds]]
    # this may not work on PCs
    catch [set SessionLog(log,userName) $env(USER)]
}

proc SessionLogStopLogging {} {
    global SessionLog

    # final things this module will log
    set SessionLog(log,endTime) [clock format [clock seconds]]

}

proc SessionLogLog {} {
    global SessionLog

    set log "" 

    # get everything that was stored in the log part of the array
    set loglist [array names SessionLog log,*]
    puts $loglist

    # format the things this module will log
    foreach item $loglist {
	set name ""
	# get name without leading 'log,'
	regexp {log,(.*)} $item match name
	set val $SessionLog($item)
	set log "${log}${name}: ${val}\n"
    }

    # log something from this module
    return $log
}

#-------------------------------------------------------------------------------
# .PROC SessionLogShowLog
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogShowLog {} {
    global Module SessionLog
    
    # display the output that will be written to the file.
    set log ""
    foreach m $Module(idList) {
	if {[info exists Module($m,procSessionLog)] == 1} {
	    set log "${log}Module: $m\n[$Module($m,procSessionLog)]"
	}
    }

    # clear the text box
    $SessionLog(textBox) delete 1.0 end
    # put the new log there
    $SessionLog(textBox) insert end $log

}

#-------------------------------------------------------------------------------
# .PROC SessionLogWriteLog
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogWriteLog {} {
    global Module SessionLog

    # append everything modules decided to record.
    set out [open $SessionLog(fileName) a]
    
    # Lauren fix
    # find last session number from file
    set number 1

    puts $out "\nSession Number $number"

    # get the goods:
    foreach m $Module(idList) {
	if {[info exists Module($m,procSessionLog)] == 1} {
	    puts "LOGGING: $m"
	    set info [$Module($m,procSessionLog)]
	    puts $out "Module: $m"
	    puts $out $info
	}
    }

    close $out
    
}

#-------------------------------------------------------------------------------
# .PROC SessionLogReadLog
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogReadLog {} {
    global SessionLog

    # read in a log file for kicks.
    set in [open $SessionLog(fileName)]
    $SessionLog(textBox) insert end [read $in]
    close $in
}


#-------------------------------------------------------------------------------
# .PROC SessionLogShowRandomFortune
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogShowRandomFortune {} {
    global SessionLog

    set length [expr [llength $SessionLog(fortunes)] -1]
    set random [expr rand()]
    set index [expr round([expr $random * $length])]

    set fortune [lindex $SessionLog(fortunes) $index]

    tk_messageBox -message "Thanks for logging!\n\n$fortune"
    
}

#-------------------------------------------------------------------------------
# .PROC SessionLogInitRandomFortune
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogInitRandomFortune {} {
    global SessionLog

    set SessionLog(fortunes) {\
	    {'Outside of a dog, a book is man's best friend. Inside of a dog, it's too dark to read.'
    Groucho Marx} 
    {'I told my psychiatrist that everyone hates me. He said I was being ridiculous - everyone hasn't met me yet.'
    Rodney Dangerfield.}
    {'Did you ever walk in a room and forget why you walked in? I think that's how dogs spend their lives.'
    Sue Murphy}

}

}

