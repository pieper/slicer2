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
# FILE:        isprogress.tcl
# PROCEDURES:  
#   isprogress_demo
#==========================================================================auto=
# - won't be needed once iSlicer is a package
package require Iwidgets

#########################################################
#
if {0} { ;# comment

isprogress - a widget for showing the progress of
             a slow process. Can control the main progress indicator
             Can also deal with abort commands.

        Use method "set_progress_text" for non vtk_widgets.
        Other just use -vtk_process_object to send the object
}
#
#########################################################

#
# Default resources
# - sets the default colors for the widget components
#
option add *isprogress.title             "Progress"    widgetDefault
option add *isprogress.geometry          "400x300"     widgetDefault
option add *isprogress.cancel_text       "Stop"        widgetDefault
option add *isprogress.use_main_progress "1"           widgetDefault
option add *isprogress.initial_progress_text "Running" widgetDefault
option add *isprogress.vtk_process_object    ""        widgetDefault

#########################################################

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class isprogress] == "" } {

    itcl::class isprogress {
        inherit iwidgets::Labeledwidget

        constructor {args} {}
        destructor { }

        variable _name              ""
        variable _w                 ""
        variable _progress_text     ""
        variable _geometry          ""
        variable _abort             ""
        variable _abort_command     ""
        variable _use_main_progress ""

        #
        # itk_options for widget options that may need to be
        # inherited or composed as part of other widgets
        # or become part of the option database
        #
        itk_option define -title title Title "Progress"
        itk_option define -geometry geometry Geometry "400x150"
        itk_option define -progress_text progress_text \
                           Progress_text "Running"
        itk_option define -cancel_text cancel_text Cancel_text "Stop"
        itk_option define -abort_command abort_command Abort_command ""
        itk_option define -use_main_progress use_main_progress Use_main_progress "1"
        ## only used if use_main_progress is defined
        itk_option define -vtk_process_object vtk_process_object Vtk_process_object ""

        method w {}                    {return $_w}
        method is_abort {}             { return $_abort }
        method    abort {}             { set _abort 1; eval "$_abort_command" }

        ## set the progress message text
        method set_progress_text {text} {}
        ## update progress
        method update_progress { ProcessObj}  { }
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body isprogress::constructor {args} {
    set _name [namespace tail $this]
    set _w .progresswin
    set  _progress_text "Running"
    set  _geometry      "400x15"
    set  _abort         "0"
    set  _use_main_progress "1"
    # puts $_w
    # puts $_name
    global Gui

    catch {toplevel $_w}
    wm title $_w "Progress"
    wm geometry $_w $_geometry
    set _progress_text "Running"

    frame $_w.f  -bg $Gui(backdrop) -relief sunken -bd 2
    pack $_w.f -expand yes -fill both
    eval {label $_w.f.l -text $_progress_text} $Gui(BLA)
    pack $_w.f.l
    eval  {button $_w.f.bcancel -text "Run" -command "$this abort" } $Gui(WBA)
    # DevAddButton $_w.f.bcancel "Run" "$this abort"
    pack $_w.f.bcancel -pady 2

    update ;# make sure the window exists before grabbing events
    raise $_w
    catch "grab $_w"

    eval itk_initialize $args
}

itcl::body isprogress::destructor {} {
    catch {grab release $_w}
    catch {destroy $_w }
    if {$_use_main_progress == 1} { MainEndProgress }
}

#------------------------------------------------------------------------------
# OPTIONS: -title, -geometry,-cancel_text,-progress_text
#
# DESCRIPTION: set the title of the window, size, text on cancel button,
#              and the initail text to report progress      
#
#------------------------------------------------------------------------------
itcl::configbody isprogress::title    {  
   wm title $_w $itk_option(-title)
}

#------------------------------------------------------------------------------
itcl::configbody isprogress::geometry { 
   wm geometry $_w $itk_option(-geometry)
}

#------------------------------------------------------------------------------

itcl::configbody isprogress::cancel_text { 
   $_w.f.bcancel configure -text $itk_option(-cancel_text)
}

#------------------------------------------------------------------------------

itcl::configbody isprogress::use_main_progress { 
   set _use_main_progress $itk_option(-use_main_progress)
   if {$_use_main_progress == 1} { 
       MainStartProgress 
       global Gui
       set Gui(progressText) $_progress_text
       puts "isprogress: $_progress_text"
   }
}

#------------------------------------------------------------------------------

itcl::configbody isprogress::vtk_process_object {
   if {$itk_option(-vtk_process_object) != ""} {
       $itk_option(-vtk_process_object) AddObserver ProgressEvent \
       "$this update_progress $itk_option(-vtk_process_object)"
   }
}

#------------------------------------------------------------------------------

itcl::configbody isprogress::progress_text { 
   $this set_progress_text $itk_option(-progress_text)
}

#------------------------------------------------------------------------------

itcl::body isprogress::update_progress { process_obj } {
    update
    MainShowProgress $process_obj
    update
    return 0
}

#------------------------------------------------------------------------------

itcl::body isprogress::set_progress_text { text } {
   set _progress_text $text
   $_w.f.l configure -text $_progress_text
   if {$_use_main_progress == 1} { 
     global Gui
     set Gui(progressText) "$_progress_text"
   }
}
#------------------------------------------------------------------------------

itcl::configbody isprogress::abort_command {
   set _abort_command $itk_option(-abort_command)
}

##############################

#-------------------------------------------------------------------------------
# .PROC isprogress_demo
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc isprogress_demo {} {
    global isprogresscounter

    catch {destroy .isprogressdemo}
    set isprogresscounter 0

    isprogress .isprogressdemo \
        -title "My Demo" -geometry 200x200  \
        -cancel_text "CANCEL ME" \
        -progress_text "This is Good" \
        -abort_command "puts done"

    while {  [.isprogressdemo is_abort] == 0 } {
        incr isprogresscounter
        after 300
        .isprogressdemo set_progress_text "Running $isprogresscounter"
        update
    }
    destroy .isprogressdemo
}

