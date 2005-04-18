#=auto==========================================================================
# (c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.
#
# This software ("3D Slicer") is provided by The Brigham and Women's 
# Hospital, Inc. on behalf of the copyright holders and contributors. 
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for 
# research purposes only, provided that (1) the above copyright notice and 
# the following four paragraphs appear on all copies of this software, and 
# (2) that source code to any modifications to this software be made 
# publicly available under terms no more restrictive than those in this 
# License Agreement. Use of this software constitutes acceptance of these 
# terms and conditions.
# 
# 3D Slicer Software has not been reviewed or approved by the Food and 
# Drug Administration, and is for non-clinical, IRB-approved Research Use 
# Only.  In no event shall data or images generated through the use of 3D 
# Slicer Software be used in the provision of patient care.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
# OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
# NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
#
#===============================================================================
# FILE:        istask.tcl
# PROCEDURES:  
#   istask_taskcb
#   istask_demo
#==========================================================================auto=

# TODO - won't be needed once iSlicer is a package
package require Iwidgets

#########################################################
#
if {0} { ;# comment

istask - a widget for managing a background task with a 
configurable delay between invokations and an on/off button

# TODO : 
    - make it look better (maybe some bitmaps?)
}
#
#########################################################

#
# Default resources
# - sets the default colors for the widget components
#
option add *istask.taskcommand "" widgetDefault
option add *istask.oncommand "" widgetDefault
option add *istask.offcommand "" widgetDefault
option add *istask.taskdelay 30 widgetDefault

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class istask] == "" } {

    itcl::class istask {
        inherit iwidgets::Labeledwidget

        constructor {args} {}
        destructor {}

        #
        # itk_options for widget options that may need to be
        # inherited or composed as part of other widgets
        # or become part of the option database
        #
        itk_option define -taskcommand taskcommand Taskcommand {}
        itk_option define -oncommand oncommand Oncommand {}
        itk_option define -offcommand offcommand Offcommand {}
        itk_option define -taskdelay taskdelay Taskdelay 30

        variable _w ""
        variable _onoffbutton ""
        variable _mode "off"
        variable _taskafter ""

        method w {} {return $_w}
        method on {} {}
        method off {} {}
        method toggle {} {}
        method istask_taskcb {} {}

        proc stopall {} {}
    }
}

#-------------------------------------------------------------------------------
# .PROC istask_taskcb
# A placeholder so the task callback isn't undefined if the 
# class instance is destroyed before the after completes.
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc istask_taskcb {} {

}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body istask::constructor {args} {
    component hull configure -borderwidth 0

    set _w [frame $itk_interior.f]
    pack $_w -fill both -expand true

    set _onoffbutton [button $_w.oob -text "Start" -command "$this on"]
    pack $_onoffbutton -side left 
    
    eval itk_initialize $args
}


itcl::body istask::destructor {} {
    catch "after cancel $_taskafter"
}

itcl::body istask::on {} {
    $_onoffbutton configure -text "Running..." -command "$this off" -relief sunken
    if { $_mode == "off" } {
        eval $itk_option(-oncommand)
    }
    set _mode "on"
    $this istask_taskcb
}

itcl::body istask::off {} {
    $_onoffbutton configure -text "Start" -command "$this on" -relief raised
    if { $_mode == "on" } {
        eval $itk_option(-offcommand)
    }
    set _mode "off"
    $this istask_taskcb
}

itcl::body istask::toggle {} {
    if { $_mode == "off" } {
        $this on
    } else {
        $this off
    }
}

itcl::body istask::istask_taskcb {} {

    switch $_mode {
        "off" {
            if {$_taskafter != ""} {
                after cancel $_taskafter
            }
            set _taskafter ""
        }
        "on" {
            if {$itk_option(-taskcommand) != ""} {
                eval $itk_option(-taskcommand)
            }
            set _taskafter [after $itk_option(-taskdelay) "$this istask_taskcb"]
        }
    }
}

itcl::body istask::stopall {} {
    foreach t [itcl::find objects -class istask] {
        catch "$t off" 
    }
}

#-------------------------------------------------------------------------------
# .PROC istask_demo
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc istask_demo {} {
    global istaskcounter

    set istaskcounter 0

    catch "destroy .istaskdemo"
    toplevel .istaskdemo
    wm title .istaskdemo "istaskdemo"

    pack [istask .istaskdemo.task1 \
        -taskcommand ".istaskdemo.task1 configure -labeltext \[clock format \[clock seconds\]\]" \
        -taskdelay 30 ]

    pack [istask .istaskdemo.task2 \
        -taskcommand {
            global istaskcounter
            .istaskdemo.task2 configure -labeltext $istaskcounter
            incr istaskcounter} \
        -taskdelay 300 ]

    pack [button .istaskdemo.stopall -text stopall -command istask::stopall]
}
