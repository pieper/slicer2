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
#  This module is an example for developers.  It shows how to add a module 
#  to the Slicer.  To find it when you run the Slicer, click on More->SessionLog.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#  Variables
#  These are (some of) the variables defined by this module.
# 
#  int SessionLog(count) counts the button presses for the demo 
#  list SessionLog(eventManager)  list of event bindings used by this module
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
    
    # Define Tabs
    #------------------------------------
    # Description:
    #   Each module is given a button on the Slicer's main menu.
    #   When that button is pressed a row of tabs appear, and there is a panel
    #   on the user interface for each tab.  If all the tabs do not fit on one
    #   row, then the last tab is automatically created to say "More", and 
    #   clicking it reveals a second row of tabs.
    #
    #   Define your tabs here as shown below.  The options are:
    #   
    #   row1List = list of ID's for tabs. (ID's must be unique single words)
    #   row1Name = list of Names for tabs. (Names appear on the user interface
    #              and can be non-unique with multiple words.)
    #   row1,tab = ID of initial tab
    #   row2List = an optional second row of tabs if the first row is too small
    #   row2Name = like row1
    #   row2,tab = like row1 
    #
    set m SessionLog
    set Module($m,row1List) "Help Start Log"
    set Module($m,row1Name) "{Help} {Start Here} {Log}"
    set Module($m,row1,tab) Start

    # Define Procedures
    #------------------------------------
    # Description:
    #   The Slicer sources all *.tcl files, and then it calls the Init
    #   functions of each module, followed by the VTK functions, and finally
    #   the GUI functions. A MRML function is called whenever the MRML tree
    #   changes due to the creation/deletion of nodes.
    #   
    #   While the Init procedure is required for each module, the other 
    #   procedures are optional.  If they exist, then their name (which
    #   can be anything) is registered with a line like this:
    #
    #   set Module($m,procVTK) SessionLogBuildVTK
    #
    #   All the options are:
    #
    #   procGUI   = Build the graphical user interface
    #   procVTK   = Construct VTK objects
    #   procMRML  = Update after the MRML tree changes due to the creation
    #               of deletion of nodes.
    #   procEnter = Called when the user enters this module by clicking
    #               its button on the main menu
    #   procExit  = Called when the user leaves this module by clicking
    #               another modules button
    #   procStorePresets  = Called when the user holds down one of the Presets
    #               buttons.
    #   procRecallPresets  = Called when the user clicks one of the Presets buttons
    #               
    #   Note: if you use presets, make sure to give a preset defaults
    #   string in your init function, of the form: 
    #   set Module($m,presets) "key1='val1' key2='val2' ..."
    #   
    set Module($m,procGUI) SessionLogBuildGUI
    set Module($m,procEnter) SessionLogEnter
    set Module($m,procExit) SessionLogExit

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
    set Module($m,depend) ""

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
	    {$Revision: 1.1 $} {$Date: 2001/02/22 15:22:09 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set SessionLog(count) 0
    set SessionLog(Volume1) $Volume(idNone)
    set SessionLog(Model1)  $Model(idNone)
    set SessionLog(FileName)  ""

    set SessionLog(eventManager)  ""
    
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
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    #-------------------------------------------
    # Start->Top frame
    #-------------------------------------------
    set f $fStart.fTop

    
    #-------------------------------------------
    # Start->Middle frame
    #-------------------------------------------
    set f $fStart.fMiddle
    
    # file browse box
    DevAddFileBrowse $f SessionLog FileName "Log File:" [] \
	    ".txt" [] "Open" "Select Log File" \
	    "Choose the log file for segmentation of this grayscale volume."

    #-------------------------------------------
    # Start->Bottom frame
    #-------------------------------------------
    set f $fStart.fBottom
    
    # make frames inside the Bottom frame for nice layout
    foreach frame "" {
	frame $f.f$frame -bg $Gui(activeWorkspace) 
	pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }

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
# .PROC SessionLogShowLog
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogShowLog {} {
    global Module
    
    # display the output that will be written to the file.
    foreach m $Module(idList) {
	if {[info exists Module($m,procSessionLog)] == 1} {
	    puts "LOG: $m"
	    $Module($m,procSessionLog)
	}
    }
}

#-------------------------------------------------------------------------------
# .PROC SessionLogWriteLog
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SessionLogWriteLog {} {
    global Module

    # append everything modules decided to record.
    set out [open $SessionLog(fileName) a]
    
    # Lauren fix
    # find last session number from file
    set number 1

    puts $out "\nSession Number $number"

    # Lauren fix
    # set date
    set date "00/00/00 at 17 pm!!!"
    puts $out $data

    foreach module $Module(idList) {
	if {[info exists $Module(procSessionLog)]} {
	    set info [$Module(procSessionLog)]
	    # dump to file
	    puts $out $info
	}
    }
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

    puts [lindex $SessionLog(fortunes) $index]
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