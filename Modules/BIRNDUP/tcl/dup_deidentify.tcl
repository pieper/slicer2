#=auto==========================================================================
#   Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.
# 
#   See Doc/copyright/copyright.txt
#   or http://www.slicer.org/copyright/copyright.txt for details.
# 
#   Program:   3D Slicer
#   Module:    $RCSfile: dup_deidentify.tcl,v $
#   Date:      $Date: 2005/12/20 22:54:46 $
#   Version:   $Revision: 1.11.2.1 $
# 
#===============================================================================
# FILE:        dup_deidentify.tcl
# PROCEDURES:  
#==========================================================================auto=

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
        method runall {} {}
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
    pack [iwidgets::scrolledframe $f -hscrollmode dynamic -vscrollmode dynamic] -fill both -expand true -pady 15
    set _frame $f

    eval itk_initialize $args
}


itcl::body dup_deidentify::destructor {} {
}

itcl::body dup_deidentify::refresh {} {

    foreach w [winfo children $_frame] {
        destroy $w
    }

    set b 0
    foreach s [$parent studies] {
        if { ![file exists $s/deidentify_operations] } {
            tk_messageBox -message "Warning: no deidentify_operations for $s"
            continue
        }
        if { [file exists $s/ready_for_review] } {
            continue
        }
        set birnid [lindex [file split $s] end-3] 
        set bb $_frame.b$b 
        pack [button $bb -text "Deidentify $birnid" -command "$this run $s"]
        TooltipAdd $bb "$s"
        incr b
    }

    if { $b == 0 } {
        pack [label $_frame.l -text "Nothing to deidentify"]
    } else {
        pack [button $_frame.bspace -text "" -relief flat ]
        pack [button $_frame.ball -text "Deidentify All" -command "$this runall"]
    }
}

itcl::body dup_deidentify::runall {} {

    foreach s [$parent studies] {
        $this run $s
    }
}


itcl::body dup_deidentify::run {dir} {

    set fp [open $dir/deidentify_operations "r"]
    set ops [read $fp]
    close $fp

    ## fake

    set resp [DevOKCancel "Click Ok to run deidentification"]

    if { $resp == "ok" } {

        $parent log "starting deidentify of $dir"
        set dcanon_dir [$parent pref DCANON_DIR]
        set ::env(DCANON_DIR) $dcanon_dir
        set ::env(MRI_DIR) $dcanon_dir
        foreach op $ops {
            puts "executing $op"
            $parent log "executing $op"
            if { [catch "exec $dcanon_dir/$op" res] } {
                puts "$op failed: $res"
                $parent log "$op failed: $res"
            } else {
                puts "$op succeeded: $res"
                $parent log "$op succeeded: $res"
            }
        }
        $parent log "finished deidentify of $dir"

        $parent log "starting rendering of $dir"
        foreach ser [glob -nocomplain $dir/*-anon] {
            puts "rendering $ser"
            $parent log "rendering $ser"

            # this avoids warning messages when slicer starts
            set ::env(SLICER_CUSTOM_CONFIG) "true"

            set steps 15 ;# face 24 degrees per frame
            set skip 3 ;# slices show every 3 mm
            if { [catch "exec $::env(SLICER_HOME)/slicer2-linux-x86 --agree_to_license --load-dicom $ser --script $::env(SLICER_HOME)/Modules/iSlicer/tcl/evaluation-movies.tcl --exec eval_movies $ser/Deface $steps $skip ., exit" res] } {
                puts "$op failed: $res"
                $parent log "$op failed: $res"
            } else {
                puts "$op succeeded: $res"
                $parent log "$op succeeded: $res"
            }

        }
        $parent log "finished deidentify of $dir"

        close [open $dir/ready_for_review "w"]
        $parent refresh review
        $parent refresh deidentify

    } else {
        $parent log "bypassed deidentify of $dir"
    }

}

