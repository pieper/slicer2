
# TODO - won't be needed once iSlicer is a package
package require Iwidgets

#########################################################
#
if {0} { ;# comment

dup_review - the review pane of the birndup interface

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
if { [itcl::find class dup_review] == "" } {

    itcl::class dup_review {
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
itcl::body dup_review::constructor {args} {
    global env

    set cs [$this childsite]

    set f $cs.frame
    pack [iwidgets::scrolledframe $f -hscrollmode dynamic -vscrollmode dynamic] -fill both -expand true 
    set _frame $f

    eval itk_initialize $args
}


itcl::body dup_review::destructor {} {
}

itcl::body dup_review::refresh {} {

    foreach w [winfo children $_frame] {
        destroy $w
    }

    set defacedir [$parent pref DEFACE_DIR]
    set visits [glob -nocomplain $defacedir/Project_*/*/Visit_*/Study_*/Raw_Data]
    

    set b 0
    foreach v $visits {
        if { [file exists $v/ready_for_upload] } {
            continue
        }
        if { ![file exists $v/ready_for_review] } {
            continue
        }
        set birnid [lindex [file split $v] end-3] 
        set bb $_frame.b$b 
        pack [button $bb -text "Review $birnid" -command "$this run $v"]
        TooltipAdd $bb "$v"
        incr b
    }

    if { $b == 0 } {
        pack [label $_frame.l -text "Nothing to review"]
        return
    }
}

itcl::body dup_review::run {dir} {

    $parent log "starting review of $dir"

    tk_messageBox -message "Review of $dir" 

    $parent log "finished review of $dir"
    close [open $dir/ready_for_upload "w"]
    $parent refresh upload
    $parent refresh review
}

