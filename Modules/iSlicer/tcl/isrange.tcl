
package require Iwidgets


#########################################################
#
if {0} { ;# comment

isrange - a class for interacting with a min/max range

note: this is a widget

# TODO : 

}
#
#########################################################


#
# Default resources
# - sets the default colors for the widget components
#
option add *isrange.title "Range" widgetDefault
option add *isrange.command "" widgetDefault
option add *isrange.height 35 widgetDefault
# TODO: option add *isrange.minvariable "" widgetDefault
# TODO: option add *isrange.maxvariable "" widgetDefault
# TODO: option add *isrange.fromvariable "" widgetDefault
# TODO: option add *isrange.tovariable "" widgetDefault

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class isrange] == "" } {

    itcl::class isrange {
      inherit iwidgets::Labeledwidget

      constructor {args} {}
      destructor {}

      #
      # itk_options for widget options that may need to be
      # inherited or composed as part of other widgets
      # or become part of the option database
      #
      itk_option define -title title Title {}
      itk_option define -command command Command {}
      itk_option define -height height Height {}
      # TODO: itk_option define -tovariable tovariable Tovariable {}
      # TODO: itk_option define -fromvariable fromvariable Fromvariable {}
      # TODO: itk_option define -minvariable minvariable Minvariable {}
      # TODO: itk_option define -maxvariable maxvariable Maxvariable {}

      ### variables
      variable _name

      # is components for the controller
      variable _is3d
      variable _ismodel

      # drag state
      variable _start_fx
      variable _start_fmin
      variable _start_fmax
      variable _mode

      # min/max in [0-1] space
      variable _fmin 0.25
      variable _fmax 0.75

      # full range
      variable _from 0
      variable _to 100
      variable _fullmin 25
      variable _fullmax 75


      ### methods

      method is3d {} {return $_is3d}
      method ismodel {} {return $_ismodel}
      method bindings {} {return $_ismodel}
      method dragcb { state x y } {}
      method validate_set {min max} {}
      method validate_set_range {min max} {}
      method update_model {} {}
      method update_variables {which args} {}
      method set_bounds {f t} {set from $f; set to $t}
      method get_bounds {} {return "$from $to"}
      method range {} {}
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body isrange::constructor {args} {

    # make a unique name associated with this object
    set _name [namespace tail $this]
    # remove dots from name so it can be used in widget names
    regsub -all {\.} $_name "_" _name

    set cs [$this childsite]
    #
    # make an is3d widget 
    #
    set _is3d $cs.is3d_$_name
    ::is3d $_is3d -background #ffffff
    $_is3d configure -longitude 0 -latitude -90 -distance 1.9
    [$_is3d tkrw] configure -height 50 
    [[[.b.r is3d] ren] GetActiveCamera] ParallelProjectionOn
    pack forget [$_is3d controls]

    set _ismodel ::ismodel_$_name
    catch "delete object $_ismodel"
    ::ismodel $_ismodel 
    $this update_model
    [$_is3d ren] AddActor [$_ismodel actor]
    $this bindings

    #
    # pack em in a row
    #
    pack [iwidgets::spinfloat $cs.from -step 1 -width 5 -labeltext From -labelpos nw -textvariable [itcl::scope _from]] -side left
    pack [iwidgets::spinfloat $cs.min -step 1 -width 5 -labeltext Min -labelpos nw -textvariable [itcl::scope _fullmin]] -side left
    pack $_is3d -fill x -expand false -side left
    pack [iwidgets::spinfloat $cs.max -step 1 -width 5 -labeltext Max -labelpos nw -textvariable [itcl::scope _fullmax]] -side left
    pack [iwidgets::spinfloat $cs.to -step 1 -width 5 -labeltext To -labelpos nw -textvariable [itcl::scope _to]] -side left

    set _from 0
    set _to 100
    set _fullmin 25
    set _fullmax 75
    trace variable [itcl::scope _from] w "$this update_variables fromto" 
    trace variable [itcl::scope _to] w "$this update_variables fromto" 
    trace variable [itcl::scope _fullmin] w "$this update_variables minmax" 
    trace variable [itcl::scope _fullmax] w "$this update_variables minmax" 

    #
    # Initialize the widget based on the command line options.
    #
    eval itk_initialize $args
}


itcl::body isrange::destructor {} {
    delete object $_ismodel
    trace vdelete [itcl::scope _from] w "$this update_variables fromto" 
    trace vdelete [itcl::scope _to] w "$this update_variables fromto" 
    trace vdelete [itcl::scope _fullmin] w "$this update_variables minmax" 
    trace vdelete [itcl::scope _fullmax] w "$this update_variables minmax" 
}

# ------------------------------------------------------------------
#                             OPTIONS
# ------------------------------------------------------------------

itcl::configbody isrange::command {
}

# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------

itcl::body isrange::bindings { } {
    $_is3d bindings clear
    bind [$_is3d tkrw] <ButtonPress-1> "$this dragcb start %x %y"
    bind [$_is3d tkrw] <B1-Motion> "$this dragcb drag %x %y"
}

itcl::body isrange::dragcb {state x y} {

    set width [lindex [[$_is3d tkrw] configure -width] 4]
    set fx [expr $x / ($width * 1.0)]

    switch -- $state {
        "start" {
            set _start_fx $fx
            set _start_fmin $_fmin
            set _start_fmax $_fmax
            if { $fx < $_fmax && $fx > $_fmin } {
                set _mode "range"
            } 
            if { $fx < $_fmin } {
                set _mode "min"
            } 
            if { $fx > $_fmax } {
                set _mode "max"
            } 
        }
        "drag" {
            set deltax [expr $fx - $_start_fx]
            switch -- $_mode {
                "range" {
                    set fmin [expr $_start_fmin + $deltax]
                    set fmax [expr $_start_fmax + $deltax]
                    validate_set_range $fmin $fmax
                }
                "min" {
                    set fmin [expr $_start_fmin + $deltax]
                    validate_set $fmin $_fmax
                }
                "max" {
                    set fmax [expr $_start_fmax + $deltax]
                    validate_set $_fmin $fmax
                }
            }
            $this update_model
        }
    }
}

itcl::body isrange::update_model {} {

    set fcenter [expr 0.5 * ($_fmax + $_fmin)]
    set fradius [expr 0.5 * ($_fmax - $_fmin)]

    # convert to rendering space
    set mcenter [expr -20.0 * ($fcenter - 0.5)]
    set mradius [expr 20.0 * $fradius]
    if { $mradius < .1 } {
        set mradius .1
    }
    $_ismodel cylinder "$mcenter 0 0" $mradius

    [[$_ismodel actor] GetProperty] SetDiffuse 0
    $_is3d expose

}

#
# set the slider based on the values of the spinfloats
#
itcl::body isrange::update_variables {which args} {

    set fullrange [expr 1.0 * ($_to - $_from)]
    switch -- $which {
        "fromto" {
            # adjust entries to reflext slider
            set _fullmin [expr $_fmin * $fullrange]
            set _fullmax [expr $_fmax * $fullrange]
        }
        "minmax" {
            # adjust slider to reflect entries
            set _fmin [expr $_fullmin / $fullrange]
            set _fmax [expr $_fullmax / $fullrange]
            update_model
        }
    }

}

itcl::body isrange::validate_set_range {min max} {
    set range [expr $_fmax - $_fmin]
    if { $min < 0.0 } {
        set _fmin 0.0
        set _fmax $range
        return
    }
    if { $max > 1.0 } {
        set _fmax 1.0
        set _fmin [expr $_fmax - $range]
        return
    }
    if { $max < $min } {
        set max $min
    }
    set _fmin $min
    set _fmax $max
    $this range
}

itcl::body isrange::validate_set {min max} {

    if { $min < 0.0 } {
        set min 0.0
    }
    if { $max > 1.0 } {
        set max 1.0
    }
    if { $max < $min } {
        set max $min
    }
    set _fmin $min
    set _fmax $max
    $this range
}

itcl::body isrange::range {} {
    set fullrange [expr $_to - $_from]
    set ffmin $_fmin ;# save the state since the traced variable will reset the global
    set ffmax $_fmax
    set _fullmin [expr $_from + $fullrange * $ffmin]
    set _fullmax [expr $_from + $fullrange * $ffmax]
    return "$_fullmin $_fullmax"
}

