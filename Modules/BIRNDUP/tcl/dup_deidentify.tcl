
# TODO - won't be needed once iSlicer is a package
package require Iwidgets

#########################################################
#
if {0} { ;# comment

dup_deidentify - the deidentify pane of the birndup interface

# TODO : 
    * make an about_dialog
    * the "view" method currently copies to temp dir - should fix the dicom
    loader that accepts a list of filenames
}
#
#########################################################

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class dup_deidentify] == "" } {

    itcl::class dup_deidentify {
        inherit iwidgets::Labeledwidget

        public variable parent ""

        variable _frame ""

        constructor {args} {}
        destructor {}

        method refresh {} {}
        method run {dir} {}
    }
}


# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body dup_deidentify::constructor {args} {
    global env

    set cs [$this childsite]

    set f $cs.frame
    pack [iwidgets::scrolledframe $f -hscrollmode dynamic -vscrollmode dynamic] -fill both -expand true 
    set _frame $f

    eval itk_initialize $args
}


itcl::body dup_deidentify::destructor {} {
}

itcl::body dup_deidentify::refresh {} {

    foreach w [winfo children $_frame] {
        destroy $w
    }

    set defacedir [$parent pref DEFACE_DIR]
    set visits [glob -nocomplain $defacedir/Project_*/*/Visit_*/Study_*/Raw_Data]


    set b 0
    foreach v $visits {
        if { ![file exists $v/deidentify_operations] } {
            tk_messageBox -message "Warning: no deidentify_operations for $v"
            continue
        }
        if { [file exists $v/ready_for_review] } {
            continue
        }
        set birnid [lindex [file split $v] end-3] 
        set bb $_frame.b$b 
        pack [button $bb -text "Run $birnid" -command "$this run $v"]
        TooltipAdd $bb "$v"
        incr b
    }

    if { $b == 0 } {
        pack [label $_frame.l -text "Nothing to deidentify"]
        return
    }
}

itcl::body dup_deidentify::run {dir} {

    set fp [open $dir/deidentify_operations "r"]
    set ops [read $fp]
    close $fp

    $parent log "starting deidentify of $dir"
    foreach op $ops {
        puts "running $op"
    }
    $parent log "finished deidentify of $dir"
    close [open $dir/ready_for_review "w"]
    $parent refresh review
    $parent refresh deidentify
}

