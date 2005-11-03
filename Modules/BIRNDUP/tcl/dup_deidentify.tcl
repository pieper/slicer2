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

