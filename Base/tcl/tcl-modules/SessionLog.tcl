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
#   SessionLogStartLogging
#   SessionLogGetVersionInfo
#   SessionLogSetFilenameAutomatically
#   SessionLogLog
#   SessionLogShowLog
#   SessionLogEndSession
#   SessionLogWriteLog
#   SessionLogReadLog
#   SessionLogShowRandomFortune
#   SessionLogInitRandomFortune
#   SessionLogStorePresets
#   SessionLogRecallPresets
#   SessionLogGenericLog
#   SessionLogFollowSliceOffsets
#   SessionLogTraceSliceOffsetsCallback
#   SessionLogTraceSliceDescriptionCallback
#   SessionLogStartTimingSlice
#   SessionLogStartTimingAllSlices
#   SessionLogStopTimingAllSlices
#   SessionLogStartTiming description
#   SessionLogStopTiming description
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
# This procedure can automatically log certain users if the path is set
# for logging and the file UsersToAutomaticallyLog exists.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogInit {} {
    global SessionLog Module Volume Model Path env
    set m SessionLog
    
    # Set up GUI tabs
    set Module($m,row1List) "Help Start Log"
    set Module($m,row1Name) "{Help} {Start Here} {Log}"
    set Module($m,row1,tab) Start

    # Register procedures that will be called 
    set Module($m,procGUI) SessionLogBuildGUI
    set Module($m,procEnter) SessionLogEnter
    set Module($m,procExit) SessionLogExit
    # the proc that will log things this module keeps track of
    set Module($m,procSessionLog) SessionLogLog

    # Set our presets
    #lappend Module(procStorePresets) SessionLogStorePresets
    #lappend Module(procRecallPresets) SessionLogRecallPresets
    #set Module(SessionLog,presets) ""

    #   Record any other modules that this one depends on.
    set Module($m,depend) ""

    # Set version info
    lappend Module(versions) [ParseCVSInfo $m \
	    {$Revision: 1.7 $} {$Date: 2001/04/09 20:35:39 $}]

    # Initialize module-level variables
    set SessionLog(fileName)  ""
    set SessionLog(currentlyLogging) 0
    set SessionLog(autoLogging) 0

    # default directory to log to (used for auto logging)
    # (should be set from Options.xml)
    set SessionLog(defaultDir) "/projects/slicer/logs/editorLogs"
    if {[file isdirectory $SessionLog(defaultDir)] == 0} {
	set SessionLog(defaultDir) ""
    }
    
    # event bindings
    set SessionLog(eventManager)  ""

    # call init functions
    #SessionLogInitRandomFortune

    # figure out if we should automatically log this user.
    # (this info should be in Options.xml but then people could save their
    # own and old Options.xml files would ruin the auto-logging experiment)

    # if username is set in the env global array
    if {[info exists env(LOGNAME)] == 1} {
	#puts "logname: $env(LOGNAME)"
	set logname $env(LOGNAME)


	# if the file listing which users to log exists
	# ExpandPath looks first in current dir then in program directory
	set filename [ExpandPath "UsersToAutomaticallyLog"]
	if {[file exists $filename]} {

	    #puts "file $filename found"
	    set in [open $filename]
	    set users [read $in]
	    close $in

	    # check and see if this user in file (then should be logged)
	    foreach user $users {
		#puts $user
		if {$logname == $user} {
		    #puts "match: $user == $logname"

		    # automatically log this user
		    set SessionLog(autoLogging) 1
		    puts "Automatically logging user $user.  Thanks!"
		    SessionLogStartLogging
		    SessionLogSetFilenameAutomatically
		}
	    }
	}
    }
}

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
    
    # if we are already logging (automatically)
    if {$SessionLog(currentlyLogging) == "1"} {
	set red [MakeColor "200 60 60"]
	$SessionLog(lLogging) config -text \
		"Logging is on." \
		-fg $red
    }

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

    DevAddButton $f.bStart "Start Logging" SessionLogStartLogging 15
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

#-------------------------------------------------------------------------------
# .PROC SessionLogStartLogging
# Start logging.  Set SessionLog(currentlyLogging) to 1, log initial items,
# start timing slice info, display random quote, the works.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogStartLogging {{tk "0"}} {
    global SessionLog env

    # if this proc was called by hitting a button, $tk ==1
    if {$tk == "1"} {
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
    }
    
    # make a funny joke
    #SessionLogShowRandomFortune $tk

    # in case any module wants to know if we are logging or not
    set SessionLog(currentlyLogging) 1

    # set up things this module is going to log
    set datatype "{{date,start}}"
    set SessionLog(log,$datatype) [clock format [clock seconds]]
    set datatype "{{misc,machine}}"
    set SessionLog(log,$datatype) [info hostname]
    # record user name
    set datatype "{{misc,user}}"
    set SessionLog(log,$datatype) ""
    if {[info exists env(LOGNAME)] == 1} {
	set SessionLog(log,$datatype) $env(LOGNAME)
    }

    SessionLogTraceSliceOffsets
    SessionLogGetVersionInfo
}

#-------------------------------------------------------------------------------
# .PROC SessionLogGetVersionInfo
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogGetVersionInfo {}  {
    global SessionLog Module

    # extract version info for each module
    # dependent on string formatting in $Module(versions)
    foreach ver $Module(versions) {
	set module [lindex $ver 0]
	set revision [lindex [lindex $ver 1] 1]
	set date [lindex [lindex $ver 2] 1]
	set time [lindex [lindex $ver 2] 2]
	set var "\{\{modversion,$module\},\{date,$date\},\{time,$time\}\}"
	set value "$revision"
	set SessionLog(log,$var) $value
    }
}

#-------------------------------------------------------------------------------
# .PROC SessionLogSetFilenameAutomatically
# create a unique log filename using the user's id and the time
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogSetFilenameAutomatically {} {
    global SessionLog

    # day of year (1-366), hour, min, sec, and year
    set formatstr "_%j_%H_%M_%S_%Y"
    set unique [clock format [clock seconds] -format $formatstr]
    set datatype "{{misc,user}}"
    set filename "$SessionLog(log,$datatype)$unique.log"
    set wholename [file join $SessionLog(defaultDir) $filename]
    
    # test if this has worked
    if {[file exists $wholename] == 1} {
	set wholename [file join $SessionLog(defaultDir) "$SessionLog(log,userName)${unique}Error.log"]
	puts "Automatically generated log filename already exists!"
    }

    puts "Session log will be automatically saved as $wholename"
    set SessionLog(fileName) $wholename
}


#-------------------------------------------------------------------------------
# .PROC SessionLogLog
# returns the things this module logs, in a formatted string
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogLog {} {
    global SessionLog

    # final things this module will log
    set datatype "{{date,end}}"
    set SessionLog(log,$datatype) [clock format [clock seconds]]


    # call generic function to grab all SessionLog(log,*)
    SessionLogGenericLog SessionLog
}

#-------------------------------------------------------------------------------
# .PROC SessionLogShowLog
# show the current stuff each module is keeping track of
# (the log file will be written when the user exits)
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
# .PROC SessionLogEndSession
#  called on program exit to finish logging and call WriteLog
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogEndSession {} {
    global SessionLog

    if {$SessionLog(currentlyLogging) == 0} {
	return
    }
	
    if {$SessionLog(fileName) == ""} {
	SessionLogSetFilenameAutomatically
    }

    # make sure we record final times
    SessionLogStopTimingAllSlices    

    puts "Saving session log file as $SessionLog(fileName)"

    SessionLogWriteLog

}

#-------------------------------------------------------------------------------
# .PROC SessionLogWriteLog
#  actually grab log info from all modules and write the file
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogWriteLog {} {
    global Module SessionLog

    # append everything modules decided to record.
    set out [open $SessionLog(fileName) a]
    
    # get the goods:
    foreach m $Module(idList) {
	if {[info exists Module($m,procSessionLog)] == 1} {
	    puts "LOGGING: $m"
	    set info [$Module($m,procSessionLog)]

	    set info "\{\n\{_Module $m\}\n\{$info\}\n\}\n"
	    #puts $out "\n_Module $m"
	    #puts $out "\n_ModuleLogItems"
	    puts $out $info
	    #puts $out "\n_EndModule"
	}
    }

    close $out
    
}

#-------------------------------------------------------------------------------
# .PROC SessionLogReadLog
# read in a log (text) file and display in the box
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
# pop up a message box or use cout to display a random quote
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogShowRandomFortune {{tk "0"}} {
    global SessionLog

    set length [expr [llength $SessionLog(fortunes)] -1]
    set random [expr rand()]
    set index [expr round([expr $random * $length])]

    set fortune [lindex $SessionLog(fortunes) $index]

    set message "Thanks for logging!\n\n$fortune\n"
    if {$tk == "1"} {
	tk_messageBox -message $message
    } else {
	puts $message
    }
    
}

#-------------------------------------------------------------------------------
# .PROC SessionLogInitRandomFortune
# init the list of quotes
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

#-------------------------------------------------------------------------------
# .PROC SessionLogStorePresets
#  Put things into the presets Options node in Options.xml
# (This file is read in when slicer starts up)
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogStorePresets {p} {
    global Preset SessionLog

    # store the current default directory
    set Preset(SessionLog,defaultDir) $SessionLog(defaultDir) 

}

#-------------------------------------------------------------------------------
# .PROC SessionLogRecallPresets
# Get startup info from the Options.xml file (through the Preset array)
# This is used for a configurable default storage dir for automatic
# logging.  Edit slicer/program/Options.xml to change this directory.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogRecallPresets {p} {
    global Preset SessionLog
    
    # test if the default dir exists
    if {[info exists Preset(SessionLog,defaultDir)] == 1} {
	set dir $Preset(SessionLog,defaultDir)
	if {[file isdirectory $dir] == 1} {
	    set SessionLog(defaultDir) $dir
	} else {
	    puts "Error in SessionLog: default directory from Options.xml does not exist"
	}
    } else {
	puts "SessionLog: no default dir in Options.xml."
    }
    # get the user names we should log for. ???
}


########################################################
# Utility functions to help other modules log
######################################################

#-------------------------------------------------------------------------------
# .PROC SessionLogGenericLog
# generic logging procedure 
# which grabs everything in $m(log,*) and returns a formatted 
# string that can then be returned by a module's logging procedure
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogGenericLog {m} {
    global $m

    set log "" 

    # get everything that was stored in the log part of the array
    set loglist [array names $m log,*]
    #puts $loglist

    # format the things this module will log
    foreach item $loglist {
	set name ""
	# get name without leading 'log,'
	regexp {log,(.*)} $item match name
	# get matching value stored in array
	eval {set val} \$${m}($item)
	# append to list
	lappend log "\{$name   \{$val\}\}"
	#set log "${log}\{${name}: ${val}\}\n"
    }

    # alphabetize the list
    set alpha [lsort -dictionary $log]

    # add newlines between items so it's readable
    set final ""
    foreach item $alpha {
	set final "$final\n$item"
    }

    return $final
}

#-------------------------------------------------------------------------------
# .PROC SessionLogFollowSliceOffsets
# uses tcl "trace variable" command to be notified whenever slice offsets change.
# this allows timing of editing, etc., per slice without hacking MainSlices.
# also traces other variables used in the "description" of the slice time
# (like current label, etc.) so that we can time, for example, time per slice per label.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogTraceSliceOffsets {}  {
    global Slice SessionLog
    global Module Editor Label
    foreach s $Slice(idList) {
	# callback will be called whenever variable is written to ("w")
	trace variable Slice($s,offset) w SessionLogTraceSliceOffsetsCallback

	# initialize vars
	if {[info exists SessionLog(trace,prevSlice$s)] == 0} {
	    set SessionLog(trace,prevSlice$s) ""
	}
    }

    # trace all variables we are using as part of the slice time description
    # this is needed to keep an accurate time count...
    # otherwise we won't start and stop timing when description 
    # changes and none of this will work.

    set varlist "{Module(activeID)} {Editor(activeID)} {Editor(idWorking)} \
	    {Editor(idOriginal)} {Label(label)} {Slice(activeID)}"

    foreach var $varlist {
	trace variable $var w SessionLogTraceSliceDescriptionCallback
	#puts $var
	#eval {puts } \$$var
    }

    return

    trace variable Module(activeID) w SessionLogTraceSliceDescriptionCallback
    trace variable Editor(activeID) w SessionLogTraceSliceDescriptionCallback
    trace variable Editor(idWorking) w SessionLogTraceSliceDescriptionCallback
    trace variable Editor(idOriginal) w SessionLogTraceSliceDescriptionCallback
    trace variable Label(label) w SessionLogTraceSliceDescriptionCallback
    trace variable Slice(activeID) w SessionLogTraceSliceDescriptionCallback
}

#-------------------------------------------------------------------------------
# .PROC SessionLogTraceSliceOffsetsCallback 
# called when slice offset tcl var changes.
# initiates timing of time spent in slice
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogTraceSliceOffsetsCallback {variableName indexIfArray operation} {
    global Slice Module Editor


    # we only care about editing time per slice
    if {$Module(activeID) != "Editor"} {
	return
    }

    # Lauren what if slice is not active?

    # get slice number
    upvar 1 $variableName var
    set num $var($indexIfArray)

    # get slice id number
    set s [lindex [split $indexIfArray ","] 0]

    SessionLogStartTimingSlice $s
}

#-------------------------------------------------------------------------------
# .PROC SessionLogTraceSliceDescriptionCallback
# called when any var that is used as part of the description of
# the slice time changes.  restarts timing of all slices w/ new description
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogTraceSliceDescriptionCallback {variableName indexIfArray operation} {
    
    SessionLogStartTimingAllSlices

}

#-------------------------------------------------------------------------------
# .PROC SessionLogStartTimingSlice
# Form description string that describes current situation we are timing.
# Record start time for this description. Stop timing previous description.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogStartTimingSlice {s} {
    global SessionLog Slice Module Editor Label

    # this copies a bunch of Editor code, it could all be in same place...

    # form description of exact event
    # key-value pairs describing the event
    set datatype "{sliceTime,elapsed}"
    set module "{module,$Module(activeID)}"
    set submodule "{submodule,$Editor(activeID)}"
    set workingid "{workingid,$Editor(idWorking)}"
    set originalid "{originalid,$Editor(idOriginal)}"
    set label "{label,$Label(label)}"
    set slice  "{slice,$s}"
    set sliceactive  "{sliceactive,$Slice(activeID)}"
    set slicenum "{slicenum,$Slice($s,offset)}"
    #set eventinfo "{eventinfo,}"
    set var "\{$datatype,$module,$submodule,$workingid,$originalid,$label,$slice,$slicenum\}"
    
    # previous slice timing description
    set prev $SessionLog(trace,prevSlice$s)

    # actually record the time
    if {$var != $prev} {

	SessionLogStartTiming $var
	
	# stop timing previous slice
	if {$prev != ""} {
	    SessionLogStopTiming $prev
	}
	# remember this one for next time
	set SessionLog(trace,prevSlice$s)  $var
    }
}


#-------------------------------------------------------------------------------
# .PROC SessionLogStartTimingAllSlices
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogStartTimingAllSlices {} {
    global SessionLog Slice
    
    foreach s $Slice(idList) {
	SessionLogStartTimingSlice $s
    }

}

#-------------------------------------------------------------------------------
# .PROC SessionLogStopTimingAllSlices
# Only use this before exiting the program to record final time
# for all slices
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogStopTimingAllSlices {} {
    global SessionLog Slice
    
    foreach s $Slice(idList) {

	# stop timing previous slice
	set prev $SessionLog(trace,prevSlice$s)
	if {$prev != ""} {
	    SessionLogStopTiming $prev
	}
	# clear the previous slice since we are not timing one
	set SessionLog(trace,prevSlice$s)  ""

    }

}

#-------------------------------------------------------------------------------
# .PROC SessionLogStartTiming
# grab clock value now
# .ARGS
# d  description pseudo-list of whatever we are timing
# .END
#-------------------------------------------------------------------------------
proc SessionLogStartTiming {d} {
    global SessionLog

    set SessionLog(logInfo,$d,startTime) [clock seconds]
}

#-------------------------------------------------------------------------------
# .PROC SessionLogStopTiming
# add to the total time in editor (or submodule) so far
# .ARGS
# d  description pseudo-list of whatever we are timing
# .END
#-------------------------------------------------------------------------------
proc SessionLogStopTiming {d} {
    global SessionLog

    # can't stop if we never started
    if {[info exists SessionLog(logInfo,$d,startTime)] == 0} {
	return
    }

    set SessionLog(logInfo,$d,endTime) [clock seconds]
    set elapsed \
	    [expr $SessionLog(logInfo,$d,endTime) - $SessionLog(logInfo,$d,startTime)]
    
    # variable name is a list describing the exact event
    # the first thing is datatype: time is the database table
    # it should go in, and elapsed describes the type of time...
    set var $d

    # initialize the variable if needed
    if {[info exists SessionLog(log,$var)] == 0} {
	set SessionLog(log,$var) 0
    }
    
    # increment total time
    set total [expr $elapsed + $SessionLog(log,$var)]
    set SessionLog(log,$var) $total    
}

