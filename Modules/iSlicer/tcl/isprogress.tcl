# - won't be needed once iSlicer is a package
package require Iwidgets

#########################################################
#
if {0} { ;# comment

isprogress - a widget for showing the progress of
a slow process
}
#
#########################################################

#
# Default resources
# - sets the default colors for the widget components
#
option add *isprogress.title    "Progress"  widgetDefault
option add *isprogress.geometry "400x300"   widgetDefault
option add *isprogress.initial_progress_text "Running" widgetDefault
option add *isprogress.cancel_text "Stop" widgetDefault

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class isprogress] == "" } {

    itcl::class isprogress {
        inherit iwidgets::Labeledwidget

        constructor {args} {}
        destructor { }

        variable _name          ""
        variable _w             ""
        variable _progress_text ""
        variable _geometry      ""
        variable _abort         ""
    variable _abort_command ""

        #
        # itk_options for widget options that may need to be
        # inherited or composed as part of other widgets
        # or become part of the option database
        #
    itk_option define -title title Title "Progress"
    itk_option define -geometry geometry Geometry "400x150"
    itk_option define -initial_progress_text initial_progress_text \
                       Initial_progress_text "Running"
    itk_option define -cancel_text cancel_text Cancel_text "Stop"
    itk_option define -abort_command abort_command Abort_command ""

        method w {}                        {return $_w}
        method update_progress_text {text} {set $_progress_text $text}
        method is_abort {}                 { return $_abort }
        method    abort {}                 { set _abort 1; $_abort_command }
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body isprogress::constructor {args} {
    set _name [namespace tail $this]
    set _w .progresswin
     set  _progress_text "::progress_$_name"
    set  _geometry      "400x15"
    set  _abort         "0"
    puts $_w

    global Gui

    catch {toplevel $_w}
    wm title $_w "Progress"
    wm geometry $_w $_geometry
    set $_progress_text "Running"

    frame $_w.f  -bg $Gui(backdrop) -relief sunken -bd 2
    pack $_w.f -expand yes -fill both
    eval {label $_w.f.l -textvariable $_progress_text} $Gui(BLA)
    pack $_w.f.l
    eval  {button $_w.f.bcancel -text "Run" -command "$this abort" } $Gui(WBA)
    # DevAddButton $_w.f.bcancel "Run" "$this abort"
    pack $_w.f.bcancel -pady 2

    update ;# make sure the window exists before grabbing events
    raise $_w
    catch "grab -global $_w"

    eval itk_initialize $args
}


itcl::body isprogress::destructor {} {
    catch {grab release $_w}
    catch {destroy $_w }
}

#-------------------------------------------------------------------------------
# OPTIONS: -title, -geometry,-cancel_text,-initial_progress_text
#
# DESCRIPTION: set the title of the window, size, text on cancel button,
#              and the initail text to report progress      
#
#-------------------------------------------------------------------------------
itcl::configbody isprogress::title    {  
   wm title $_w $itk_option(-title)
}
itcl::configbody isprogress::geometry { 
   wm geometry $_w $itk_option(-geometry)
}

itcl::configbody isprogress::cancel_text { 
   $_w.f.bcancel configure -text $itk_option(-cancel_text)
}

itcl::configbody isprogress::initial_progress_text { 
   set $_progress_text $itk_option(-initial_progress_text)
}

itcl::configbody isprogress::abort_command {
   set _abort_command $itk_option(-abort_command)
}


##############################

proc isprogress_demo {} {
    global isprogresscounter

    catch {destroy .isprogressdemo}
    set isprogresscounter 0

    isprogress .isprogressdemo \
        -title Yo -geometry 400x200  \
            -cancel_text "CANCEL" \
        -initial_progress_text "This is Good" 

    while {  [.isprogressdemo is_abort] == 0 } {
      update
    }
    destroy .isprogressdemo
}
