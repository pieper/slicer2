
# TODO - won't be needed once iSlicer is a package
package require Iwidgets

#########################################################
#
if {0} { ;# comment

dup_upload - the upload pane of the birndup interface

# TODO : 
    * make an about_dialog
    * the "view" method currently copies to temp dir 
      - should fix the slicer dicom loader to accept a list of filenames
}
#
#########################################################

#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class dup_upload] == "" } {

    itcl::class dup_upload {
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
itcl::body dup_upload::constructor {args} {
    global env

    set cs [$this childsite]

    set f $cs.frame
    pack [iwidgets::scrolledframe $f -hscrollmode dynamic -vscrollmode dynamic] -fill both -expand true -pady 15
    set _frame $f

    eval itk_initialize $args
}


itcl::body dup_upload::destructor {} {
}

itcl::body dup_upload::refresh {} {

    foreach w [winfo children $_frame] {
        destroy $w
    }

    set defacedir [$parent pref DEFACE_DIR]
    set studies [$parent studies]
    

    set b 0
    foreach s $studies {
        if { ![file exists $s/ready_for_upload] } {
            continue
        }
        if { [file exists $s/uploaded] } {
            continue
        }
        set birnid [lindex [file split $s] end-3] 
        set bb $_frame.b$b 
        pack [button $bb -text "Upload $birnid" -command "$this run $s"]
        TooltipAdd $bb "$s"
        incr b
    }

    if { $b == 0 } {
        pack [label $_frame.l -text "Nothing to upload"]
        return
    }
}

itcl::body dup_upload::run {dir} {

    $parent log "starting upload of $dir"


    if { [DevOKCancel "Upload of $dir complete.  Temp copy will now be deleted." ] == "ok" } {
        file delete -force $dir
    } else {
        close [open $studydir/uploaded "w"]
    }

    $parent log "finished upload of $dir"
    $parent refresh upload
}

