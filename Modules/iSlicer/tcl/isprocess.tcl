#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: isprocess.tcl,v $
#   Date:      $Date: 2006/03/02 20:20:13 $
#   Version:   $Revision: 1.1 $
# 
#===============================================================================
# FILE:        isprocess.tcl
# PROCEDURES:  
#   isprocess_processcb
#   isprocess_demo
#==========================================================================auto=

# TODO - won't be needed once iSlicer is a package
package require Iwidgets

#########################################################
#
if {0} { ;# comment

isprocess - a widget for managing an external command line process 
 - executes command line as an external process
 - shows the stdout of the command in a scrolling text box
 - uses an istask to periodically update the output boxes
 - istask button is also used to initiate running the command 
   and to interrupt/stop execution

# TODO : 

}
#
#########################################################

#
# Default resources
# - sets the default colors for the widget components
#
option add *isprocess.commandline "" widgetDefault

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class isprocess] == "" } {

    itcl::class isprocess {
        inherit iwidgets::Labeledwidget

        constructor {args} {}
        destructor {}

        #
        # itk_options for widget options that may need to be
        # inherited or composed as part of other widgets
        # or become part of the option database
        #
        itk_option define -commandline commandline Taskcommand {}

        variable _w ""
        variable _commandline ""
        variable _task ""
        variable _cmdentry ""
        variable _outtext ""
        variable _fp ""

        method w {} {return $_w}

        method entrycallback {} {}
        method start {} {puts "start $itk_option(-commandline)"}
        method update_outtext {} {puts "update"}
        method cancel {} {puts "cancel"}
    }
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body isprocess::constructor {args} {
    component hull configure -borderwidth 0

    set _w [frame $itk_interior.f]
    pack $_w -fill both -expand true

    set _cmdentry $_w.cmdentry
    iwidgets::entryfield $_cmdentry \
        -labeltext "Command Line: " -textvariable [itcl::scope _commandline] \
        -command "$this entrycallback"
    pack $_cmdentry -side top -expand false -fill x

    set _task $_w.task
    istask $_task \
        -taskcommand "$this update_outtext" \
        -offcommand "$this cancel" \
        -oncommand "$this start"
    pack $_task -side top -expand false -fill x


    set _outtext $_w.outtext
    iwidgets::scrolledtext $_outtext -labeltext "Output: " -wrap none \
        -vscrollmode dynamic -hscrollmode dynamic \
        -width 5i -height 2i
    pack $_outtext -expand true -fill both
    
    eval itk_initialize $args
}


itcl::body isprocess::destructor {} {
}

itcl::body isprocess::entrycallback {} {
    
    $this configure -commandline $_commandline
}

itcl::body isprocess::start {} {
    
    $_outtext clear

    set _fp [open "| $_commandline" r]
    fconfigure $_fp -blocking 0
}

itcl::body isprocess::update_outtext {} {
    
    if { $_fp == "" } {
        return
    }

    if { [eof $_fp] } {
        $_task off
        return
    }

    set line [read $_fp]
    if { $line != "" } {
        $_outtext insert end $line
        $_outtext see end
    }
}

itcl::body isprocess::cancel {} {

    set ret [catch "close $_fp" res]
    set _fp ""
    if { $ret != 0 } {
        tk_messageBox -title "Process Result" -type ok -message "\nCommand line: \n\n  $_commandline\n\nreturned error:\n\n  $res"
    }
}

#-------------------------------------------------------------------------------
# .PROC isprocess_demo
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc isprocess_demo {} {

    catch "destroy .isprocessdemo"
    toplevel .isprocessdemo
    wm title .isprocessdemo "isprocessdemo"

    pack [isprocess .isprocessdemo.process -commandline "ls" ] -fill both -expand true

}
